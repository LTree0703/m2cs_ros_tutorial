#!/usr/bin/env python
from calendar import formatstring
import rospy
from geometry_msgs.msg import Twist
from turtlesim.srv import SetPen

# hint: some imports are missing (done)
from m2_ps4.msg import Ps4Data

old_data = Ps4Data()
k = 2.0
r, g, b, width, off = 255, 255, 255, 3, 0

def callback(data):
    global old_data, k, r, g, b, width, off

    # you should publish the velocity here!
    vel = Twist()
    # hint: to detect a button being pressed, you can use the following pseudocode:
    # if ((data.button is pressed) and (old_data.button not pressed)),
    # then do something...
    
    if data.dpad_y != old_data.dpad_y:
        k += data.dpad_y
        if k < 1.0: k = 1.0
        elif k > 5.0: k = 5.0
    
    if data.ps == True:
        off = 1
    
    if data.cross == True:
        r, g, b = 0, 0, 255
    elif data.circle == True:
        r, g, b = 0, 255, 0
    elif data.triangle == True:
        r, g, b = 255, 0, 0
    elif data.square == True:
        r, g, b = 160, 32, 240

    call_set_pen_service(r, g, b, width, off)
        
    vel.linear.x = data.hat_ly * k
    vel.linear.y = 0
    vel.linear.z = 0
    
    vel.angular.x = 0
    vel.angular.y = 0
    vel.angular.z = data.hat_rx * 2
    
    pub.publish(vel)
    rospy.loginfo(data)
    old_data = data

def call_set_pen_service(r, g, b, width, off):
    response = srv_pen(r, g, b, width, off)
    rospy.loginfo(response)
    
if __name__ == '__main__':
    rospy.init_node('ps4_controller')
    
    pub = rospy.Publisher('/turtle1/cmd_vel', Twist, queue_size=1)# publisher object goes here... hint: the topic type is Twist
    sub = rospy.Subscriber('input/ps4_data', Ps4Data, callback) # subscriber object goes here
        
    # one service object is needed for each service called!
    srv_pen = rospy.ServiceProxy('/turtle1/set_pen', SetPen) # service client object goes here... hint: the srv type is SetPen
    # fill in the other service client object...

    
    
    rospy.spin()
