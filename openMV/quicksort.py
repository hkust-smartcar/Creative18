from random import randint

def swap(iterable, a, b):
    iterable[a],iterable[b] = iterable[b], iterable[a]

'''
sort in place between [l,m)
'''
def quicksort(iterable, cmp = lambda x,y: x < y, l=0, m=None):
    if(m == None): m = len(iterable)
    if(m-l <=1):
        return iterable
    pivot_index = randint(l,m-1)
    swap(iterable, pivot_index, m-1)
    pivot = iterable[m-1]
    j = l
    for i in range(l,m-1):
        if(cmp(iterable[i],pivot)):
            swap(iterable, i, j)
            j+=1
    swap(iterable, m-1, j)
    quicksort(iterable,cmp,l,j)
    quicksort(iterable,cmp,j+1,m)
    return iterable