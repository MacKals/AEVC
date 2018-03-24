
# import libraries
import cv2
import numpy as np
import sys

args = sys.argv
if (len(args) != 2):
    sys.exit(str(args) + "Argument error")

filename = args[1]

image = cv2.imread(filename)

class Finder:
    # Circle properties
    diameter = 0.1 # m

    # Camera properties
    xPixels = 3280
    yPixels = 2464

    xAngleOfView = 62.2/2 * np.pi/180 # rad
    yAngleOfView = 48.8/2 * np.pi/180 # rad


    # Helper constants
    widthAt1m = 2 * np.tan(xAngleOfView) # total width of cameras field of view 1m in front
    expectedWidthFraction = diameter / widthAt1m


    def xAngleFromOffset(self, x):
        pixelOffset = x - self.xPixels/2
        return pixelOffset/self.xPixels * self.xAngleOfView

    def yAngleFromOffset(self, y):
        pixelOffset = y - self.yPixels/2
        return pixelOffset/self.yPixels * self.yAngleOfView


    # Distance from camera to object
    # Assuming: port is located in centre of image to avoid underestimate
    def x(self, width):
        return self.xPixels/width * self.expectedWidthFraction

    # Angle circle must be turned to face camera
    # Assuming: port is located in centre of image to minimize error
    def theta(self, width, height):
        return np.arccos(width/height)

    # Angle camera must be turned to centre cirlce in image
    def phi(self, x):
        return self.xAngleFromOffset(x)

    def h(self, y, height):
        return self.x(height) * np.tan(self.yAngleFromOffset(y))




def convertToBinary(image):
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    ret, binary = cv2.threshold(gray, 80, 255, cv2.THRESH_BINARY_INV)
    return binary


def printRelativePositionFromBinary(binary):

    kernel = np.ones((5,5), np.uint8)
    returnedImage, contours, hierarchy = cv2.findContours(binary, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    if not contours:
        return

    # pick largest contour
    index = 0
    maxArea = 0

    for i in range(len(contours)):
        if cv2.contourArea(contours[i])> maxArea:
            index = i
            maxArea = cv2.contourArea(contours[i])

    (x, y), (width, height), _ = cv2.fitEllipse(contours[index])

    # computing parameters
    f = Finder()

    rComputed = f.x(width)
    thetaComputed = f.theta(width, height)
    phiComputed = f.phi(x)
    hComputed = f.h(y, height)

    xRelative = rComputed * np.sin(thetaComputed)
    yRelative = rComputed * np.cos(thetaComputed)
    zRelative = hComputed
    phiRelative = phiComputed

    print(str("%.4f" % xRelative) + ' ' + str("%.4f" % yRelative) + ' ' + str("%.4f" % zRelative) + ' ' + str("%.4f" % phiRelative))

binary = convertToBinary(image)
printRelativePositionFromBinary(binary)
