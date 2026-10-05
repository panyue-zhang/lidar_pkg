// 雷达测距+速度控制实现避障
#include <ros/ros.h>
#include <sensor_msgs/LaserScan.h>
#include <geometry_msgs/Twist.h>

ros::Publisher vel_pub;  //全局定义发布者变量
int nCount = 0;//增加转弯角度

void LidarCallback(const sensor_msgs::LaserScan msg)
{
    float fMidDist = msg.ranges[180];
    ROS_INFO("前方测距 ranges[180] = %f 米",fMidDist);

    if(nCount > 0)
    {
        nCount --; //若还在转弯指令中就减1
        return;
    }

    //速度控制消息包
    geometry_msgs::Twist vel_cmd;  //制作名为 vel_cmd 的速度指令盒子
    vel_cmd.linear.x = 0;   // 加上这些初始化速度
    vel_cmd.linear.y = 0;
    vel_cmd.linear.z = 0;
    vel_cmd.angular.x = 0;
    vel_cmd.angular.y = 0;
    vel_cmd.angular.z = 0;

    if(fMidDist < 1.5)
    {
        vel_cmd.angular.z = 0.3 ;
        nCount = 50 ;  //接下来50次雷达数据都强制进行转弯确保转弯半径变大，成功避障
    }
    else
    {
        vel_cmd.linear.x = 0.05 ;
    }
    vel_pub.publish(vel_cmd);
}

int main(int argc, char *argv[])
{
    setlocale(LC_ALL,"");
    ros::init(argc,argv,"lidar_node");

    ros::NodeHandle n;
    ros::Subscriber lidar_sub = n.subscribe("/scan",10, &LidarCallback);
    vel_pub = n.advertise<geometry_msgs::Twist>("/cmd_vel",10);

    ros::spin();

    return 0;
}


