from util import *
"""
brief: fuse corners
"""

def fuse_corners_old(corners, threshold):
    uniqueCorners = []
    while(len(corners) > 0):
        C = corners.pop(0)
        sameCornerIndice = []
        [x,y] = C
        for k,c in enumerate(corners):
            if(dist(c,C)<threshold):
                print("insert ",c,"|",C,"|",k)
                sameCornerIndice.insert(k,0)
        for k in sameCornerIndice:
            x += corners[k][0]
            y += corners[k][1]
            corners.pop(k)
            print("remove ",k)
        #C = [int(x/(len(sameCornerIndice)+1)),int(y/(len(sameCornerIndice)+1))]
        uniqueCorners.append(C)
        print("append ",C)
    print("uniques", uniqueCorners) 
    return uniqueCorners

def s(p):
    ss = str(p[0])+","+str(p[1])
    # print(ss)
    return ss

def fuse_corners(corners, threshold):
    fused={}
    cs = []
    for C in corners:
        if s(C) in fused:
            continue
        x = C[0]
        y = C[1]
        cnt = 1
        for c in corners:
            if s(c) in fused:
                continue
            if dist(c,C) < threshold:
                fused[s(c)] = True
                x+=c[0]
                y+=c[1]
                cnt+=1
                continue
        #TODO: calculate new c from the ps list
        C = [int(x/cnt),int(y/cnt)]
        cs.append(C)
    return cs