# -*- coding: utf-8 -*-
"""
Created on Mon Jul 16 14:09:57 2018

@author: dipsy
"""
import matplotlib.pyplot as plt
import numpy as np

csv = ""
index = 1
while(True):
    try:
        csv += open("../rotation_data (%d).csv"%index,'r').read()
        index += 1
    except Exception as e:
        print(e)
        break

pairs = csv.split('\n')
#print(pairs[:10])
pairs.pop()#last data is invalid
for k, pair in enumerate(pairs):
    pairs[k] = pair.split(',')
    pairs[k][0] = float(pairs[k][0])
    pairs[k][1] = float(pairs[k][1])
    
pairs = sorted(pairs, key=lambda pair:pair[0])
print(pairs[:10])

y = list(map(lambda pair: pair[1], pairs))
x = list(map(lambda pair: pair[0], pairs))
#t = list(map(lambda pair: [1,pair[0],pair[0]**2,pair[0]**3,pair[0]**4,pair[0]**5,pair[0]**6,pair[0]**7,pair[0]**8,pair[0]**9], pairs))
z = np.polyfit(x,y,15)
print(z)
p = np.poly1d(z)

_ = plt.plot(x, y, '.', x, p(x), '-')
plt.show()