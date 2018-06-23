# -*- coding: utf-8 -*-
"""
Created on Sat Jun 23 20:25:17 2018

@author: dipsy
"""

from PIL import Image
from math import cos, sin

path = "tilted_square.jpg"
jpgfile = Image.open(path)

fixedfile = Image.new('RGB',(24,24))

#jpgfile.show()
def dist(p1,p2):
    return sqrt((p1[0]-p2[0])**2+(p1[1]-p2[1])**2)

theta = 30

corners = [[38,28],[66,45],[49,73],[21,57]]
l = dist(corners[0],corners[1])

dy = 10000
dx=0

co = cos(theta*2*3.14/360)
si = sin(theta*2*3.14/360)

for c in corners:
    if(c[1]<dy):
        dy=c[1]
        dx=c[0]


def pm2(p):
    [x,y] = p
    #print(c*x0)
    x=x*l/24
    y=y*l/24
    x1 = co*x - si*y
    y1 = si*x + co*y
    x0 = (x1+dx)
    y0 = y1+dy
    return (int(x0), int(y0))

for x in range(100):
    for y in range(100):
        p=pm2((x,y))
        try:
            fixedfile.putpixel((x,y),jpgfile.getpixel(p))
        except:
            x
            #print((x,y),p)
        
fixedfile.show()
fixedfile.save("tsquarefixed.jpg")