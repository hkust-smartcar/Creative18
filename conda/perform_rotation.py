# -*- coding: utf-8 -*-
"""
Created on Fri Jul  6 16:50:50 2018

@author: dipsy
"""

from PIL import Image
from math import cos, sin, pi
import json
from pprint import pprint

json_path = "img/test2/frame_rotations2.txt"
img_path = "img/test2/test2.mp4_frame60.jpgfixed.jpg"
old_size = (200,250)
rotation_origin = (95, 270)
frame_id = 17

with open(json_path) as f:
    data = json.load(f)

rotations = data

fixedfile = Image.new('RGB',(500,500))
jpgfile = Image.open(img_path)

theta = -0.158650429 #rotations[str(frame_id)]
co = cos(theta)
si = sin(theta)

def rotateTransform(p):
    x = p[0] - rotation_origin[0]
    y = rotation_origin[1] - p[1]
    x1 = co*x - si*y
    y1 = si*x + co*y
    return (int(x1)+2*rotation_origin[0], rotation_origin[1] - int(y1))

for x in range(old_size[0]):
    for y in range(old_size[1]):
        p=rotateTransform((x,y))
        try:
            fixedfile.putpixel(p,jpgfile.getpixel((x,y)))
        except:
            x
            
fixedfile.show()