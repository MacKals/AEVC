
import time
import positionFromImage as pos

import picamera
import serial

imageName = '/tmp/result.bmp'

def takePicture():
    with picamera.PiCamera() as camera:
        camera.vflip = True
        camera.capture(imageName)


port = serial.Serial("/dev/ttyACM0", baudrate=115200, timeout=3.0)

state = "detecting"

sideOfPort = "unknown"

expectedDoneMessages = 0


phiErrorLimit = 1  # degree
xError = 0.005     # m
yError = 0.005     # m
zError = 0.005     # m


def send(letter, num):
    global expectedDoneMessages

    if letter == 'MF':
        expectedDoneMessages = 2
    elif letter == 'A':
        expectedDoneMessages = 1
    elif letter == 'ST':
        expectedDoneMessages = 1
    elif letter == 'S':
        expectedDoneMessages = 2
    elif letter == 'SB':
        expectedDoneMessages = 3

    port.write(letter + ' ' + str("%.4f" % num))

    return expectedDoneMessages


# startup sequence

print 'Sarted'

time.sleep(3)

port.write('EN')

time.sleep(1)

port.write('H')
time.sleep(3)

while port.readline != "homing done":
    time.sleep(0.5)

previousState = state

while True:

    if port.inWaiting:
        message = port.readline
        if message == "done":
            expectedDoneMessages -= 1

    time.sleep(0.5)

    if previousState != state:
        print(state)
        previousState = state

    if expectedDoneMessages <= 0:

        if state == "detecting":
            if pos.posFromImage(imageName):
                state = "center_phi"
        
        takePicture()
        x, y, z, phi = pos.posFromImage(imageName)

        if state == "center_phi":
            if (abs(phi) > phiErrorLimit):
                send('S', phi)
            else:
                state = "center_z"

        elif state == "center_z":
            if (abs(z) > zError):
                send('A', -z)
            else:
                state = "approximate_y"

        elif state == "approximate_y":
            if (abs(y) > yError):
                if (y > 500):
                    send('MF', y-500)

        elif state == "center_x":
            if (abs(x) > xError):
                print("yeah!")
            else:
                state = "checkPosition"

        elif state == "checkPosition":
            if abs(phi) > phiErrorLimit or abs(z) > zError or abs(y) > yError or abs(x) > xError:
                state = "center_phi"
            else:
                state = "connect"

        elif state == "connect":
            send('MF', y-250)
            state = "charging"

        elif state == "charging":
            time.sleep(10)
            state = "return"

        elif state == "return":
            send('MF', -1000)
