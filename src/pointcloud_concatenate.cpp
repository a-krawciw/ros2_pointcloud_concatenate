#include "pointcloud_concatenate/pointcloud_concatenate.hpp"
#include <algorithm>

// Constructor
PointcloudConcatenate::PointcloudConcatenate() : Node("pointcloud_concatenate")
{
  // Initialise variables / parameters to class variables
  param_frame_target_ = declare_parameter<std::string>("target_frame", "base_link");
  param_clouds_ = declare_parameter<int>("clouds", 2);
  param_hz_ = declare_parameter<double>("hz", 10.0);

  clouds_.resize(param_clouds_);
  
  // Queues size of 1 to only keep the most recent message

  for (unsigned i = 0; i < param_clouds_; ++i) {
    const auto topic_i = declare_parameter<std::string>("cloud_in" + std::to_string(i+1) + "_topic", "cloud_in" + std::to_string(i+1));
    const auto pc_cb_i = [i, this](const PointCloudMsg::SharedPtr msg) {this->subCallbackCloud(msg, i);};
    pc_subs_.push_back(create_subscription<sensor_msgs::msg::PointCloud2>(topic_i, 1, pc_cb_i));
  }

  // Initialization tf2 listener
  tf_buffer_ = std::make_shared<tf2_ros::Buffer>(this->get_clock());
  tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);

  // Initialise publishers and subscribers
  pub_cloud_out_ = this->create_publisher<sensor_msgs::msg::PointCloud2>(declare_parameter<std::string>("cloud_out_topic", "cloud_out"), 1);

}

// Destructor
PointcloudConcatenate::~PointcloudConcatenate()
{
  // Free up allocated memory
  RCLCPP_INFO(this->get_logger(), "Destructing PointcloudConcatenate...");
  // delete pointer_name;
}

void PointcloudConcatenate::subCallbackCloud(const PointCloudMsg::SharedPtr msg, const int index)
{
  clouds_[index] = msg;
}


double PointcloudConcatenate::getHz()
{
  return param_hz_;
}

void PointcloudConcatenate::update()
{
  // Is run periodically and handles calling the different methods
  if (pub_cloud_out_->get_subscription_count() == 0)
  {
    return;
  }

  if (std::all_of(clouds_.begin(), clouds_.end(), [](const PointCloudMsg::SharedPtr pc ) {return pc == nullptr;}))
  {
    RCLCPP_WARN(this->get_logger(), "No pointclouds received yet. Sleeping 1s...");
    return;
  }

  // Initialise pointclouds
  sensor_msgs::msg::PointCloud2 cloud_out; // Clear the output pointcloud

  unsigned i = 0;
  for (const auto& pc_ptr : clouds_) {
    sensor_msgs::msg::PointCloud2 cloud_to_concat;

    if (pc_ptr == nullptr) { 
      RCLCPP_WARN(this->get_logger(), "No cloud received for index %d", i);
      continue;
    }

    // Track success of transforms
    bool success = pcl_ros::transformPointCloud(param_frame_target_, *pc_ptr, cloud_to_concat, *tf_buffer_);
    if (!success)
    {
      RCLCPP_WARN(this->get_logger(), "Transforming cloud from %s to %s failed! Skipping.", pc_ptr->header.frame_id.c_str(), param_frame_target_.c_str());
    } else {
      pcl::concatenatePointCloud(cloud_out, cloud_to_concat, cloud_out);
    }
    ++i;
  }


  // Publish the concatenated pointcloud
  if (cloud_out.height > 0)
  {
    publishPointcloud(cloud_out);
  }
}

void PointcloudConcatenate::publishPointcloud(sensor_msgs::msg::PointCloud2 &cloud)
{
  // Publishes the combined pointcloud

  // Update the timestamp
  cloud.header.stamp = this->now();
  cloud.header.frame_id = param_frame_target_;
  // Publish
  pub_cloud_out_->publish(cloud);
}