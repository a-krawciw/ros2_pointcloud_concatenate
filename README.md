# Concatenate / combining pointclouds and outputing a single concatenated pointcloud

ROS2 fork of https://github.com/aseligmann/pointcloud_concatenate.

## `pointcloud_concatenate`

This package provides a node which can be used for concatenating several pointclouds into one.

The parameter `clouds` determines how many subscriptions are created.

### **Dependencies**

This package depends on the following libraries

* `pcl`
* `pcl_ros`

### **Installation**

To use simply clone the package into your workspace and integrate the node into your launch file.

Make sure the dependencies are installed by running rosdep.

```bash
rosdep install --from-paths src --ignore-src -r -y
```

---

### **Launch files**

This node is intended to be included as a part of a larger launch file.
And example launch file is provided.

* `pointcloud_concatenate.launch.py`  
  A sample launch file which highlights the necessary parameters and topics.  
  Launches a node which subscribes to 2 pointclouds, concatenates them and publishes the
  concatenated pointcloud in frame `base_link` at a frequency of 10 Hz.

### **ROS topics**

The package uses the following topics, which can be changed in ```concatenate_params.yaml```.

#### Publishers

* `cloud_out` - [`sensor_msgs/PointCloud2`]  
  The concatenated pointcloud.

#### Subscribers

* `cloud_in{i}` - [`sensor_msgs/PointCloud2`]  
  Each cloud i starting from 1.
* `tf` - [`geometry_msgs/TransformStamped`]
  The TF tree for aligning the point clouds.
### **ROS2 parameters**

* `clouds` - [integer]  
  Sets the number of pointclouds to concatenate.  
  Default value: `2`.
* `target_frame` - [a valid frame_id]  
  Sets the frame_id which the pointclouds will be collected in before concatenation.  
  `cloud_out` will be in this frame.  
  Default value: `base_link`.
* `hz` - [double]  
  Sets the frequency at which the output is updated and published.  
  The input pointclouds should be publishing faster or at an equal rate to this frequency.  
  If an input pointclouds is not received anew before a new update cycle, the previous value will be used.  
  Default value: `10.0`.
* `cloud_in*_topic` - [string]  
  Topics at which [`sensor_msgs/PointCloud2`] messages are received.
  There must be the same amount of topics set as the `clouds` otherwise an exception will be thrown.
