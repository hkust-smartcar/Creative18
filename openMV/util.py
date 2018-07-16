from math import sqrt, pi
from pj import *
from jp import *
from exceptions import NotEnoughDataException

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
    return rad*360/(pi*2) % 360

"""
given a list, return its median
"""
def median(l):
    try:
        R = sorted(l)[int(len(l)/2)]
    except IndexError as e:
        print(e, len(l))
        R = None
    return R


def mode(l):
    if(len(l)==0): raise NotEnoughDataException
    d = {}
    m = 0
    for i in l:
        if(i in d):
            d[i]+=1
        else:
            d[i]=1
        if(d[i]>m):
            m=d[i]
    # print(d)
    t = []
    for k in d:
        if d[k] == m:
            t.append(k)
    return t[0]

"""
take mode, if more than one have same frequency, take median
"""
def modeMedian(l):
    if(len(l)==0): raise NotEnoughDataException
    d = {}
    m = 0
    for i in l:
        if(i in d):
            d[i]+=1
        else:
            d[i]=1
        if(d[i]>m):
            m=d[i]
    # print(d)
    t = []
    for k in d:
        if d[k] == m:
            t.append(k)
    return median(t)

def modeSimilarMedian(l,threshold = 0.1):
    if(len(l)==0): raise NotEnoughDataException
    d = {}
    m = 0

    #get mode frequency
    for i in l:
        if(i in d):
            d[i]+=1
        else:
            d[i]=1
        if(d[i]>m):
            m=d[i]
    
    #get keys with same frequency
    t = []
    for k in d:
        if d[k] == m:
            t.append(k)

    if len(t)==1:
        return t[0]
    
    for i in l:
        for j in t:
            if(abs(i-j)<threshold):
                d[j]+=1
                if(d[j]>m):
                    m=d[j]
    t = []
    for k in d:
        if d[k] == m:
            t.append(k)

    return median(t)