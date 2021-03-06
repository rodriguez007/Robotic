#include <ros/ros.h>
#include "std_msgs/String.h"
#include "geometry_msgs/Twist.h"
#include "turtlesim/Pose.h"
#include "turtlesim/SetPen.h"
#include "assignment1/triangle.h"
#include <math.h>
#define _USE_MATH_DEFINES
using namespace std;



char nodeName [] = "draw_triangle_node_Testing";
ros::Publisher thePublisher;
double currentX;
double currentY;
bool starting = false;


void movePublisher(double speed, bool cw, double distance);
void rotationProcessing(bool cw, int angle);
void moveManager(float dist, bool clw);

void commandVelocityReceived(const turtlesim::Pose &msgIn) {
    ROS_INFO_STREAM("From the x axe: " << msgIn.x);
    ROS_INFO_STREAM("From the y axe: " << msgIn.y);
    ROS_INFO_STREAM("From the theta: " << msgIn.theta);
    currentX = msgIn.x;
    currentY = msgIn.y;

}
//Listenner to receive triangle info

void commandtriangleReceived(const assignment1::triangle &msgIn) {
    moveManager(msgIn.sideLength, msgIn.clw);
}

int main(int argc, char *argv[]) {
    ros::init(argc, argv, nodeName);
    ros::NodeHandle nh;

    thePublisher = ros::Publisher(nh.advertise<geometry_msgs::Twist>("turtle1/cmd_vel", 1000));

    ros::Subscriber subPose = nh.subscribe("turtle1/pose", 1000, &commandVelocityReceived);
    ros::Subscriber subTriangle = nh.subscribe("cmd", 1000, &commandtriangleReceived);

    //Setting up the Client service 
    ros::ServiceClient theClient = nh.serviceClient<turtlesim::SetPen>("turtle1/set_pen");
    ros::service::waitForService("turtle1/set_pen");


    turtlesim::SetPen::Request thePenServiceReq;
    turtlesim::SetPen::Response thePenServiceRes;
    thePenServiceReq.r = 255;
    thePenServiceReq.g = 0;
    thePenServiceReq.b = 0;
    theClient.call(thePenServiceReq, thePenServiceRes);

    ros::spin();

    return 0;
}

//Now we are building the move publisher

void movePublisher(double speed, bool cw, double distance) {

    geometry_msgs::Twist moveMsg;
    //if (cw)	 
    //	moveMsg.linear.x =abs(speed);
    // else 
    // {
    //Going in the other direction
    if (starting &&!cw) {
        rotationProcessing(cw, 180);
    }

    //}
    moveMsg.linear.x = abs(speed);


    //Now we need to set the linear y and z to 0
    moveMsg.linear.z = 0;
    moveMsg.linear.y = 0;

    moveMsg.angular.x = 0;
    moveMsg.angular.y = 0;
    moveMsg.angular.z = 0;
    double current_distance = double(0);
    double T0 = ros::Time::now().toSec();
    ros::Rate rosRate(100);
    thePublisher.publish(moveMsg);
    do {
        double T1 = ros::Time::now().toSec();
        current_distance = speed * (T1 - T0);
        ros::spinOnce();
        rosRate.sleep();

    } while (current_distance <= distance);
    moveMsg.linear.x = 0;
    moveMsg.linear.y = 0;
    moveMsg.linear.z = 0;
    moveMsg.angular.x = 0;
    moveMsg.angular.y = 0;
    moveMsg.angular.z = 0;
    thePublisher.publish(moveMsg);
    rotationProcessing(cw, 120);
}
//rostopic pub -1 /assignment1/cmd geometry_msgs/Twist -- '[2.0, 0.0, 0.0]' '[0.0, 0.0, 1.8]'

void rotationProcessing(bool cw, int angle) {
    double angular_speed = 5 * M_PI / 180; // Converting to radians
    double angleInRadius = angle * M_PI / 180;
    geometry_msgs::Twist theMsg;
    //We are now perfoming the rotation task:
    //Step1 we set all the linear values to 0 as we are not performing any linear motion
    theMsg.linear.x = 0;
    theMsg.linear.y = 0;
    theMsg.linear.z = 0;

    //Angular velocity in the Y-axies
    // We have to consider the rotation direction (clock/conter clockwise)
    //Clockwise == negative values
    //Conter clockwise = positive values
    theMsg.angular.y = 0;
    theMsg.angular.x = 0;
    if (cw == true)
        theMsg.angular.z = -abs(angular_speed);
    else
        theMsg.angular.z = abs(angular_speed);


    //Now Publishing the message
    thePublisher.publish(theMsg);


    double current_angle = double(0);
    double T0 = ros::Time::now().toSec();
    ros::Rate rosRate(100);

    do {
        /* code */
        thePublisher.publish(theMsg);
        double T1 = ros::Time::now().toSec();
        current_angle = angular_speed * (T1 - T0);
        ros::spinOnce();
        rosRate.sleep();

    } while (current_angle < angleInRadius); //60 degrees is the desired angle
    cout << "the angle is already greater" << endl;
    theMsg.angular.z = 0;
    thePublisher.publish(theMsg);
}

void moveManager(float dist, bool clw) {
    int counter = 3;
    if (dist > 5) dist = 5;

    for (int i = 0; i < counter; i++) {
        /* code */
        if (i == 0)starting = true;
        else starting = false;

        movePublisher(10, clw, dist);
    }


}


