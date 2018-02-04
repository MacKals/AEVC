import numpy as np
import cv2


def solidity(cnt):
    area = cv2.contourArea(cnt)
    hull = cv2.convexHull(cnt)
    hull_area = cv2.contourArea(hull)
    return float(area)/hull_area

# Use metrics to determine which contour corresponds to the port of the i3
def i3PortContour(contours):
    
    index = 0
    maxArea = 0

    for i in range(len(contours)):
        if cv2.contourArea(contours[i])> maxArea and solidity(contours[i]) > 0.98:
            index = i
            maxArea = cv2.contourArea(contours[i])
    
    return contours[index]


def pointAngle(p1, p2, p3):
        
    v1 = p2-p1
    v2 = p2-p3
        
    return np.arccos(np.dot(v1,v2)/(np.linalg.norm(v1)*np.linalg.norm(v2)))*180/np.pi

def anglesBetweenPoints(points):
    p = np.append([points[-1]], np.append(points, [points[0]], axis=0), axis=0)
    return [pointAngle(p[i], p[i+1], p[i+2]) for i in range(len(points))]


def removeLoneEntriesFromBinary(a):
    
    r = np.empty_like(a)
    
    looped = np.append([a[-1]], np.append(a, [a[0]], axis=0), axis=0)
    
    for i in range(len(a)):
        if looped[i] == looped[i+2] and looped[i] != looped[i+1]:
            r[i] = looped[i]
        else:
            r[i] = looped[i+1]
    
    return r


#returns rising edges and falling edges in two seperate arrays
def edgeIndecies(a):
    
    looped = np.append(a, [a[-1]], axis=0)
    
    risingIndecies = []
    fallingIndecies = []
    
    for i in range(len(a)):
        if looped[i] > looped[i+1]:
            fallingIndecies.append(i)
        elif looped[i] < looped[i+1]:
            risingIndecies.append(i+1)
        
    return (risingIndecies, fallingIndecies)

def dist(a, b):
    return ((a[0] - b[0])**2 + (a[1] - b[1])**2)**(1/2)

def keepFourLongest(a, p):
    risingIndecies, fallingIndecies = edgeIndecies(a)
    
    if len(risingIndecies) != len(fallingIndecies):
        print('lengthFault!')
    
    count = len(risingIndecies)
    
    if count < 4:
        print('Not enough straight lines')
    
    if risingIndecies[0] > fallingIndecies[0]:
        fallingIndecies = [fallingIndecies[(i+1)%count] for i in range(count)]
        #fallingIndecies = np.roll(fallingIndecies, 1)
        
    distance = np.array([dist(p[risingIndecies[i], :], p[fallingIndecies[i], :]) for i in range(len(risingIndecies))])
    
    
    indeciesToKeep = np.zeros((4,2))
    
    for i in range(4):
        longestIndex = np.argmax(distance)
        
        indeciesToKeep[i,0] = risingIndecies[longestIndex]
        indeciesToKeep[i,1] = fallingIndecies[longestIndex]
        
        distance[longestIndex] = 0        
    
    print(indeciesToKeep)
    indeciesToKeep = indeciesToKeep[indeciesToKeep[:,0].argsort()]
    print(indeciesToKeep)
    r = np.zeros((4, 2, 2))
    
    for i in range(4):
        r[i,0,:] = p[int(indeciesToKeep[i, 0])]
        r[i,1,:] = p[int(indeciesToKeep[i, 1])]
    
    return r



# line: [y(0), dy/dx]
def lineFromPoints(p1, p2):
    m = (p1[1]-p2[1])/(p1[0]-p2[0])
    y0 = p1[1]-m*p1[0]
    return [y0, m]


def intersection(l1, l2):
    x = (l1[0]-l2[0])/(l2[1]-l1[1])
    y = l1[0]+l1[1]*x
    return [x,y]


def flattenToCoordianteArray(a):
    c = np.array(a).flatten('F')
    
    x = c[:int(np.size(c)/2)]
    y = c[int(np.size(c)/2):]

    return np.array([x,y]).T
