# main - By: dipsy - Mon Jun 18 2018

import sensor, image, time
from fuse_corners import *
from get_rotation import get_rotation
from util import mapToWorld, mapToImage

sensor.reset()
sensor.set_pixformat(sensor.RGB565) # grayscale is faster (160x120 max on OpenMV-M7)
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(time = 2000)
clock = time.clock()

size = [160,120]
Size = [280,240]

equations = []
corners = []

id=0

while(True):
    clock.tick()
    equations = []
    corners=[]
    img = sensor.snapshot()

    rects = img.find_rects(threshold = 25000)
    dx,l = get_rotation(img,rects,5)
    #print(dx,l)
    [x0,y0] = mapToImage([140,240])
    [x1,y1] = mapToImage([140-dx,240-l])
    img.draw_line(x0,y0,x1,y1, color=(255,0,0))
    for k,r in enumerate(rects):
        c = r.corners()
        corners += c
        for i, p in enumerate(c):
            p_ = c[i-1]
            img.draw_line(p[0],p[1],p_[0],p_[1], 5, color=(255,0,0))
    r = rects[0]
    kpts = img.find_keypoints(r.rect())
    print(kpts)
    break
    #         #img.draw_circle(p[0], p[1], 5, color = (255, 0, 0))
    #corners = list(map(mapToWorld, corners))
    #print(len(corners),end=" , ")
    #corners = fuse_corners(corners,5)
    #print(len(corners))
    #corners = list(map(mapToImage, corners))
    #for p in corners:
        #img.draw_circle(p[0], p[1], 5, color = (0, 255, 0))


    # while(1):
    #     x=0
    # img.save("/img/testing"+str(id)+".bmp")
    # id+=1
    #printEq(img)
