# Find Lines Example
#
# This example shows off how to find lines in the image. For each line object
# found in the image a line object is returned which includes the line's rotation.

# Note: Line detection is done by using the Hough Transform:
# http://en.wikipedia.org/wiki/Hough_transform
# Please read about it above for more information on what `theta` and `rho` are.

# find_lines() finds infinite length lines. Use find_line_segments() to find non-infinite lines.

enable_lens_corr = True # turn on for straighter lines...

import sensor, image, time

def transform (x,y):
    T = [
    [0,1,0],
    [1,0,0],
    [0,0,1]
    ]
    x_ = T[0,0]*x+T[0,1]*y+T[0,2]
    y_ = T[1,0]*x+T[1,1]*y+T[1,2]

sensor.reset()
sensor.set_pixformat(sensor.GRAYSCALE) # grayscale is faster
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(time = 2000)
clock = time.clock()

# All line objects have a `theta()` method to get their rotation angle in degrees.
# You can filter lines based on their rotation angle.

min_degree = 0
max_degree = 179

# All lines also have `x1()`, `y1()`, `x2()`, and `y2()` methods to get their end-points
# and a `line()` method to get all the above as one 4 value tuple for `draw_line()`.


while(True):
    clock.tick()
    img = sensor.snapshot()
    if enable_lens_corr: img.lens_corr(1.8) # for 2.8mm lens...

    for l in img.find_lines(threshold = 1500, theta_margin = 25, rho_margin = 25):
        if (min_degree <= l.theta()) and (l.theta() <= max_degree):
            img.draw_line(l.line(), color = (255, 0, 0))
            # print(l)

    print("FPS %f" % clock.fps())

# About negative rho values:
#
# A [theta+0:-rho] tuple is the same as [theta+180:+rho].
