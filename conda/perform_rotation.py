# -*- coding: utf-8 -*-
"""
Created on Fri Jul  6 16:50:50 2018

@author: dipsy
"""

from PIL import Image
from math import cos, sin, pi
import json
from pprint import pprint

def rotateforeach(vid_path = "img/test2/test2.mp4",
           json_path = "img/test2/frame_details.txt",
           old_size = (300,250),
           rotation_origin = (145, 270),
           offset = 0,
           last_frame = 160,
           preview = False):
           

    with open(json_path) as f:
        data = json.load(f)
    
    rotations = data["local_rotations"]
    
    def rotateSingle(img_path, theta, preview = False):
        fixedfile = Image.new('RGB',(500,500))
        jpgfile = Image.open(img_path)
        
        co = cos(theta)
        si = sin(theta)
        
        def rotateTransformPx(p):
            x = p[0] - rotation_origin[0]
            y = rotation_origin[1] - p[1]
            x1 = co*x - si*y
            y1 = si*x + co*y
            return (int(x1) + rotation_origin[0], rotation_origin[1] - int(y1))
        
        for x in range(old_size[0]):
            for y in range(old_size[1]):
                p=rotateTransformPx((x,y))
                try:
                    fixedfile.putpixel(p,jpgfile.getpixel((x,y)))
                except:
                    x
        
        if preview:           
            fixedfile.show()
        fixedfile.save(img_path+"rotated.jpg")
        print("done rotating"+img_path)
    
    for index in range(offset, last_frame+1,1):
        try:
            theta = rotations[str(index - offset)] #-0.158650429
            rotateSingle(vid_path+"_frame"+str(index)+".jpgfixed.jpg", theta)
        except Exception as e:
            print(e)
        
    print("done rotate foreach")
    
#rotateforeach(vid_path = "img/test3/test3.mp4", json_path="img/test3/frame_details.txt", offset=2)