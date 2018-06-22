# -*- coding: utf-8 -*-
"""
Created on Wed Jun 20 21:05:38 2018

@author: dipsy
"""
from json import dumps
from math import sqrt
from copy import deepcopy
from collections import OrderedDict 

def dist(a,b):
    a=a.split(',')
    b=b.split(',')
    a[0] = int(a[0])
    a[1] = int(a[1])
    b[0] = int(b[0])
    b[1] = int(b[1])
    print('yoooo',a,b)
    return sqrt((a[0] - b[0])**2+ (a[1] - b[1])**2)

def stablemarriage(list_a, list_b,rankAB,rankBA):
    A = deepcopy(list_a)
    B = deepcopy(list_b)
    #rA = map(lambda a: {a: map(lambda b: {b:rankAB(a,b)},B)},A)
    #rB = map(lambda b: {b: map(lambda a: {a:rankBA(b,a)},A)},B)
    #print(dumps(rA))
    rA = {}
    for a in A:
        rA[a] = sorted(B, cmp=lambda b1,b2:rankAB(a,b1)<rankAB(a,b2))
    single = deepcopy(A)
    marriage = {}
    while len(single)>0:
        a = single.pop(0)
        print(rA[a])
        b = list(rA[a].keys())[0]
        if(not (b in marriage) or rankBA(b,a)>rankBA(b,marriage[b])):
            if(b in marriage):
                single.append(marriage[b])
            marriage[b]=a
    print('marriage',marriage)
    return list(map(lambda b:[marriage[b],b,rankAB(a,b),rankBA(B,A)] ,marriage.keys()))
A = ["0,0","1,1","2,2","3,3"]
print(dumps(stablemarriage(A,A,dist,dist), ensure_ascii=False))