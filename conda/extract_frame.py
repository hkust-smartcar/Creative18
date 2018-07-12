import cv2
import numpy as np

"""
convert mp4 to many jpg
@param {threshold}: 1000 for different top left number, 2000 for whole different frame
"""

def extract(path = 'img/test2/test2.mp4', threshold = 1000):
    vidcap = cv2.VideoCapture(path)
    read_success = True
    prev = None
    count = 0
    while read_success:
        read_success,image = vidcap.read()
        flag = False
        if(read_success):
            if type(prev) == type(None):
                flag = True
            else:
                diff = cv2.subtract(prev, image)
                if np.any(diff):
                    diff = np.linalg.norm(diff)
                    print('frameid %d diff %d'%(count,diff))
                    if diff > threshold:
                        flag = True
        if flag:
            filename = path+"_frame%d.jpg" % count
            cv2.imwrite(filename , image)     # save frame as JPEG file
            count += 1
            print('output a new frame', filename)
        prev = image
        #print('Read a new frame: ', read_success)