#!/usr/bin/env python3
#coding=utf-8

import rospy
from sensor_msgs.msg import LaserScan

def LidarCallback(msg):
    dist = msg.ranges[180]
    rospy.loginfo("前方测距 ranges[180] = %f 米",dist)

if __name__ == "__main__":
    rospy.init_node("lidar_node")
    lidar_sub = rospy.Subscriber("/scan",LaserScan,LidarCallback,queue_size=10)
    rospy.spin()