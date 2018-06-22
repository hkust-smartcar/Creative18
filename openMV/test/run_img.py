# -*- coding: utf-8 -*-
"""
Created on Fri Jun 22 21:10:31 2018

@author: dipsy
"""

from PIL import Image
from jp import *
from pj import *

path = "Capture2.jpg"
jpgfile = Image.open(path)

oldsize = (160,120)
fixedsize = (280,240)
fixedfile = Image.new('RGB',fixedsize)

def pm2(h,s):
    [x,y]=s
    return (int((h[0][0]*x+h[0][1]*y+h[0][2])/(h[2][0]*x+h[2][1]*y+h[2][2])),\
            int((h[1][0]*x+h[1][1]*y+h[1][2])/(h[2][0]*x+h[2][1]*y+h[2][2])))

for x in range(fixedsize[0]):
    for y in range(fixedsize[1]):
        try:
            fixedfile.putpixel((x,y),jpgfile.getpixel(pm2(pj,(x,y))))
        except:
            x
            
fixedfile.show()
fixedfile.save(path+"_fixed.jpg")