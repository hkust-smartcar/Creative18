# main - By: dipsy - Mon Jun 18 2018

import sensor
import image
import time
import pyb
from util import mapToWorld, mapToImage, deg
from math import sin, cos
import preprocess
from exceptions import NoEdgeException, NotEnoughDataException

import comm
import protocol

draw = True

mycomm = comm.Comm(print)

protocol.init(mycomm)

sensor.reset()
# grayscale is faster (160x120 max on OpenMV-M7)
sensor.set_pixformat(sensor.RGB565)
# sensor.set_pixformat(sensor.GRAYSCALE)
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(time=2000)
clock = time.clock()

size = [160, 120]
Size = [280, 240]

equations = []
corners = []

gRotation = 0
lRotation = 0
gTranslation = [0,0]
lTranslation = [0,0]

frame_id = 0

while(True):
    clock.tick()
    equations = []
    corners = []
    img = sensor.snapshot().lens_corr(1.8,1)
    startTime = pyb.millis()

    img.draw_string(0,0,str(frame_id),scale=2,color=(255,255,255))
    img.draw_string(0,0,str(frame_id),scale=2,color=(0,0,0))

    frame_id += 1

    #get rects
    rects = img.find_rects(threshold=0)
    
    if(len(rects) == 0):
        continue

    #draw raw rects
    for k, r in enumerate(rects):
        c = r.corners()
        for i, p in enumerate(c):
            p_ = c[i-1]
            if draw:
                img.draw_line(p[0], p[1], p_[0], p_[1], 5, color=(0, 0, 0))
    
    try:
        theta = preprocess.main(rects,img)
    except NoEdgeException as e:
        # print(e)
        continue
    except NotEnoughDataException as e:
        continue

    # gRotation = getGlobalRotation(gRotation, lRotation, theta)
    # protocol.feedGlobalRotation(gRotation, pyb.millis() - startTime,frame_id)
    protocol.feedLocalRotation(theta, pyb.millis() - startTime,frame_id)
    lRotation = theta

    #display the rotations
    if draw:
        [x0, y0] = mapToImage([100, 200])
        [x1, y1] = mapToImage([100-int(40*cos(lRotation)), 200-int(40*sin(lRotation))])
        img.draw_line(x0, y0, x1, y1, color=(255, 255, 255), thickness=5)
        img.draw_line(x0, y0, x1, y1, color=(0, 0, 0), thickness=2)

        # [x0, y0] = [70, 60]
        # [x1, y1] = [70-int(40*cos(gRotation)), 60-int(40*sin(gRotation))]
        # img.draw_line(x0, y0, x1, y1, color=(255, 255, 255), thickness=5)
        # img.draw_line(x0, y0, x1, y1, color=(0, 0, 0), thickness=2)
    

    
    
