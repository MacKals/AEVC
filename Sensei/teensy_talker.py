import serial


#port = None

port = serial.Serial("/dev/ttyACM0", baudrate=115200, timeout=3.0)

def init():
    port = serial.Serial("/dev/ttyACM0", baudrate=115200, timeout=3.0)

def readLine():
    if port.inWaiting:
        return port.readline
    return False

def enable():
    port.write("EN")
    return "ok"

def disable():
    port.write("DI")
    return "ok"

def moveForward(dist):
    port.write("MF " + str(dist))

def spin(angle):
    port.write("S " + str(angle))

def height(dist):
    port.write("A " + str(dist))

def spinBody(angle):
    port.write("ST " + str(angle))

def spinBase(angle):
    port.write("SB " + str(angle))

def homeArm():
    port.write("HA")

def homeBase():
    port.write("HT")

def home(angle):
    port.write("H")
