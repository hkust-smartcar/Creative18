import sensor
import image
import time
from util import sqdist, dist, mapToImage, mapToWorld, sgn, median, mode, modeSimilarMedian
from math import acos, pi, sin, cos, atan, tan
from exceptions import NoEdgeException, StupidPartitionException


def main(rects, img):
    transformRects(rects)
    edges = formEdges(rects)
    rects = []
    edges = filterEdgesByLength(edges)
    if(len(edges) == 0 ):
        raise NoEdgeException
    lRotation = getLocalRotation(edges)
    edges = filterEdgesByRotation(edges, lRotation)
    edges = adjustIntercept(edges)
    printEdges(edges, img)
    E1, E2 = partitionEdges(edges)
    t1 = partitionAlignInterceptType(E1)
    t2 = partitionAlignInterceptType(E2)
    print(t1, t2)
    [ox,oy,ix,iy,thetaIntercept] = getInterceptCommonOffsets(E1,E2, lRotation)
    print(ox,oy,ix,iy,thetaIntercept,lRotation)
    printIntercepts(E1, img)
    printIntercepts(E2, img)
    # printGrid(ox,oy,ix,iy,thetaIntercept,lRotation,img)
    return lRotation


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
        [p1,p2] = edge["corners"]
        p1 = worldToDisplayCoordinate(p1)
        p2 = worldToDisplayCoordinate(p2)
        # p1 = mapToImage(p1)
        # p2 = mapToImage(p2)
        k = edge["type"]
        img.draw_line(p1[0],p1[1],p2[0],p2[1],color=c[k], thickness=1)
        #img.draw_string((p1[0]+p2[0])//2, (p1[1]+p2[1])//2, str(edge["type"]), color=(123,123,123),scale=1)


def printIntercepts(edges, img):
    c = [
        (255,0,0),
        (0,255,0),
        (0,0,255),
        (0,0,0)
    ]
    for edge in edges:
        x,y = 0,0
        if(edge["interceptType"]=="x"):
            x = edge["intercept"]
        else:
            y = edge["intercept"]
        x,y = worldToDisplayCoordinate([x,y])
        k = edge["type"]
        img.draw_circle(x,y,5,color=c[k], thickness=1)


def printGrid(ox,oy,ix,iy,thetaIntercept,theta,img):
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
            img.draw_line(p1[0],p1[1],p2[0],p2[1],color=(255,0,255))
        for y in range(oy, w, iy):
            y_ = y - w/tan(theta)
            p1 = worldToDisplayCoordinate([0,y])
            p2 = worldToDisplayCoordinate([w,y_])
            img.draw_line(p1[0],p1[1],p2[0],p2[1],color=(255,0,255))
    else:
        for x in range(ox, w, ix):
            x_ = x - h*tan(theta)
            p1 = worldToDisplayCoordinate([x,0])
            p2 = worldToDisplayCoordinate([x_,h])
            img.draw_line(p1[0],p1[1],p2[0],p2[1],color=(255,0,255))
        for y in range(oy, w, iy):
            y_ = y + w*tan(theta)
            p1 = worldToDisplayCoordinate([0,y])
            p2 = worldToDisplayCoordinate([w,y_])
            img.draw_line(p1[0],p1[1],p2[0],p2[1],color=(255,0,255))


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


def filterEdgesByRotation(edges, lRotation, threshold = 0.1):
    edges_ = []
    for edge in edges:
        theta = edge["theta"] % (pi/2)
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
        theta = edge["theta"]
        if(theta<pi/2):
            thetas.append(theta)
        else:
            thetas.append(theta-pi/2)
    return modeSimilarMedian(thetas)
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


def partitionEdges(edges):
    E1 = []
    E2 = []
    while(len(edges)>0):
        edge = edges.pop()
        if edge["type"]%2 == 1:
            E1.append(edge)
        else:
            E2.append(edge)
    return E1, E2


def partitionAlignInterceptType(edges):
    L = list(map(lambda e:e["interceptType"], edges))
    interceptType = mode(L)
    print(interceptType,L)
    if interceptType == 'x':
        partitionChangeToUseX(edges)
    else: # y-intercept
        partitionChangeToUseY(edges)
    return interceptType


def partitionChangeToUseX(edges):
    for edge in edges:
        if(edge["theta"]==0):
            continue #this type of edge is ridiculous
        if(edge["interceptType"]!='x'):
            edge["interceptType"] = 'x'
            edge["intercept"] = getXIntercept(edge["corners"][0],edge["theta"])


def partitionChangeToUseY(edges):
    for edge in edges:
        if(edge["theta"]==pi/2):
            continue #this type of edge is ridiculous
        if(edge["interceptType"]!='y'):
            edge["interceptType"] = 'y'
            edge["intercept"] = getYIntercept(edge["corners"][0],edge["theta"])


def getInterceptCommonOffsets(E1, E2, lRotation):
    ix = 50/sin(lRotation)
    iy = 50/sin(lRotation)
    if(E1[0]["interceptType"] == E2[0]["interceptType"]):
        if(E1[0]["interceptType"] == 'x'):
            partitionChangeToUseY(E2)
        else:
            partitionChangeToUseX(E1)
    elif(E1[0]["interceptType"] != 'x'):
        E1, E2 = E1, E2
    ox = modeSimilarMedian(list(map(lambda e:e["intercept"]%ix, E1)))
    oy = modeSimilarMedian(list(map(lambda e:e["intercept"]%iy, E2)))
    return [ox,oy,ix,iy,getInterceptType(lRotation)]
