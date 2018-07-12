# -*- coding: utf-8 -*-
"""
Created on Fri Jun 22 14:56:59 2018

@author: dipsy
"""

from PIL import Image
from jp import jp
from pj import pj

def fix(path = "img/test2.jpg", oldsize = (160,120), fixedsize = (300,250), preview = True):

    jpgfile = Image.open(path)
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
                
    if preview:
        fixedfile.show()
    fixedfile.save(path+"fixed.jpg")
    print("done fixing "+path)
    
    #path = "fixed.jpg"
    #jpgfile = Image.open(path)
    
    #ofile = Image.new('RGB',oldsize)
    
    #for x in range(oldsize[0]):
    #    for y in range(oldsize[1]):
    #        try:
    #            ofile.putpixel((x,y),jpgfile.getpixel(pm2(jp,(x,y))))
    #        except:
    #            x
                
    #ofile.show()

def fixforeach(path = "img/vid/vid.mp4", index = 0, end = 146, preview = False):
    while(index < end):
        fix(path+"_frame"+str(index)+".jpg", preview = preview)
        index+=1
    print("done fix for each")