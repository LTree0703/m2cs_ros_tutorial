#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "ros/ros.h"
#include "std_msgs/Int32.h"

// add the includes needed for socketCAN

int main(int argc, char **argv)
{
  ros::init(argc, argv, "encoder_node");
  ros::NodeHandle nh;

  ros::Publisher encoder_pub = nh.advertise<std_msgs::Int32>("p_feedback", 1000);

    // set up socket
    int s, i; 
	int nbytes;
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame;

	printf("CAN Sockets Receive Demo\r\n");

	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("Socket");
		return 1;
	}

	strcpy(ifr.ifr_name, "vcan0" );
	ioctl(s, SIOCGIFINDEX, &ifr);

	memset(&addr, 0, sizeof(addr));
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("Bind");
		return 1;
	}

  int count = 0;
  while (ros::ok())
  {
    // read the CAN frame
    nbytes = read(s, &frame, sizeof(struct can_frame));

    if (nbytes < 0) {
      perror("Read");
      return 1;
    }

    printf("0x%03X [%d] ",frame.can_id, frame.can_dlc);

    for (i = 0; i < frame.can_dlc; i++)
      printf("%02X ",frame.data[i]);

    printf("\r\n");

    std_msgs::Int32 msg;

    // fill in msg based on the contents from CAN frame
    std::stringstream ss;
    ss << frame.data;
    msg.data = ss.str();

    ROS_INFO("%s", msg.data.c_str());

    // publish msg
    encoder_pub.publish(msg);

    ros::spinOnce();
  }

  // close socket
  if (close(s) < 0) {
    perror("Close");
    return 1;
  }

  return 0;
}
