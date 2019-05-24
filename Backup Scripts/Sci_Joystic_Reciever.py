#!/usr/bin/python
import serial
import syslog
import time
import rospy
from scijoy.msg import SciMessage

readPort = '/dev/ttyACM0'
writePort = '/dev/ttyUSB1'

readArd = serial.Serial(readPort,115200,timeout = 5)
writeArd = serial.Serial(writePort,115200,timeout = 5)

def main(): #main
    sci_pub=rospy.Publisher('/scijoy', SciMessage, queue_size=1)
    rospy.init_node('talker',anonymous=True)
    r=rospy.Rate(50)
    sci_message = SciMessage()
    while True:
        # print('========\n')
        msg_finished=True
        while msg_finished:
            i=0
            msg=readArd.read(1)
            if msg.decode() == 'S':
                msg = msg_temp = ""
                while i<50 and msg_temp != 'E':
                    i+=1
                    msg = "".join((msg,msg_temp))
                    msg_temp = readArd.read(1)
                    msg_temp =  msg_temp.decode()
                if msg_temp == 'E':
                    msg_finished = False
            msg = msg.decode('utf-8').split(',')
            msg = [word for word in msg if word.isalnum()]#to trim out the escape sequences when splicing the serial into elements 
            if len(msg)!=0:
                sci_message.pot1 = msg[0]
                sci_message.button1 = msg[1]
        # sci_message.pot1 = msg
        sci_pub.publish(sci_message)
        r.sleep()

if __name__ == "__main__":
    main()