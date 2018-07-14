import sensor
import image
import time
from util import sqdist, dist, mapToImage, mapToWorld, sgn, median, mode
from math import acos, pi, sin, cos, atan, tan
from exceptions import NoEdgeException


def main(rects, img):
    transformRects(rects)
    edges = formEdges(rects)
    rects = []
    edges = filterEdgesByLength(edges)
    if(len(edges) == 0 ):
        raise NoEdgeException
    lRotation = getLocalRotation(edges)
    edges = adjustIntercept(edges)
    printEdges(edges, img)
    return lRotation


def hi(p):
    return [int(p[0]/5+50),int(p[1]/5+50)]


def printEdges(edges, img):
    c = [
        (255,0,0),
        (0,255,0),
        (0,0,255),
        (0,0,0)
    ]
    for edge in edges:
        [p1,p2] = edge["corners"]
        p1 = hi(p1)
        p2 = hi(p2)
        # p1 = mapToImage(p1)
        # p2 = mapToImage(p2)
        k = edge["type"]
        img.draw_line(p1[0],p1[1],p2[0],p2[1],color=c[k], thickness=1)
        #img.draw_string((p1[0]+p2[0])//2, (p1[1]+p2[1])//2, str(edge["type"]), color=(123,123,123),scale=1)


def transformRects(rects):
    for _ in range(len(rects)):
        corners = list(rects.pop(0).corners())
        for _ in range(len(corners)):
            corners.append(mapToWorld(corners.pop(0)))
        sortCorners(corners)
        rects.append(corners)
    # newRects = []
    # for rect in rects:
    #     corners = rect.corners()
    #     newCorners = []
    #     for corner in corners:
    #         newCorners.append(mapToWorld(corner))
    #     newRects.append(sortCorners(newCorners))
    # return newRects


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


def formEdges(rects):
    edges = []
    for rect in rects:
        for k, p2 in enumerate(rect):    # k is the type of edge
            p1 = rect[k-1]
            theta = getTheta(p1,p2)
            interceptType = getInterceptType(theta)
            if (interceptType == 'x'):
                intercept = getXIntercept(p1, theta)
            else:
                intercept = getYIntercept(p1, theta)
            edges.append({
                "corners":[p1,p2],
                "type":k,
                "theta": theta,
                "interceptType": interceptType,
                "intercept": intercept
            })
    return edges


def edgeReducer(prevE, currRect):

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

def adjustIntercept(edges):
    for edge in edges:
        _type = edge["type"]
        theta = edge["theta"]
        if _type==2 or _type == 1:
            # print(edge["corners"])
            if _type == 2:
                edge["corners"][0] = (edge["corners"][0][0] + 6*sin(theta), edge["corners"][0][1] - 6*cos(theta))
                edge["corners"][1] = (edge["corners"][1][0] + 6*sin(theta), edge["corners"][1][1] - 6*cos(theta))
            else:
                edge["corners"][0] = (edge["corners"][0][0] - 6*sin(theta), edge["corners"][0][1] + 6*cos(theta))
                edge["corners"][1] = (edge["corners"][1][0] - 6*sin(theta), edge["corners"][1][1] + 6*cos(theta))
            if(edge["interceptType"] == 'x'):
                edge["intercept"] = getXIntercept(edge["corners"][0],theta)
            else:
                edge["intercept"] = getYIntercept(edge["corners"][0],theta)
    return edges


def partitionEdges(edges, theta):
    E1 = []
    E2 = []
    for edge in edges:
        if edge["theta"]<pi:
            E1.append(edge)
        else:
            E2.append(edge)
    return E1, E2
