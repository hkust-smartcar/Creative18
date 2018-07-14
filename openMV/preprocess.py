import sensor
import image
import time
from util import sqdist, dist, mapToImage, mapToWorld, sgn, median, mode
from math import acos, pi, sin, cos, atan, tan


def main(rects, img):
    rects = transformRects(rects)
    edges = formEdges(rects)
    edges = filterEdgesByLength(edges)
    if(len(edges) == 0 ):
        raise Exception("no edge")
    printEdges(edges, img)
    lRotation = getLocalRotation(edges)
    return lRotation


def printEdges(edges, img):
    for edge in edges:
        [p1,p2] = edge["corners"]
        p1 = mapToImage(p1)
        p2 = mapToImage(p2)
        img.draw_line(p1[0],p1[1],p2[0],p2[1],color=(0,0,0), thickness=4)


def transformRects(rects):
    newRects = []
    for rect in rects:
        corners = rect.corners()
        newCorners = []
        for corner in corners:
            newCorners.append(mapToWorld(corner))
        newRects.append(sortCorners(newCorners))
    return newRects


def sortCorners(corners):
    d = 10000
    m = -1
    for k,c in enumerate(corners):
        d_ = sqdist([0,0],c)
        if d_ < d:
            d = d_
            m = k
    for _ in range(m):
        corners.append(corners.pop(0))
    return corners


def formEdges(rects):
    edges = []
    for rect in rects:
        edges = edgeReducer(edges, rect)
    return edges


def edgeReducer(prevE, currRect):
    for k, p2 in enumerate(currRect):    # k is the type of edge
        p1 = currRect[k-1]
        theta = getTheta(p1,p2)
        interceptType = getInterceptType(theta)
        if (interceptType == 'x'):
            intercept = getXIntercept(p1, theta)
        else:
            intercept = getYIntercept(p1, theta)
        prevE.append({
            "corners":(p1,p2),
            "type":k,
            "theta": theta,
            "interceptType": interceptType,
            "intercept": intercept
        })
    return prevE


def filterEdgesByLength(edges, threshold = (30,55)):
    edges_ = []
    for edge in edges:
        [p1,p2] = edge["corners"]
        d = dist(p1, p2)
        if (d < threshold[0] or d > threshold[1]):
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
        theta = edge["theta"]
        if(theta<pi/2):
            thetas.append(theta)
        else:
            thetas.append(theta-pi/2)
    return mode(thetas)
    # return median(thetas)

def partitionEdges(edges, theta):
    E1 = []
    E2 = []
    # for edge in edges