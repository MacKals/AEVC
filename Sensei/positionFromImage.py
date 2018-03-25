## AEVC ##

import cv2
import numpy as np

# Circle properties
diameter = 0.1  # m

# Camera properties
xPixels = 3280
yPixels = 2464

xAngleOfView = 62.2/2 * np.pi/180  # rad
yAngleOfView = 48.8/2 * np.pi/180  # rad


# Helper constants
widthAt1m = 2 * np.tan(xAngleOfView) # total width of cameras field of view 1m in front
expectedWidthFraction = diameter / widthAt1m

def xAngleFromOffset(x):
    pixelOffset = x - xPixels/2
    return pixelOffset/xPixels * xAngleOfView

def yAngleFromOffset(y):
    pixelOffset = y - yPixels/2
    return pixelOffset/yPixels * yAngleOfView


# Distance from camera to object
# Assuming: port is located in centre of image to avoid underestimate
def xFrom(width):
    return xPixels/width * expectedWidthFraction

# Angle circle must be turned to face camera
# Assuming: port is located in centre of image to minimize error
def thetaFrom(width, height):
    return np.arccos(width/height)

# Angle camera must be turned to centre cirlce in image
def phiFrom(x):
    return xAngleFromOffset(x)

def hFrom(y, height):
    return x(height) * np.tan(yAngleFromOffset(y))




def convertToBinary(image):
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    ret, binary = cv2.threshold(gray, 80, 255, cv2.THRESH_BINARY_INV)
    return binary


def relativePositionFromBinary(binary):

    kernel = np.ones((5, 5), np.uint8)
    returnedImage, contours, hierarchy = cv2.findContours(binary, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    if not contours:
        return

    # pick largest contour
    index = 0
    maxArea = 0

    for i in range(len(contours)):
        if cv2.contourArea(contours[i]) > maxArea:
            index = i
            maxArea = cv2.contourArea(contours[i])

    (x, y), (width, height), _ = cv2.fitEllipse(contours[index])

    # computing parameters

    rComputed = xFrom(width)
    thetaComputed = thetaFrom(width, height)
    phiComputed = phiFrom(x)
    hComputed = hFrom(y, height)

    xRelative = rComputed * np.sin(thetaComputed)
    yRelative = rComputed * np.cos(thetaComputed)
    zRelative = hComputed
    phiRelative = phiComputed

    return (xRelative*1000, yRelative*1000, zRelative*1000, phiRelative)


def posFromImage(image):
    binary = convertToBinary(image)
    return relativePositionFromBinary(binary)
