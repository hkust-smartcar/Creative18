# Untitled - By: dipsy - Mon Jun 18 2018

import sensor, image, time

sensor.reset()
sensor.set_pixformat(sensor.RGB565) # grayscale is faster (160x120 max on OpenMV-M7)
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(time = 2000)
clock = time.clock()

while(True):
    clock.tick()
    img = sensor.snapshot()
    img.lens_corr(1.8)

    # `threshold` below should be set to a high enough value to filter out noise
    # rectangles detected in the image which have low edge magnitudes. Rectangles
    # have larger edge magnitudes the larger and more contrasty they are...

    for r in img.find_rects(threshold = 30000):
        #img.draw_rectangle(r.rect(), color = (255, 0, 0))
        c = r.corners()
        for i, p in enumerate(c):
            print(i,p,c)
            print("yo")
            p_ = c[i-1]
            img.draw_line(p[0],p[1],p_[0],p_[1], color=(255,0,0))
            img.draw_circle(p[0], p[1], 5, color = (0, 255, 0))
        #print(r)

    print("FPS %f" % clock.fps())
