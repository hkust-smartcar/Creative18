import sensor, image, time
from util import dist, mapToImage, mapToWorld
from math import acos

def vote(votes,min_dx, threshold):
    for v in votes:
        if(abs(v-min_dx)<threshold):
            votes[v]+=1
            return
    votes[min_dx] = 1

def getHighestVote(votes):
    max_v_count = 0
    max_v = 0
    for v in votes:
        if votes[v]>max_v_count:
            max_v_count = votes[v]
            max_v = v
    return max_v#acos(max_v/40)


def get_rotation(img,rects, threshold):
    votes = {}
    dist_votes = {}
    for rk, r in enumerate(rects):
        c = r.corners()
        min_dx = 10000
        for i, p1 in enumerate(c):
            p1 = mapToWorld(p1)
            p2 = mapToWorld(c[i-1])
            # if(abs(dist(p1,p2)-26)>threshold):
            #     print(p1,p2,dist(p1,p2))
            #     continue
            vote(dist_votes,dist(p1,p2), threshold)
            img.draw_circle(p1[0],p1[1],5,color=(255,0,0))
            if p1[1] > p2[1]:
                p1, p2 = p2, p1
            dx = p2[0] - p1[0]
            if(abs(dx)<abs(min_dx)):
                min_dx = dx
        vote(votes,min_dx, threshold)
    return getHighestVote(votes),getHighestVote(dist_votes)