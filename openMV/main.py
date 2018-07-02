# main - By: dipsy - Mon Jun 18 2018

import sensor
import image
import time
from fuse_corners import fuse_corners
from find_num import find_num
from grid import get_rotation,\
get_length,\
getGoodRects,\
getGlobalRotation,\
getGlobalRotationWithDirection,\
getRotateCorners,\
getLocalDisplacement,\
getMergedRotatedCornerLength
from util import mapToWorld, mapToImage, deg
from math import sin, cos

import comm
import protocol

mycomm = comm.Comm(print)

protocol.init(mycomm)
# protocol.test()
# protocol.test2()

# while(True):
#     x = 0
#     mycomm.listen()
#     mycomm.period()

sensor.reset()
# grayscale is faster (160x120 max on OpenMV-M7)
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(time=2000)
clock = time.clock()

size = [160, 120]
Size = [280, 240]

equations = []
corners = []

gRotation = 0
lRotation = 0

frame_id = 0

while(True):
    clock.tick()
    equations = []
    corners = []
    img = sensor.snapshot().lens_corr(1.8,1)
    rects = img.find_rects(threshold=10000)
    for k, r in enumerate(rects):
        c = r.corners()
        corners += c
        for i, p in enumerate(c):
            p_ = c[i-1]
            img.draw_line(p[0], p[1], p_[0], p_[1], 5, color=(255, 255, 0))
    protocol.feedCorners(frame_id,corners)
    img.draw_line(80, 120, 80, 0, 5, color=(255, 0, 0))
    frame_id+=1

while(True):
    clock.tick()
    equations = []
    corners = []
    img = sensor.snapshot()#.lens_corr(1.8,1)

    rects = img.find_rects(threshold=10000)
    for k, r in enumerate(rects):
        c = r.corners()
        corners += c
        for i, p in enumerate(c):
            p_ = c[i-1]
            img.draw_line(p[0], p[1], p_[0], p_[1], 5, color=(255, 255, 0))
    # try:
    length = get_length(img, rects, 2)
    print('p_length',length)
    rects = getGoodRects(rects, length, 10)
    dx, theta = get_rotation(img, rects, length, 2)
    gRotation = getGlobalRotation(gRotation, lRotation, theta)
    #gRotation = getGlobalRotationWithDirection(gRotation,lRotation,theta,False)
    lRotation = theta
    # print('deg', deg(gRotation), 'theta', theta)
    # except Exception as e:
    # print(e)
    # continue
    # print(dx,length,theta)
    [x0, y0] = mapToImage([140, 240])
    [x1, y1] = mapToImage([140-dx, 240-length])
    img.draw_line(x0, y0, x1, y1, color=(255, 0, 0))

    [x0, y0] = [70, 60]
    [x1, y1] = [70-int(length*cos(gRotation)), 60-int(length*sin(gRotation))]
    img.draw_line(x0, y0, x1, y1, color=(0, 255, 0))

    for k, r in enumerate(rects):
        c = r.corners()
        corners += c
        for i, p in enumerate(c):
            p_ = c[i-1]
            img.draw_line(p[0], p[1], p_[0], p_[1], 5, color=(255, 0, 0))

    protocol.feedCorners(frame_id, corners)
    # corners = list(map(mapToWorld, corners))
    # corners = fuse_corners(corners, 5)
    # corners = getRotateCorners(img, corners, theta)
    # print(corners)
    # for p in corners:
    #     img.draw_circle((p[0])//2, p[1]//2, 5, color=(0, 255, 0))
    # m_len = getMergedRotatedCornerLength(corners)
    # m_len = 24
    # print('m_length',m_len)
    # if(m_len == 40):
    #     print('corners',len(corners))
    #     continue
    # print('local displacement',getLocalDisplacement(img,corners,m_len))
    #localDisplacement = getLocalDisplacement(img, corners)
    # rects = getrects(rects, length, 10)
    # if len(rects):
    #     r = rects[0]
    #     img.draw_rectangle(r.rect(),color=(0,255,0))
    #     find_num(img,r,theta,length)

    #         #img.draw_circle(p[0], p[1], 5, color = (255, 0, 0))
    #
    #print(len(corners),end=" , ")
    #corners = fuse_corners(corners,5)
    # print(len(corners))
    #corners = list(map(mapToImage, corners))
    # for p in corners:
        #img.draw_circle(p[0], p[1], 5, color = (0, 255, 0))

    # while(1):
    #     x=0
    # img.save("/img/testing"+str(id)+".bmp")
    # id+=1
    # printEq(img)
    frame_id += 1
