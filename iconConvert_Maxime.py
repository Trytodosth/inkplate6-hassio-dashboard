# -----------
# Simple python script to
# create icon header files for Inkplate 6
# Arduino sketches
#
# Takes all files from /icons and saves them to /binary_icons
#
# -----------

from PIL import Image
from pathlib import Path
import os, logging
import PIL

from PIL.Image import new

# Parameters
output_3bit = True
invert_colors = True
resize_w = 50 # Desired Width. Set <0 to desactivate the resizing
channel_to_consider = 'G'
consider_alpha = True

include_text = ''

if output_3bit:
    color_range = 256 / pow(2, 3)
    backward_conversion = 255 / (pow(2, 3) - 1)
else:
    color_range = 256 / 2
    backward_conversion = 255

logging.basicConfig(level=logging.INFO)

input_dir = os.path.join(os.getcwd(), 'icons_to_binary')
output_dir = os.path.join(os.getcwd(), 'binary_icons')

if not os.path.isdir(output_dir):
    logging.info('Output dir missing, will be created as : ' + output_dir)
    os.mkdir(output_dir)

count = 0
logging.info('Looping through input pictures in ' + input_dir)
for file in os.listdir(input_dir):
    count += 1
    logging.info('#{:d}: {}:'.format(count, file))
    try:
        raw_im = Image.open(os.path.join(input_dir, file))
        logging.debug('Input mode:' + raw_im.mode)
        
        w, h = raw_im.size
        logging.info('size: {:d}x{:d} px'.format(w, h))
        
        #new_im = raw_im.convert('LA') # Grayscale with A channel
        new_im = raw_im.getchannel(channel_to_consider)
        new_im_A = raw_im.getchannel('A') # for Alpha consideration

        # Resize if requested
        if resize_w > 0:
            h = round(resize_w/w*h)
            new_im = new_im.resize((resize_w, h), PIL.Image.LANCZOS)
            new_im_A = new_im_A.resize((resize_w, h), PIL.Image.LANCZOS)
            w, h = new_im.size
            logging.info('> resized: {:d}x{:d} px'.format(w, h))


        # Outputs
        output_px = [0 for x in range(w * h)]
        output_im = Image.new('L', (w,h))

        pixels = []
        # Extracting picture information
        for x in range(w):
            for y in range(h):
                col = new_im.getpixel((x, y))
                pixels.append(col)

                if consider_alpha:
                    col = int((col * new_im_A.getpixel((x, y))) // 255)
                if invert_colors:
                    col = 255 - col

                #print('{:d}x{:d}: {:d}'.format(x, y, col))
                #logging.debug('{:d}x{:d}: {:d}'.format(x, y, pix))
                
                output_im.putpixel((x,y), int(col // color_range * backward_conversion))
                #output_px[x + w * y] = int(col // color_range * backward_conversion)
                output_px[x + w * y] = int(col)

        # For debug purpose
        raw_im
        new_im
        output_im

        # Output name for the file and function
        function_name = ('icon_' + Path(file).stem).replace('-','_') # Special characters to be removed before, except '-' handled here
        if resize_w > 0: function_name += '_{:d}px'.format(resize_w) # if resized, added ot the name

        # Go get'em Binary!
        output_binary = [0 for x in range(len(output_px)//2)]
        binary_s = ''
        for y in range(h):
            for x in range(0,w,2):
                binary_s += str(hex((output_px[x + w * y] & 240) | ((output_px[x + w * y +1] >> 4) & 15))) + ','
            if w % 2 != 0:
                binary_s += str(hex((output_px[w + w * y] & 240))) + ','
            binary_s += '\n'
        binary_s += '};' + '\nint {}_w = {:d};\nint {}_h = {:d};'.format(function_name, w, function_name, h)

        # Output to the file
        with open(os.path.join(output_dir, function_name + '.h'), 'w') as f:
            print('//{}: {:d}x{:d} image, {}'.format(file, w, h, '3bits' if output_3bit else '1bit'), file=f)
            print('const uint8_t ' + function_name + '[] PROGMEM = {', file=f)
            print(binary_s, file=f)

        include_text += '#include "{}.h"\n'.format(function_name)

    except IOError:
        logging.error('Failed opening {}'.format(file))


with open(os.path.join(output_dir, 'include.txt'), 'w') as f:
    print(include_text, file=f)

logging.info('Program terminated with success. Include headers in "include.txt"')