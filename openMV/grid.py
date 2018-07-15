import sensor
import image
import time
from util import sqdist, dist, mapToImage, mapToWorld, sgn
from math import acos, pi, sin, cos
#from quicksort import quicksort


origin = (0, 0)

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
    cs = []
    for rk, c in enumerate(rects):
        cs += c
        min_dx = 10000
        for i, p1 in enumerate(c):
            p2 = c[i-1]
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
        print("l69",e,d,length,votes,rects,cs)
        R = 594561648413218498712, 0
    return R


def transformRects(rects):
    newRects = []
    for rect in rects:
        corners = rect.corners()
        newCorners = []
        for corner in corners:
            newCorners.append(mapToWorld(corner))
        newRects.append(newCorners)
    return newRects

def get_length(img, rects, threshold):
    dist_votes = {}
    length = 50
    for c in rects:
        for i, p1 in enumerate(c):
            p2 = c[i-1]
            local_length = dist(p1, p2)
            if(local_length < 35 or local_length > 45):
                break
            vote(dist_votes, local_length, threshold)
    length = getHighestVote(dist_votes, length)
    #print('length,', length)
    return length


def getGoodRects(rects, length, threshold):
    goodRects = []
    for corners in rects:
        reject = False
        for k, c in enumerate(corners):
            if(abs(dist(c, corners[k-1]) - length) > threshold):
                reject = True
                break
        if not reject:
            goodRects.append(corners)
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

"""
return 0,1,2,3
0: -pi/4 to pi/4
1: pi/4 to 3pi/4
2: 3pi/4 to 5pi/4
3: 5pi/4 to 7pi/4
"""
def getLocalRotateType(gRotation,theta):
    if gRotation<pi/4 or gRotation>7*pi/4:
        return 0
    elif gRotation>pi/4 and gRotation<3*pi/4:
        return 1
    elif gRotation>3*pi/4 and gRotation<5*pi/4:
        return 2
    elif gRotation>5*pi/4 and gRotation<7*pi/4:
        return 3
    else:
        if theta == pi/4:
            if gRotation==pi/4:
                return 0
            elif gRotation==pi/4:
                return 1
            elif gRotation==3*pi/4:
                return 2
            elif gRotation==5*pi/4:
                return 3
        elif theta == -pi/4:
            if gRotation==7*pi/4:
                return 0
            elif gRotation==3*pi/4:
                return 1
            elif gRotation==5*pi/4:
                return 2
            elif gRotation==7*pi/4:
                return 3
    raise Exception('unknow angle',gRotation,theta)

def getLocalTranslation(corners):
    tlcs = [] #top left corners
    #brcs = [] #bottom right corners

    #for each rects
    for i in range(0,len(corners),4):
        c = corners[i:i+4]
        tlc = [0,0]

        #find its center point
        for j in range(0,4,1):
            tlc[0]+=c[j][0]
            tlc[1]+=c[j][1]
        tlcs.append([tlc[0]/4,tlc[1]/4])

    dx, dy = 0, 0
    for c in tlcs:
        dx+=c[0]
        dy+=c[1]
    
    try:
        R = int(dx/len(tlcs)%50),int(dy/len(tlcs)%50)
    except Exception as e:
        print(e,tlcs)
        R = 0,0
    return R

def rotateLocalTranslation(dx,dy,gRotation, theta):
    rtype = getLocalRotateType(gRotation, theta)
    if rtype==0:
        return dy%50, -dx%50
    elif rtype==1:
        return (dx)%50, dy%50
    elif rtype==2:
        return -dy%50, dx%50
    elif rtype==3:
        return -dx%50, -dy % 50
    else:
        raise Exception('unknown rotation type ',rtype)

"""
only calculate X or Y
"""
def getGlobalSubTranslation(prevg, prevl, currl):
    if (prevl >= 35 and currl <= 15 and abs(prevl - currl)>25):
        print("add 1")
        currg = prevg + 50 - prevl + currl
    elif (prevl <= 35 and currl >= 15 and abs(prevl - currl)>25):
        print("minus 1")
        currg = prevg - prevl + currl - 50
    else:
        currg = prevg - prevl + currl
    return currg

def getGlobalTranslation(prevg, prevl, currl):
    return getGlobalSubTranslation(prevg[0], prevl[0], currl[0]),\
           getGlobalSubTranslation(prevg[1], prevl[1], currl[1])