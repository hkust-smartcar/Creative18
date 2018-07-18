# -*- coding: utf-8 -*-
"""
Created on Mon Jul 16 14:09:57 2018

@author: dipsy
"""
import matplotlib.pyplot as plt
import numpy as np

csv = ""
index = 1
#while(True):
#    try:
#        csv += open("../rotation_data (%d).csv"%index,'r').read()
#        index += 1
#    except Exception as e:
#        print(e)
#        break
csv += open("../rotation_data (19).csv",'r').read()

theta0 = []
theta1 = []
theta2 = []
y0 = []
y1 = []
y2 = []


pairs = csv.split('\n')
#print(pairs[:10])
pairs.pop()#last data is invalid
for k, pair in enumerate(pairs):
    pairs[k] = pair.split(',')
    pairs[k][0] = float(pairs[k][0])
    pairs[k][1] = float(pairs[k][1])
    pairs[k][2] = float(pairs[k][2])
#    if pairs[k][0] < 0.63:
#        pairs[k][3] = float(pairs[k][3]) + 50 
#    elif pairs[k][0] > 1.5 and float(pairs[k][3]) < 20:
#        pairs[k][3] = 40
#    else:
    pairs[k][3] = float(pairs[k][3])
    if pairs[k][0] < 0.6:
        theta0.append(pairs[k][0])
        y0.append(pairs[k][3])
    elif pairs[k][0] < 1.2:
        theta1.append(pairs[k][0])
        y1.append(pairs[k][3])
    else:
        theta2.append(pairs[k][0])
        y2.append(pairs[k][3])
        
    
pairs = sorted(pairs, key=lambda pair:pair[0])
print(pairs[:10])

theta = list(map(lambda pair: pair[0], pairs))
dist = list(map(lambda pair: pair[1], pairs))
x = list(map(lambda pair: pair[2], pairs))
y = list(map(lambda pair: pair[3], pairs))
#t = list(map(lambda pair: [1,pair[0],pair[0]**2,pair[0]**3,pair[0]**4,pair[0]**5,pair[0]**6,pair[0]**7,pair[0]**8,pair[0]**9], pairs))
fdist = np.polyfit(theta,dist,10)
print("fdist",fdist.tolist())
pdist = np.poly1d(fdist)

fx = np.polyfit(theta,x,3)
print("fx",fx)
px = np.poly1d(fx.tolist())

fy = np.polyfit(theta,y,3)
print("fy",fy)
py = np.poly1d(fy.tolist())


fy0 = np.polyfit(theta0,y0,1)
print("fy0",fy0)
py0 = np.poly1d(fy0.tolist())

fy1 = np.polyfit(theta1,y1,1)
print("fy1",fy1)
py1 = np.poly1d(fy1.tolist())

fy2 = np.polyfit(theta2,y2,1)
print("fy2",fy2)
py2 = np.poly1d(fy2.tolist())

_ = plt.plot(theta, dist, '.', theta, pdist(theta), '-', theta, x, '.', theta, px(theta), '-', theta,y,'.',theta0,py0(theta0),'-',theta1,py1(theta1),'-',theta2,py2(theta2),'-',theta,py(theta),'-')
plt.show()