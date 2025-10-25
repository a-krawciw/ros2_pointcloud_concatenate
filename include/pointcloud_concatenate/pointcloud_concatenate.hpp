#pragma once                       // Only include once per compile
#ifndef POINTCLOUD_CONCATENATE_HPP // Conditional compiling
#define POINTCLOUD_CONCATENATE_HPP

// Includes
#include <rclcpp/rclcpp.hpp> // ROS header

#include <tf2_ros/transform_listener.h>
#include <tf2_ros/buffer.h>

#include <pcl_ros/transforms.hpp>
#include <pcl_conversions/pcl_conversions.h>

#include <sensor_msgs/msg/point_cloud2.hpp>

// Macro to warn about unset parameters
#define PARAM_WARN(param_name, default_val)                                 \
  std::cout << "\033[33m"                                                   \
            << "[WARN] Param is not set: " << param_name                    \
            << ". Setting to default value: " << default_val << "\033[0m\n" \
            << std::endl

// Define class
class PointcloudConcatenate : public rclcpp::Node
{
public:
  using PointCloudMsg = sensor_msgs::msg::PointCloud2;

  // Constructor and destructor
  PointcloudConcatenate();
  ~PointcloudConcatenate();

  // Public functions
  void update();
  double getHz();


private:
  // Parameters
  std::string param_frame_target_;
  int param_clouds_;
  double param_hz_;

  // Publisher and subscribers
  std::vector<rclcpp::Subscription<PointCloudMsg>::SharedPtr> pc_subs_;
  rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pub_cloud_out_;

  // Private functions
  void subCallbackCloud(const PointCloudMsg::SharedPtr msg, const int index);
  void publishPointcloud(sensor_msgs::msg::PointCloud2 &cloud);

  // Other
  std::vector<PointCloudMsg::SharedPtr> clouds_;

  // Initialization tf2 listener
  std::shared_ptr<tf2_ros::Buffer> tf_buffer_;
  std::shared_ptr<tf2_ros::TransformListener> tf_listener_;
};

#endif // POINTCLOUD_CONCATENATE_HPP