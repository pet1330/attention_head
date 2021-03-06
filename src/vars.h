#include "ros/ros.h"
#include <tf/transform_listener.h>
#include <vector>
#include <math.h>
#include <cob_people_detection_msgs/DetectionArray.h>
#include <iostream>
#include <sensor_msgs/JointState.h>
#include <tf/transform_listener.h>
#include <boost/thread.hpp>
#include "uol_openday_common/Find_peopleAction.h"
#include <actionlib/server/simple_action_server.h>
#include <ros/time.h>

class vars {
protected:

    ros::NodeHandle nh_;
    // NodeHandle instance must be created before this line. Otherwise strange error may occur.
    actionlib::SimpleActionServer<uol_openday_common::Find_peopleAction> as_;
    std::string action_name_;
    // create messages that are used to published feedback/result
    uol_openday_common::Find_peopleFeedback feedback_;
    uol_openday_common::Find_peopleResult result_;
    double x, y, z;
    double last_x, last_y, last_z;
    ros::Publisher listening_pub;
    ros::Subscriber sub;
    tf::TransformListener listener;
    boost::mutex mutex;
    bool first_notfound;
    ros::Time end;
    bool Server_success;

public:

    vars(std::string name) :
    as_(nh_, name, boost::bind(&vars::executeCB, this, _1), false),
    action_name_(name) {
        as_.registerPreemptCallback(boost::bind(&vars::preemptCB, this));
        as_.start();
        //previous coordiantes
        last_x = 0;
        last_y = 0;
        last_z = 15;
        //current coordinates
        x = 0;
        y = 0;
        z = 0;
        //subscriber feel to get input from cob_people_detection
        sub = nh_.subscribe("/cob_people_detection/detection_tracker/face_position_array", 4, &vars::locationCallback, this);
        listening_pub = nh_.advertise<sensor_msgs::JointState>("/head/commanded_state", 10);
        first_notfound = true;
        Server_success = false;
    }

    ~vars(void) {
    }

    void executeCB(const uol_openday_common::Find_peopleGoalConstPtr &goal);
    void preemptCB();
    void locationCallback(const cob_people_detection_msgs::DetectionArray::ConstPtr& detectionArray);


};
