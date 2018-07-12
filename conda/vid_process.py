# -*- coding: utf-8 -*-
"""
Created on Thu Jul 12 11:35:26 2018

@author: dipsy
"""

import extract_frame
import perform_transform
#import perfrom_rotation

path = "img/test3/test3.mp4"

extract_frame.extract(path)
perform_transform.fixforeach(path, index=2, end=160)