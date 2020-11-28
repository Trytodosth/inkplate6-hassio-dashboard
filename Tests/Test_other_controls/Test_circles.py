# -----------
# Simple python script to
# create icon header files for Inkplate 6
# Arduino sketches
#
# Takes all files from /icons and saves them to /binary_icons
#
# -----------

from PIL import Image
import os, logging
import PIL
import math

from matplotlib.pyplot import imshow
import numpy as np

def radius_error(x,y,r_square):
    return x**2 + y**2 - r_square

def add_pixel(x, y, center):
    output_im.putpixel((x+center[0], y+center[1]), 255)

    
def add_pixel_if(x, y, scenario, x_begin, x_end):
    if scenario == 1:
        if y >= 0 and x <= x_begin and x >= x_end:
            output_im.putpixel((x+center[0], -y+center[1]), 255)
    if scenario == 11:
        if y <= 0 and x >= x_begin and x <= x_end:
            output_im.putpixel((x+center[0], -y+center[1]), 255)
    if scenario == 2:
        if y >= 0 and (x <= x_begin or x >= x_end) or y < 0:
            output_im.putpixel((x+center[0], -y+center[1]), 255)
    if scenario == 3:
        if y <= 0 and (x >= x_begin or x <= x_end) or y > 0:
            output_im.putpixel((x+center[0], -y+center[1]), 255)
    if scenario == 4:
        if y >= 0 and x <= x_begin or y < 0 and x <= x_end:
            output_im.putpixel((x+center[0], -y+center[1]), 255)
    if scenario == 5:
        if y <= 0 and x >= x_begin or y > 0 and x >= x_end:
            output_im.putpixel((x+center[0], -y+center[1]), 255)



def draw_arc(radius, alpha, beta):
    alpha = alpha % 360
    beta = beta % 360

    r_square = radius**2
    x_begin = radius*math.cos(alpha*math.pi/180)
    y_begin = radius*math.sin(alpha*math.pi/180)


    x_end = radius*math.cos(beta*math.pi/180)
    y_end = radius*math.sin(beta*math.pi/180)


    if (alpha-180)*(beta-180) >= 0:
        if beta > alpha:
            if alpha < 180:
                scenario = 1
            else:
                scenario = 11
        else:
            if alpha < 180:
                scenario = 2
            else:
                scenario = 3
    else:
        if alpha < 180:
            scenario = 4
        else:
            scenario = 5


    x = radius
    y = 0
    add_pixel_if(x, y, scenario, x_begin, x_end)
    add_pixel_if(-y, x, scenario, x_begin, x_end)
    add_pixel_if(-x, -y, scenario, x_begin, x_end)
    add_pixel_if(y, -x, scenario, x_begin, x_end)

    while (y < x):
        if (2*(radius_error(x,y,r_square)+(2*y-1))+1-2*x) > 0:
            x -= 1
        y += 1
        add_pixel_if(x, y, scenario, x_begin, x_end)
        add_pixel_if(y, x, scenario, x_begin, x_end)
        add_pixel_if(-y, x, scenario, x_begin, x_end)
        add_pixel_if(-x, y, scenario, x_begin, x_end)
        
        add_pixel_if(-x, -y, scenario, x_begin, x_end)
        add_pixel_if(-y, -x, scenario, x_begin, x_end)
        add_pixel_if(y, -x, scenario, x_begin, x_end)
        add_pixel_if(x, -y, scenario, x_begin, x_end)



def draw_circle(radius, r_square):  
    r_square = radius**2
    x = radius
    y = 0
    add_pixel(x, y, center)
    add_pixel(-y, x, center)
    add_pixel(-x, -y, center)
    add_pixel(y, -x, center)

    while (y < x):
        if (2*(radius_error(x,y,r_square)+(2*y-1))+1-2*x) > 0:
            x -= 1
        y += 1
        add_pixel(x, y, center)
        add_pixel(y, x, center)
        add_pixel(-y, x, center)
        add_pixel(-x, y, center)
        
        add_pixel(-x, -y, center)
        add_pixel(-y, -x, center)
        add_pixel(y, -x, center)
        add_pixel(x, -y, center)



def add_pixel_band_if(x, y, length, scenario, x_begin, x_end):
    if length > 0: # Horizontal
        if x > 0:
            for d in range(length+1):
                add_pixel_if(x-d, y, scenario, x_begin, x_end)
        else:
            for d in range(length+1):
                add_pixel_if(x+d, y, scenario, x_begin, x_end)
    else: # Vertical
        if y > 0:
            for d in range(-length+1):
                add_pixel_if(x, y-d, scenario, x_begin, x_end)
        else:
            for d in range(-length+1):
                add_pixel_if(x, y+d, scenario, x_begin, x_end)



def draw_arc_filled(radius, alpha, beta, radius_inner):
    alpha = alpha % 360
    beta = beta % 360

    r_square = radius**2
    r_inner_square = radius_inner**2
    x_begin = radius*math.cos(alpha*math.pi/180)
    y_begin = radius*math.sin(alpha*math.pi/180)


    x_end = radius*math.cos(beta*math.pi/180)
    y_end = radius*math.sin(beta*math.pi/180)


    if (alpha-180)*(beta-180) >= 0:
        if beta > alpha:
            if alpha < 180:
                scenario = 1
            else:
                scenario = 11
        else:
            if alpha < 180:
                scenario = 2
            else:
                scenario = 3
    else:
        if alpha < 180:
            scenario = 4
        else:
            scenario = 5


    x = radius
    y = 0
    x_inner = radius_inner
    add_pixel_band_if(x, y, radius-radius_inner, scenario, x_begin, x_end)
    add_pixel_band_if(-y, x, radius_inner-radius, scenario, x_begin, x_end)
    add_pixel_band_if(-x, -y, radius-radius_inner, scenario, x_begin, x_end)
    add_pixel_band_if(y, -x, radius_inner-radius, scenario, x_begin, x_end)
    
    while (y < x):
        if (2*(radius_error(x,y,r_square)+(2*y-1))+1-2*x) > 0:
            x -= 1
        if y < x_inner:
            if (2*(radius_error(x_inner,y,r_inner_square)+(2*y-1))+1-2*x_inner) > 0:
                x_inner -= 1
        else:
            x_inner = y
        y += 1
        add_pixel_band_if(x, y, x-x_inner, scenario, x_begin, x_end)
        add_pixel_band_if(y, x, x_inner-x, scenario, x_begin, x_end)
        add_pixel_band_if(-y, x, x_inner-x, scenario, x_begin, x_end)
        add_pixel_band_if(-x, y, x-x_inner, scenario, x_begin, x_end)
        
        add_pixel_band_if(-x, -y, x-x_inner, scenario, x_begin, x_end)
        add_pixel_band_if(-y, -x, x_inner-x, scenario, x_begin, x_end)
        add_pixel_band_if(y, -x, x_inner-x, scenario, x_begin, x_end)
        add_pixel_band_if(x, -y, x-x_inner, scenario, x_begin, x_end)

        print('{}; {}'.format(x,y))
        print('{} ({})'.format(x-x_inner, x_inner))

def show():
    imshow(np.asarray(output_im))




logging.basicConfig(level=logging.INFO)


radius = 50
w = 4*radius+10
h = 4*radius+10
center = (radius+5, radius+5)
sqrt_2 = math.sqrt(2)



output_im = Image.new('L', (2*center[0],2*center[0]))

draw_arc_filled(radius, -30, 210, radius-5)
show()

