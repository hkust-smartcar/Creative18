import sensor
import image
import time
from util import sqdist, dist, mapToImage, mapToWorld, sgn
from math import acos, pi, sin, cos
#from quicksort import quicksort


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

def rotateLocalTranslation(dx,dy,gRotation, theta):
    rtype = getLocalRotateType(gRotation, theta)
    if rtype==0:
        return (50-(-dy+40)+2)%50, (50-dx+20-2)%50
    elif rtype==1:
        return (dx)%50,(50-(-dy+15)-23)%50
    elif rtype==2:
        return (dy-15)%50, dx%50
    elif rtype==3:
        return (50-dx+20)%50,(dy-15+25) % 50
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