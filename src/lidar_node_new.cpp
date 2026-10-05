// 雷达多角度检测 + 状态机避障
#include <ros/ros.h>
#include <sensor_msgs/LaserScan.h>
#include <geometry_msgs/Twist.h>

ros::Publisher vel_pub; // 全局发布者变量
int nCount = 0;         // 全局倒计时变量（用于强制转弯）

void LidarCallback(const sensor_msgs::LaserScan msg)
{
    // 1. 获取多角度距离（假设雷达有360个数据点，索引0-359）
    // 180度为正前方，150度为左前方30度，210度为右前方30度
    float dist_front = msg.ranges[180]; 
    float dist_left  = msg.ranges[150]; 
    float dist_right = msg.ranges[210]; 

    // 打印一下三个方向的距离，方便调试观察
    ROS_INFO("前方:%.2f 米 | 左前:%.2f 米 | 右前:%.2f 米", dist_front, dist_left, dist_right);

    // 2. 状态检查：如果还在强制转弯倒计时中
    if (nCount > 0)
    {
        nCount--; // 倒计时减 1
        return;   // 直接返回，跳过下面的判断，继续执行上一次的转弯指令
    }

    // 3. 速度控制消息包（每次循环必须重新清空初始化，防止残留数据）
    geometry_msgs::Twist vel_cmd;
    vel_cmd.linear.x = 0.0;
    vel_cmd.linear.y = 0.0;
    vel_cmd.linear.z = 0.0;
    vel_cmd.angular.x = 0.0;
    vel_cmd.angular.y = 0.0;
    vel_cmd.angular.z = 0.0;

    // 4. 多角度避障判断逻辑
    // 只要正前方、左前方、右前方，任意一个方向距离小于 1.5 米，就触发避障
    if (dist_front < 1.5 || dist_left < 1.5 || dist_right < 1.5)
    {
        vel_cmd.angular.z = 0.3; // 设置转弯角速度
        nCount = 50;             // 强制转弯 50 个周期（大概 2~3 秒），确保转弯半径足够大
        ROS_WARN("检测到障碍物！开始强制转弯避障...");
    }
    else
    {
        vel_cmd.linear.x = 0.05; // 前方安全，直行
    }

    // 5. 将决定好的速度指令发出去
    vel_pub.publish(vel_cmd);
}

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, ""); // 解决中文打印乱码
    ros::init(argc, argv, "lidar_node_new");

    ros::NodeHandle n;
    
    // 订阅雷达数据
    ros::Subscriber lidar_sub = n.subscribe("/scan", 10, &LidarCallback);
    
    // 初始化全局速度发布者
    vel_pub = n.advertise<geometry_msgs::Twist>("/cmd_vel", 10);

    // 进入死循环等待消息
    ros::spin();

    return 0;
}