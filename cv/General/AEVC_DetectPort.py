import numpy as np
import cv2
import picamera
from io import BytesIO


stream = BytesIO()
with picamera.PiCamera() as camera:
    camera.resolution = (640, 480)
    camera.vflip = True
    camera.capture(stream, format='jpeg')

#Convert the picture into a numpy array
buff = np.fromstring(stream.getvalue(), dtype=np.uint8)

#Now creates an OpenCV image
image = cv2.imdecode(buff, 1)

#Convert to grayscale
gray = cv2.cvtColor(image,cv2.COLOR_BGR2GRAY)

# detect circles in the image
circles = cv2.HoughCircles(gray, cv2.HOUGH_GRADIENT,1,150)


# ensure at least some circles were found
if circles is not None:
	# convert the (x, y) coordinates and radius of the circles to integers
	circles = np.round(circles[0, :]).astype("int")

	# loop over the (x, y) coordinates and radius of the circles
	maxCircle = circles[0]
	for (x, y, r) in circles:
		if r > maxCircle[2]:
			maxCircle = [x,y,r]
	
	print(str(x) + " " + str(y) + " " + str(r))
else:
	print("None")
