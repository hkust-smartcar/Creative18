import sensor
import image
import time
from util import sqdist, dist, mapToImage, mapToWorld, sgn
from math import acos, pi, sin, cos
#from quicksort import quicksort


origin = (145, 270)

def sortRects(rects):
    rects_ = []
    for rect in rects:
        score = 0
        corners = rect.corners()
        for corner in corners:
            score += corner[1]*4 + abs(corner[0] - origin[0])
        rects_.append((
            score,
            rect
        ))
    rects = sorted(rects_, key = lambda r:r[0])
    return list(map(lambda r:r[1],rects))

def vote(votes, new_vote, threshold):
    for v in votes:
        if(abs(v-new_vote) < threshold):
            # votes[v] += 1
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
    d = 594561648413218498712
    for rk, r in enumerate(rects):
        c = r.corners()
        min_dx = 10000
        for i, p1 in enumerate(c):
            p1 = mapToWorld(p1)
            p2 = mapToWorld(c[i-1])
            # img.draw_circle(p1[0], p1[1], 5, color=(255, 0, 0))
            if p1[1] > p2[1]:
                p1, p2 = p2, p1
            dx = p2[0] - p1[0]
            if(abs(dx) < abs(min_dx)):
                min_dx = dx
        vote(votes, min_dx, threshold)
    d = getHighestVote(votes, d)
    #print(d)
    #print(length)
    try:
        R = d, acos(d/40)-pi/2
    except Exception as e:
        print(e,d)
        R = 594561648413218498712, 0
    return R


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
    x = p[0] - origin[0]
    y = origin[1] - p[1]
    x1 = co*x - si*y
    y1 = si*x + co*y
    return (int(x1) + origin[0], origin[1] - int(y1))


def getRotateCorners(img, fixedCorners, theta):
    co = cos(theta)
    si = sin(theta)
    rotatedCorners = list(map(lambda p: rotateTransform(p, co, si), fixedCorners))
    return rotatedCorners

def getLocalDisplacement(rects):
    return rects

def getLocalPosition(corners):
    tlcs = [] #top left corners
    #brcs = [] #bottom right corners

    #for each rects
    for i in range(0,len(corners),4):
        c = corners[i:i+4]
        tlc = c[0]
        d = sqdist([0,0],c[0])

        #find its top left corner
        for j in range(1,4,1):
            d_ = sqdist([0,0],c[j])
            if (d_<d):
                tlc = c[j]
                d = d_
        tlcs.append(tlc)

    dx, dy = 0, 0
    for c in tlcs:
        dx+=c[0]%50
        dy+=c[1]%50
    
    try:
        R = int(dx/len(tlcs)),int(dy/len(tlcs))
    except Exception as e:
        print(e,tlcs)
        R = 0,0
    return R
