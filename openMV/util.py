from math import sqrt

"""
give two points, return euclidean distance
"""
def dist(p1,p2):
    return sqrt((p1[0]-p2[0])**2+(p1[1]-p2[1])**2)

"""
give upperbound and point, return whether point is in range
lowerbound is fixed to be (0,0)
"""
def inRange(r,p):
    return p[0]>=0 and p[1]>=0 and p[0]<r[0] and p[1]<r[1]
