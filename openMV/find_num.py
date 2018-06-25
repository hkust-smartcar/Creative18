import sensor, image, time
from fuse_corners import *
from util import mapToWorld, mapToImage, inRange
from math import cos, sin

def affine(p,si,co,l,dx,dy):
    [x,y] = p
    #print(c*x0)
    x=x*l/24
    y=y*l/24
    x1 = co*x - si*y
    y1 = si*x + co*y
    x0 = (x1+dx)
    y0 = y1+dy
    return (int(x0), int(y0))

def find_num(img, rect, theta, length):
    num_img = img.copy((0,0,24,24)).clear()
    co = cos(theta)
    si = sin(theta)
    corners = rect.corners()
    corners = list(map(mapToWorld,corners))
    dx=0
    dy=10000
    for c in corners:
        if(c[1]<dy):
            dy=c[1]
            dx=c[0]
    for x in range(24):
        for y in range(24):
            p=affine((x,y),si,co,length,dx,dy)
            p=mapToImage(p)
            if inRange((160,120),p):
                num_img.set_pixel(x,y,img.get_pixel(p[0],p[1]))
                img.set_pixel(x,y,num_img.get_pixel(x,y)) 
    print('ha')
    num_img.save("/test.jpg")
    print('wey')
    return num_img
    