import serial
import syslog
import time

readPort = '/dev/ttyACM0'
writePort = '/dev/ttyUSB1'

readArd = serial.Serial(readPort,115200,timeout = 5)
writeArd = serial.Serial(writePort,115200,timeout = 5)

#21 lights total
#bitwise string of 21
lightOn = False
#will change dependent on input taken from guys who see if thingy actually worked

i = 0
while True:
	#q = '9'
	#writeArd.write(q)
	#print(q)
	if lightOn:
		writeArd.write('1')
	else:
		writeArd.write('0')
	msg = readArd.read(readArd.inWaiting())
	#time.sleep(.015)
	print(msg)


#while( i > 0):
#	msg = ard.read(ard.inWaiting())
#	print(msg)
#	i = i -1

def lightOnOff(bitString,lightNum, On):
	#bitString is the bitString
	#lightNum is an integer for what number light we want to alter
	#On is a boolean of whether it is on or off, True for On, False for off
	#00
	