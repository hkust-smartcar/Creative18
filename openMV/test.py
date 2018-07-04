# test - By: dipsy - Mon Jun 18 2018

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