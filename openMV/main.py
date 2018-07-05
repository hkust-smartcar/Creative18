# main - By: dipsy - Mon Jun 18 2018

import sensor
import image
import time
import pyb
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

sensor.reset()
# grayscale is faster (160x120 max on OpenMV-M7)
# sensor.set_pixformat(sensor.RGB565)
sensor.set_pixformat(sensor.GRAYSCALE)
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
    startTime = pyb.millis()

    #get rects
    rects = img.find_rects(threshold=10000)

    #draw raw rects
    for k, r in enumerate(rects):
        c = r.corners()
        for i, p in enumerate(c):
            p_ = c[i-1]
            img.draw_line(p[0], p[1], p_[0], p_[1], 5, color=(255, 255, 0))
    
    #get mode length
    length = get_length(img, rects, 2)
    if(length == 0):
        continue

    #filter rects
    rects = getGoodRects(rects, length, 10)

    #calculate the rotation
    dx, theta = get_rotation(img, rects, length, 2)
    gRotation = getGlobalRotation(gRotation, lRotation, theta)
    protocol.feedGlobalRotation(gRotation, pyb.millis() - startTime,frame_id)
    lRotation = theta
    print('deg', deg(gRotation), 'theta', theta)

    #display the rotations
    [x0, y0] = mapToImage([100, 200])
    [x1, y1] = mapToImage([100-dx, 200-length])
    img.draw_line(x0, y0, x1, y1, color=(255, 255, 255), thickness=5)
    img.draw_line(x0, y0, x1, y1, color=(0, 0, 0), thickness=2)

    [x0, y0] = [70, 60]
    [x1, y1] = [70-int(length*cos(gRotation)), 60-int(length*sin(gRotation))]
    img.draw_line(x0, y0, x1, y1, color=(255, 255, 255), thickness=5)
    img.draw_line(x0, y0, x1, y1, color=(0, 0, 0), thickness=2)
    

    #display the good rects and push corners
    for k, r in enumerate(rects):
        c = r.corners()
        corners += c
        for i, p in enumerate(c):
            p_ = c[i-1]
            img.draw_line(p[0], p[1], p_[0], p_[1], 5, color=(255, 0, 0))
    
    #send the filtered corners for debug
    #protocol.feedCorners(frame_id,corners)


    #calculate the translation
    #simplify the grid
    corners = list(map(mapToWorld, corners))
    # corners = fuse_corners(corners, 20)
    corners = getRotateCorners(img, corners, theta)
    # print(corners)

    #draw transformed corners
    for p in corners:
        img.draw_circle((p[0])//2, p[1]//2, 5, color=(0, 255, 0))

    #get merged length
    # m_len = getMergedRotatedCornerLength(corners)
    # print('m_length',m_len)

    #get displacement
    # localDisplacement = getLocalDisplacement(img, corners,m_len)
    # print('local displacement',localDisplacement)

    protocol.feedCorners(frame_id,corners)

    img.draw_string(0,0,str(frame_id),scale=2,color=(255,255,255))
    img.draw_string(0,0,str(frame_id),scale=2,color=(0,0,0))

    frame_id += 1
