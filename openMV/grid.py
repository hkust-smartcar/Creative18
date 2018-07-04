import sensor
import image
import time
from util import dist, mapToImage, mapToWorld, sgn
from math import acos, pi, sin, cos


def vote(votes, new_vote, threshold):
    for v in votes:
        if(abs(v-new_vote) < threshold):
            cnt = votes[v]
            votes.pop(v,None)
            nv = (cnt*v + new_vote)/(cnt+1)
            votes[nv] = cnt+1
            return
    votes[new_vote] = 1


def getHighestVote(votes, default):
    max_v_count = 0
    max_v = default
    for v in votes:
        if votes[v] > max_v_count:
            max_v_count = votes[v]
            max_v = v
    return max_v


def get_rotation(img, rects, length, threshold):
    votes = {}
    d = 0
    for rk, r in enumerate(rects):
        c = r.corners()
        min_dx = 10000
        for i, p1 in enumerate(c):
            p1 = mapToWorld(p1)
            p2 = mapToWorld(c[i-1])
            img.draw_circle(p1[0], p1[1], 5, color=(255, 0, 0))
            if p1[1] > p2[1]:
                p1, p2 = p2, p1
            dx = p2[0] - p1[0]
            if(abs(dx) < abs(min_dx)):
                min_dx = dx
        vote(votes, min_dx, threshold)
    d = getHighestVote(votes, d)
    #print(d)
    #print(length)
    return d, acos(d/length)-3.141592653/2


def get_length(img, rects, threshold):
    dist_votes = {}
    length = 50
    for rk, r in enumerate(rects):
        c = r.corners()
        for i, p1 in enumerate(c):
            p1 = mapToWorld(p1)
            p2 = mapToWorld(c[i-1])
            local_length = dist(p1, p2)
            # if(local_length < 45 or local_length > 55):
            #     break
            vote(dist_votes, local_length, threshold)
    length = getHighestVote(dist_votes, length)
    #print('length,', length)
    return length


def getGoodRects(rects, length, threshold):
    goodRects = []
    for rect in rects:
        corners = list(map(mapToWorld, rect.corners()))
        reject = False
        for k, c in enumerate(corners):
            if(abs(dist(c, corners[k-1]) - length) > threshold):
                reject = True
                break
        if not reject:
            goodRects.append(rect)
    return goodRects


"""
brief: given previous global and local rotation and current local rotation
return: current global rotation
unit: radian
"""


def getGlobalRotation(prevg, prevl, currl):
    currg = prevg + prevl - currl
    # if (True or abs(prevl) > pi/4 and abs(currl) > pi/4):
    if(sgn(prevl) == 1 and sgn(currl) == -1):
        #prev is positive, current is negative
        if(abs(prevl - currl) > pi/4):
            currg -= pi/2
    elif(sgn(prevl) == -1 and sgn(currl) == 1):
        #prev is negative, current is positive
        if(abs(prevl - currl) > pi/4):
            currg += pi/2
    return currg % (2*pi)


def getGlobalRotationWithDirection(prevg, prevl, currl, clockwise):
    currg = prevg - prevl + currl
    if(clockwise):
        if(currl > prevl):
            currg -= pi/2
    else:
        if(currl < prevl):
            currg += pi/2
    return currg


def rotateTransform(p, co, si):
    x = p[0] - 95
    y = 270 - p[1]
    x1 = co*x - si*y
    y1 = si*x + co*y
    return (int(x1), int(y1))


def getRotateCorners(img, fixedCorners, theta):
    co = cos(theta)
    si = sin(theta)
    rotatedCorners = list(map(lambda p: rotateTransform(p, co, si), fixedCorners))
    return rotatedCorners

def getMergedRotatedCornerLength(corners):
    x = []
    y = []
    for p in corners:
        x.append(p[0])
        y.append(p[1])
    x=sorted(x)
    y=sorted(y)
    len_votes = {}
    for k in range(len(x)-1):
        d = abs(x[k+1]-x[k])
        if(d>10):
            vote(len_votes,d ,1)
    for k in range(len(y)-1):
        d = abs(y[k+1]-y[k])
        if(d>10):
            vote(len_votes, d,1)
    return getHighestVote(len_votes,40)

def getLocalDisplacement(img, rotatedCorners, length):
    x_votes = {}
    y_votes = {}
    for p in rotatedCorners:
        vote(x_votes, p[0] % length, 2)
        vote(y_votes, p[1] % length, 2)
    return [getHighestVote(x_votes, 0), getHighestVote(y_votes, 0)]
