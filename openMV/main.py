# main - By: dipsy - Mon Jun 18 2018

import sensor, image, time
from fuse_corners import *
from pj import *
from jp import *
from pm import *

sensor.reset()
sensor.set_pixformat(sensor.RGB565) # grayscale is faster (160x120 max on OpenMV-M7)
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(time = 2000)
clock = time.clock()

size = [160,120]
Size = [280,240]

equations = []
corners = []

while(True):
    clock.tick()
    equations = []
    corners=[]
    img = sensor.snapshot()

    for k,r in enumerate(img.find_rects(threshold = 25000)):
        c = r.corners()
        corners += c
        for i, p in enumerate(c):
            p_ = c[i-1]
            img.draw_line(p[0],p[1],p_[0],p_[1], color=(255,0,0))
            #img.draw_circle(p[0], p[1], 5, color = (255, 0, 0))
    corners = list(map(lambda p:pm(pj,p), corners))
    print(len(corners),end=" , ")
    corners = fuse_corners(corners,5)
    print(len(corners))
    corners = list(map(lambda p:pm(jp,p), corners))
    for p in corners:
        img.draw_circle(p[0], p[1], 5, color = (0, 255, 0))
    # while(1):
    #     x=0

    #printEq(img)
