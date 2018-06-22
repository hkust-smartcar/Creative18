# -*- coding: utf-8 -*-
"""
Created on Fri Jun 22 14:56:59 2018

@author: dipsy
"""

from PIL import Image
import pj

path = "test2.jpg"
jpgfile = Image.open(path)

newsize = (280,240)
newfile = Image.new('RGB',newsize)

for x in range(newsize[0]):
    for y in range(newsize[1]):
        try:
            newfile.putpixel((x,y),jpgfile.getpixel(pj.pj[x][y]))
        except:
            x
            
newfile.show()