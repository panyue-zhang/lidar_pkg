#!/usr/bin/env python3
#coding=utf-8
import rospy
from sensor_msgs.msg import LaserScan
from geometry_msgs.msg import Twist

# 全局变量
vel_pub = None
nCount = 0  # 强制转弯倒计时变量

def LidarCallback(msg):
    global nCount  # 声明我们要修改外部的 nCount 变量

    # 1. 获取多角度距离（正前方180，左前150，右前210）
    dist_front = msg.ranges[180]
    dist_left  = msg.ranges[150]
    dist_right = msg.ranges[210]

    # 打印一下三个方向的距离，方便调试观察
    rospy.loginfo("前方:%.2f 米 | 左前:%.2f 米 | 右前:%.2f 米", dist_front, dist_left, dist_right)

    # 2. 状态检查：如果还在强制转弯倒计时中
    if nCount > 0:
        nCount -= 1  # 倒计时减 1
        return       # 直接返回，跳过下面的判断，继续执行上一次的转弯指令

    # 3. 速度控制消息包
    vel_cmd = Twist()

    # 4. 多角度避障判断逻辑
    # 只要正前方、左前方、右前方，任意一个方向距离小于 1.5 米，就触发避障
    if dist_front < 1.5 or dist_left < 1.5 or dist_right < 1.5:
        vel_cmd.angular.z = 0.3  # 设置转弯角速度
        nCount = 50              # 强制转弯 50 个周期，确保转弯半径足够大
        rospy.logwarn("检测到障碍物！开始强制转弯避障...")
    else:
        vel_cmd.linear.x = 0.05  # 前方安全，直行

    # 5. 将决定好的速度指令发出去
    vel_pub.publish(vel_cmd)

if __name__ == "__main__":
    rospy.init_node("lidar_node")
    
    # 订阅雷达数据
    lidar_sub = rospy.Subscriber("/scan", LaserScan, LidarCallback, queue_size=10)
    
    # 初始化发布者
    vel_pub = rospy.Publisher("/cmd_vel", Twist, queue_size=10)
    
    # 进入死循环等待消息
    rospy.spin()