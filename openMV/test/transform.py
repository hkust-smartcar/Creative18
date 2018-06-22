# -*- coding: utf-8 -*-
"""
Created on Mon Jun 18 21:39:57 2018

@author: dipsy
"""
import numpy as np
from PIL import Image
import json


newsize = (280,240)
src = [[55,79], [107,81], [102,51], [62,50]]
tgt = [[120,200],[160,200],[160,160],[120,160]]

"""
pj = np.squeeze(np.asarray(np.matrix('''
                       1 0 0;
                       0 1 0;
                       0 0 1
                       ''')))
"""

"""
brief: generate a static array which map newsize to oldsize
param: coord in newsize
return: coord in oldsize
"""
def generate(h,name):
    f = open(name+".py","w")
    f2 = open("../"+name+".py","w")
    h=list(h)
    h[0] = list(h[0])
    h[1] = list(h[1])
    h[2] = list(h[2])
    pyContent = name+" = "+json.dumps(list(h)).replace("[","(").replace("]",")")
    f.write(pyContent)
    f2.write(pyContent)
    f.close()
    f2.close()

"""
brief: given arrays of source points and target points
return a projection matrix that map source points to target points
link: https://stackoverflow.com/questions/35819142/calculate-a-2d-homogeneous-perspective-transformation-matrix-from-4-points-in-ma
link: https://math.stackexchange.com/questions/96662/augmented-reality-transformation-matrix-optimization
"""
def getProjMatrix(s,t):
    N = [[0] * 13 for i in range(13)]
    #[[x1,y1],[x2,y2],[x3,y3],[x4,y4]] = s
    #[[_x1,_y1],[_x2,_y2],[_x3,_y3],[_x4,_y4]] = t
    N[0][0] = N[1][3] = N[2][6] = s[0][0]   #x1
    N[3][0] = N[4][3] = N[5][6] = s[1][0]   #x2
    N[6][0] = N[7][3] = N[8][6] = s[2][0]   #x3
    N[9][0] = N[10][3] = N[11][6] = s[3][0]#x4
    N[0][1] = N[1][4] = N[2][7] = s[0][1]
    N[3][1] = N[4][4] = N[5][7] = s[1][1]
    N[6][1] = N[7][4] = N[8][7] = s[2][1]
    N[9][1] = N[10][4] = N[11][7] = s[3][1]
    N[0][2] = N[1][5] = N[2][8] = 1
    N[3][2] = N[4][5] = N[5][8] = 1
    N[6][2] = N[7][5] = N[8][8] = 1
    N[9][2] = N[10][5] = N[11][8] = 1
    N[0][9] = -t[0][0]
    N[1][9] = -t[0][1]
    N[2][9] = -1
    N[3][10] = -t[1][0]
    N[4][10] = -t[1][1]
    N[5][10] = -1
    N[6][11] = -t[2][0]
    N[7][11] = -t[2][1]
    N[8][11] = -1
    N[9][12] = -t[3][0]
    N[10][12] = -t[3][1]
    N[11][12] = -1
    N[12][8] = 1
    y = [0]*13
    y[12] = 1
    y = np.transpose(y)
    x = np.linalg.solve(N,y)
    print(x)
    p = [[x[0],x[1],x[2]],[x[3],x[4],x[5]],[x[6],x[7],x[8]]]
    return p

def projmap (pj,src):
    [[a,b,c],[d,e,f],[g,h,_]] = pj
    [x,y] = src
    _x = int((a*x+b*y+c)//(g*x+h*y+1))
    _y = int((d*x+e*y+f)//(g*x+h*y+1))
    #if(_x < 0 or _y < 0): print (src,(x,y))
    return (_x,_y)

def pm2(h,s):
    [x,y]=s
    return (int((h[0][0]*x+h[0][1]*y+h[0][2])/(h[2][0]*x+h[2][1]*y+h[2][2])),\
            int((h[1][0]*x+h[1][1]*y+h[1][2])/(h[2][0]*x+h[2][1]*y+h[2][2])))

"""
link: link: https://math.stackexchange.com/questions/96662/augmented-reality-transformation-matrix-optimization
"""
def getProjMatrix2(s,t):
    [[x1,y1],[x2,y2],[x3,y3],[x4,y4]] = s
    [[_x1,_y1],[_x2,_y2],[_x3,_y3],[_x4,_y4]] = t
    #N = [[x1,y1,1,0,0,0,-x1*_x1,-y1*_x1],[x2,y2,1,0,0,0,-x2*_x2,-y2*_x2],[x3,y3,1,0,0,0,-x3*_x3,-y3,_x3],[x4,y4,1,0,0,0,-x4*_x4,-y4,_x4],[0,0,0,x1,y1,1,-x1*_y1,-y1*_y1],[0,0,0,x2,y2,1,-x2*_y2,-y2*_y2],[0,0,0,x3,y3,1,-x3*_y3,-y3*_y3],[0,0,0,x4,y4,1,-x4*_y4,-y4*_y4]]
    #y = [0]*8
    #for i in range(4): y[i]=t[i][0];y[i+4]=t[i][1]
    #y = np.transpose(y)
    #print(N,y)
    #print(y.dtype)
    N = [[x1, y1, 1, 0, 0, 0, -x1 * _x1, -y1 * _x1],\
    [x2, y2, 1, 0, 0, 0, -x2 * _x2, -y2 * _x2],\
    [x3, y3, 1, 0, 0, 0, -x3 * _x3, -y3* _x3],\
    [x4, y4, 1, 0, 0, 0, -x4 * _x4, -y4* _x4],\
    [0, 0, 0, x1, y1, 1, -x1 * _y1, -y1 * _y1],\
    [0, 0, 0, x2, y2, 1, -x2 * _y2, -y2 * _y2],\
    [0, 0, 0, x3, y3, 1, -x3 * _y3, -y3 * _y3],\
    [0, 0, 0, x4, y4, 1, -x4 * _y4, -y4 * _y4]]
    print(N)
    print([_x1, _x2, _x3, _x4, _y1, _y2, _y3, _y4])
    x = np.linalg.solve(N,[_x1, _x2, _x3, _x4, _y1, _y2, _y3, _y4])
    print(x)
    p = [[x[0],x[1],x[2]],[x[3],x[4],x[5]],[x[6],x[7],1]]
    return p

pj = getProjMatrix2(src,tgt)
print(pm2(pj,[0,0]))
print(pm2(pj,[0,1]))
print(pm2(pj,[1,1]))
print(pm2(pj,[1,0]))
pj_ = np.linalg.inv(pj)

path = "test2.jpg"
jpgfile = Image.open(path)
print(jpgfile.bits, jpgfile.size, jpgfile.format)
print('new size',newsize)
newfile = Image.new('RGB',newsize)
for x in range(newsize[0]):
    for y in range(newsize[1]):
        try:
            p = pm2(pj_,(x,y))
            newfile.putpixel((x,y),jpgfile.getpixel(p))
        except:
            x
        
#for x in range(jpgfile.size[0]):
#    for y in range(jpgfile.size[1]):
#        try:
#            p = pm2(pj,(x,y))
#            newfile.putpixel(p,jpgfile.getpixel((x,y)))
#        except Exception as e:
#            x
            #print ((x,y),p,e)
            #raise
newfile.show()
generate(pj_,"pj")
generate(pj,"jp")