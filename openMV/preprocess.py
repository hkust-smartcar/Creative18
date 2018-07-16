import sensor
import image
import time
from util import sqdist, dist, mapToImage, mapToWorld, sgn, median, mode, modeSimilarMedian, modeMedian
from math import acos, pi, sin, cos, atan, tan, sqrt
from grid import getGoodRects, get_length
from exceptions import NoEdgeException, StupidPartitionException, NoRectException


origin = (0,0)


kCorners = 0
kType=1
kTheta= 2
kInterceptType=3
kIntercept=4


def hi(rects):
    l = len(rects)
    for i in range(l-1,-1,-1):
        r = rects[i]
        for c in r:
            if(c[1]<140):
                rects.pop(i)
                break
    return rects

def main(rects, img):
    transformRects(rects)
    # rects = getGoodRects(rects,40,7)
    rects = hi(rects)
    #rects = sortRects(rects)[0:1]
    # rects = getSquare(rects)#[0:1]
    if(len(rects)==0):
        raise NoRectException
    edges = formEdges(rects)
    print(len(edges))
    if(len(edges) == 0 ):
        print("hey")
        raise NoEdgeException
    length = modeMedian(list(map(lambda e:dist(e[kCorners][0],e[kCorners][1]),edges)))
    print(length)
    edges = formEdges(rects, length = length, threshold = 10)
    if(len(edges) == 0 ):
        raise NoEdgeException
    rects = []
    # edges = noEdge(edges)
    lRotation = getLocalRotation(edges)
    edges = filterEdgesByRotation(edges, lRotation)
    edges = adjustIntercept(edges)
    d = getLocalTranslation(edges, lRotation)
    print("lRot: ", lRotation, "lTra: ", d)
    printEdges(edges, img)

    error = getLocalTranslationError(lRotation)
    d = fixLocalTranslationError(d,lRotation,error)
    print("fixed: ",d)

    return lRotation, d

"""
[x1,x2) and 
[y1,y2)
"""
def inRegion(p,x1,y1,x2,y2):
    return p[0]>=x1 and p[0]<x2 and p[1]>=y1 and p[1]<y2

def noEdge(edges):
    edges_ = []
    x1,y1,x2,y2 = 100, 120, 200, 230
    for edge in edges:
        if(inRegion(edge[kCorners][0],x1,y1,x2,y2) and inRegion(edge[kCorners][1],x1,y1,x2,y2)):
            edges_.append(edge)
    return edges_


def worldToDisplayCoordinate(p):
    return [int(p[0]/5+50),int(p[1]/5+50)]


def printEdges(edges, img):
    c = [
        (255,0,0),
        (0,255,0),
        (0,0,255),
        (0,0,0)
    ]
    for edge in edges:
        [p1,p2] = edge[kCorners]
        p1 = worldToDisplayCoordinate(p1)
        p2 = worldToDisplayCoordinate(p2)
        # p1 = mapToImage(p1)
        # p2 = mapToImage(p2)
        k = edge[kType]
        img.draw_line(p1[0],p1[1],p2[0],p2[1],color=c[k], thickness=1)
        #img.draw_string((p1[0]+p2[0])//2, (p1[1]+p2[1])//2, str(edge[kType]), color=(123,123,123),scale=1)


def printIntercepts(edges, img):
    c = [
        (255,0,0),
        (0,255,0),
        (0,0,255),
        (0,0,0)
    ]
    for edge in edges:
        x,y = 0,0
        if(edge[kInterceptType]=="x"):
            x = edge[kIntercept]
        else:
            y = edge[kIntercept]
        x,y = worldToDisplayCoordinate([x,y])
        k = edge[kType]
        img.draw_circle(x,y,5,color=c[k], thickness=1)


def printGrid(ox,oy,ix,iy,thetaIntercept,theta,img, color=(255,0,255)):
    w = 300
    h = 250
    ox = int(ox)
    oy = int(oy)
    ix = int(ix)
    iy = int(iy)
    if(thetaIntercept == 'x'):
        for x in range(ox, w, ix):
            x_ = h/tan(theta) + x
            p1 = worldToDisplayCoordinate([x,0])
            p2 = worldToDisplayCoordinate([x_,h])
            img.draw_line(p1[0],p1[1],p2[0],p2[1],color=color)
        for y in range(oy, w, iy):
            y_ = y - w/tan(theta)
            p1 = worldToDisplayCoordinate([0,y])
            p2 = worldToDisplayCoordinate([w,y_])
            img.draw_line(p1[0],p1[1],p2[0],p2[1],color=color)
    else:
        for x in range(ox, w, ix):
            x_ = x - h*tan(theta)
            p1 = worldToDisplayCoordinate([x,0])
            p2 = worldToDisplayCoordinate([x_,h])
            img.draw_line(p1[0],p1[1],p2[0],p2[1],color=color)
        for y in range(oy, w, iy):
            y_ = y + w*tan(theta)
            p1 = worldToDisplayCoordinate([0,y])
            p2 = worldToDisplayCoordinate([w,y_])
            img.draw_line(p1[0],p1[1],p2[0],p2[1],color=color)


def transformRects(rects):
    for _ in range(len(rects)):
        corners = list(rects.pop(0).corners())
        for _ in range(len(corners)):
            corners.append(mapToWorld(corners.pop(0)))
        sortCorners(corners)
        rects.append(corners)


def sortCorners(corners):
    y = 0
    m = -1
    #determine the 0th corner's index
    for k in range(len(corners)):
        pt = corners[k]     #point this
        pp = corners[k-1]   #point previous
        if (sgn(pt[0]-pp[0]) != sgn(pt[1]-pp[1])): #only consider those with negative slope
            # print("NS")
            if (m == -1):      #this is first interested corner
                m = k
                y = min(pt[1],pp[1])
            else:               #this is not first interested corner
                y_ = min(pt[1],pp[1])
                if (y_ < y):    #this is higher than the previous
                    y = y_
                    m = k
    #rearrange the corners
    for _ in range(m):
        corners.append(corners.pop(0))
    return corners


def sortRects(rects):
    rects = sorted(rects, key = lambda r: abs(r[0][0]-origin[0]))
    return sorted(rects, key = lambda r: r[0][1] , reverse = True)


def formEdges(rects, length = 40, threshold = 20):
    edges = []
    for rect in rects:
        edges_ = []
        for k, p2 in enumerate(rect):    # k is the type of edge
            p1 = rect[k-1]
            if(abs(dist(p1,p2)-40)>threshold):
                edges_ = []
                break
            theta = getTheta(p1,p2)
            interceptType = getInterceptType(theta)
            if (interceptType == 'x'):
                intercept = getXIntercept(p1, theta)
            else:
                intercept = getYIntercept(p1, theta)
            edges_.append([
                [p1,p2],
                k,
                theta,
                interceptType,
                intercept
            ])
        edges += edges_
    return edges


def filterEdgesByRotation(edges, lRotation, threshold = 0.1):
    edges_ = []
    for edge in edges:
        theta = edge[kTheta] % (pi/2)
        if (abs(lRotation - theta)>threshold):
            # print("reject",d)
            # reject edges of length not in threshold
            continue
        edges_.append(edge)
    return edges_


def getTheta(p1,p2):
    if p1[0] == p2[0]:
        return pi/2
    else:
        return atan((p1[1]-p2[1])/(p1[0]-p2[0])) % pi

def getInterceptType(theta):
    if(theta>=pi/4 and theta < 3*pi/4):
        return 'x'
    else:
        return 'y'

def getXIntercept(point, theta):
    [x,y] = point
    if(theta == pi/2):
        return x
    else:
        return x-y/tan(theta)

def getYIntercept(point, theta):
    [x,y] = point
    return y-x*tan(theta)

def getLocalRotation(edges):
    thetas = []
    for edge in edges:
        theta = edge[kTheta]
        if(theta<pi/2):
            thetas.append(theta)
        else:
            thetas.append(theta-pi/2)
    return modeSimilarMedian(thetas)
    # return median(thetas)

def adjustIntercept(edges):
    for edge in edges:
        _type = edge[kType]
        theta = edge[kTheta]
        if _type==2 or _type == 1:
            # print(edge[kCorners])
            if _type == 2:
                edge[kCorners][0] = (edge[kCorners][0][0] + 6*sin(theta), edge[kCorners][0][1] - 6*cos(theta))
                edge[kCorners][1] = (edge[kCorners][1][0] + 6*sin(theta), edge[kCorners][1][1] - 6*cos(theta))
            else:
                edge[kCorners][0] = (edge[kCorners][0][0] - 6*sin(theta), edge[kCorners][0][1] + 6*cos(theta))
                edge[kCorners][1] = (edge[kCorners][1][0] - 6*sin(theta), edge[kCorners][1][1] + 6*cos(theta))
            if(edge[kInterceptType] == 'x'):
                edge[kIntercept] = getXIntercept(edge[kCorners][0],theta)
            else:
                edge[kIntercept] = getYIntercept(edge[kCorners][0],theta)
    return edges


def getIntersectionPoint(edge, m):
    [xe,ye] = edge[kCorners][0]
    [x0, y0] = origin
    n = 1+m**2
    if(edge[kTheta]>=pi/2):
        a = m*ye + xe
        b = m*x0 - y0
        x = (a+m*b)/n
        y = (m*a-b)/n
        return [x,y]
    else:
        c = m*xe - ye
        d = m*y0 + x0
        y = (m*d - c)/n
        x = (m*c + d)/n
        return [x,y]

def getEdgeIntersectOffset(edge, intersectPoint):
    [[x0,y0],[x1,y1]] = edge[kCorners]
    [x,y] = intersectPoint
    X = abs(x0-x)
    Y = abs(y0-y)
    dx = abs(x0-x1)
    dy = abs(y0-y1)
    if dx == 0:
        ox = 0
    else:
        ox = X%dx
    if dy == 0:
        oy = 0
    else:
        oy = Y%dy
    return sqrt(ox**2+oy**2)

def getLocalTranslation(edges, lRotation):
    m = tan(lRotation)
    dxs = []
    dys = []
    for edge in edges:
        p = getIntersectionPoint(edge, m)
        delta = getEdgeIntersectOffset(edge, p)
        if(edge[kTheta]>=pi/2):
            # print("y delta",delta,edge[kTheta])
            dys.append(delta)
        else:
            # print("x delta",delta,edge[kTheta])
            dxs.append(delta)
    return (modeSimilarMedian(dxs),modeSimilarMedian(dys))

def getLocalTranslationError(lRotation):
    coefficients = [  4.68147529e+01,-4.38234706e+03, 2.95814105e+04,-8.56231655e+04
, 1.34588696e+05,-1.23605215e+05, 6.66241559e+04,-1.99138211e+04
, 2.75286783e+03,-7.01884772e+01, 3.35821566e+01]
    d = 0
    for k,c in enumerate(coefficients):
        d += c * lRotation**k
    return d

def fixLocalTranslationError(p, lRotation, error):
    return p[0] - sin(error), p[1] + cos(error)