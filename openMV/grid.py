import sensor, image, time
from util import dist, mapToImage, mapToWorld
from math import acos

def vote(votes,min_dx, threshold):
    for v in votes:
        if(abs(v-min_dx)<threshold):
            votes[v]+=1
            return
    votes[min_dx] = 1

def getHighestVote(votes, default):
    max_v_count = 0
    max_v = default
    for v in votes:
        if votes[v]>max_v_count:
            max_v_count = votes[v]
            max_v = v
    return max_v

def get_rotation(img,rects, length, threshold):
    votes = {}
    d=0
    for rk, r in enumerate(rects):
        c = r.corners()
        min_dx = 10000
        for i, p1 in enumerate(c):
            p1 = mapToWorld(p1)
            p2 = mapToWorld(c[i-1])
            img.draw_circle(p1[0],p1[1],5,color=(255,0,0))
            if p1[1] > p2[1]:
                p1, p2 = p2, p1
            dx = p2[0] - p1[0]
            if(abs(dx)<abs(min_dx)):
                min_dx = dx
        vote(votes,min_dx, threshold)
    d = getHighestVote(votes,d)
    print(d)
    print(length)
    return d,acos(d/length)-3.141592653/2

def get_length(img, rects, threshold):
    dist_votes = {}
    length = 30
    for rk, r in enumerate(rects):
        c = r.corners()
        for i, p1 in enumerate(c):
            p1 = mapToWorld(p1)
            p2 = mapToWorld(c[i-1])
            local_length = dist(p1,p2)
            if(local_length <20 or local_length >35):
                break
            vote(dist_votes, local_length, threshold)
    length = getHighestVote(dist_votes, length)
    print('length,',length)
    return length


def getGoodRects(rects, length, threshold):
    goodRects = []
    for rect in rects:
        corners = list(map(mapToWorld, rect.corners()))
        reject = False
        for k,c in enumerate(corners):
            if(abs(dist(c,corners[k-1]) - length) > threshold):
                reject = True
                break
        if not reject:
            goodRects.append(rect)
    return goodRects








