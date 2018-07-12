from math import sqrt, pi
from pj import *
from jp import *

"""
give two points, return euclidean distance
"""
def dist(p1,p2):
    return sqrt((p1[0]-p2[0])**2+(p1[1]-p2[1])**2)

"""
give two points, return squared euclidean distance
"""
def sqdist(p1,p2):
    return (p1[0]-p2[0])**2+(p1[1]-p2[1])**2

"""
give upperbound and point, return whether point is in range
lowerbound is fixed to be (0,0)
"""
def inRange(r,p):
    return p[0]>=0 and p[1]>=0 and p[0]<r[0] and p[1]<r[1]

"""
brief: given a projection matrix and source point, return the image point
"""
def pm(h,s):
    [x,y]=s
    return (int((h[0][0]*x+h[0][1]*y+h[0][2])/(h[2][0]*x+h[2][1]*y+h[2][2])),\
            int((h[1][0]*x+h[1][1]*y+h[1][2])/(h[2][0]*x+h[2][1]*y+h[2][2])))

"""
brief: given a image buffer coordinate, return its world coordinate
"""
def mapToWorld(s):
    return pm(jp,s)

"""
brief: given a world coordinate, return its image buffer coordinate
"""
def mapToImage(s):
    return pm(pj,s)

"""
return: sign of number -1: negative
                        0: zero
                        1: positive
"""
def sgn(num):
    if(num<0):
        return -1
    if(num>0):
        return 1
    return 0

"""
given an angle in radian, return degree
"""
def deg(rad):
    return rad*360/(pi*2)