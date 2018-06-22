def pushEq(p1,p2):
    [x1,y1] = p1
    [x2,y2] = p2
    m = (y1-y2)/(x1-x2+0.1)
    c = y1 - m*x1
    flag = False
    for k,eq in enumerate(equations):
        #print(eq)
        if(abs(m-eq["m"])<0.1 and abs(c-eq["c"])<5):
            equations[k] = {"m":(m+eq["m"])/2, "c":(c+eq["c"])/2}
            flag = True
            break
    if(not flag):
        equations.append({"m":m,"c":c})

def printEq(img):
    for eq in equations:
        m,c = eq["m"],eq["c"]
        p1 = [0,c]
        if(not inRange(size,p1)):
            p1 = [-c/m,0]
        p2 = [size[0],m*size[0]+c]
        if(not inRange(size,p2)):
            p2 = [(size[1]-c)/m,size[1]-c]
        img.draw_line(int(p1[0]),int(p1[1]),int(p2[0]),int(p2[1]),color=(255,0,0))