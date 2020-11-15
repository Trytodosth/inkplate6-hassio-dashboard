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
TESTING = False # Testing mode: only the 1st image processed. Shows the expected results. Nice to check threshold and other parameters

output_3bit = False
threshold_1bit = 128 # Only required if 1bit mode
invert_colors = False # based on RGB, white in the picture results into black on the plate.
resize_w = -1 # Desired Width. Set <0 to desactivate the resizing
channel_to_consider = 'G' # R: Red; G: Green; B: Blue; L: Grayscale
consider_alpha = True

include_text = ''

if output_3bit:
    color_range = 256 / pow(2, 3)
    backward_conversion = 255 / (pow(2, 3) - 1)
else:
    color_range = 256 / 2
    backward_conversion = 255

logging.basicConfig(level=logging.INFO)

input_dir = os.path.join(os.getcwd(), 'Images_to_convert')
output_dir = os.path.join(os.getcwd(), 'Binary_images')

if not os.path.isdir(output_dir):
    logging.info('Input directory missing, will be created as : ' + input_dir)
    os.mkdir(input_dir)
    exit
if not os.path.isdir(output_dir):
    logging.info('Output directory missing, will be created as : ' + output_dir)
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

        # Extracting picture information
        for x in range(w):
            for y in range(h):
                col = new_im.getpixel((x, y))

                if consider_alpha:
                    col = int((col * new_im_A.getpixel((x, y))) // 255)
                if invert_colors:
                    col = 255 - col

                #print('{:d}x{:d}: {:d}'.format(x, y, col))
                #logging.debug('{:d}x{:d}: {:d}'.format(x, y, pix))
                
                if output_3bit:
                    output_im.putpixel((x,y), int(col // color_range * backward_conversion))
                else:
                    output_im.putpixel((x,y), 255 if col > threshold_1bit else 0)
                
                output_px[x + w * y] = int(col)

        # For debug purpose
        raw_im
        new_im
        output_im

        # Output name for the file and function
        function_name = (Path(file).stem).replace('-','_') # Special characters to be removed before, except '-' handled here
        if resize_w > 0: function_name += '_{:d}px'.format(resize_w) # if resized, added ot the name

        # Go get'em Binary!
        binary_s = ''
        if output_3bit:
            # Pixel values stored as bytes, 2 pixels per byte: 01011001 (0..255 per 4bits)
            for y in range(h):
                for x in range(0,w,2):
                    # Masks: 240: 11110000 ; 15: 00001111
                    binary_s += str(hex((output_px[x + w * y] & 240) | ((output_px[x + w * y +1] >> 4) & 15))) + ','
                if w % 2 != 0: # Remaining pixels
                    binary_s += str(hex((output_px[w + w * y] & 240))) + ','
                binary_s += '\n'
        else: # 1bit
            # Pixel values stored as bytes, 8 pixels per byte: 01011001 (1 or 0 for each)
            for y in range(h):
                compressed_bit = 0
                for x in range(w):
                    compressed_bit |= (1 if output_px[x + w * y] > threshold_1bit else 0) << (7 - x % 8)
                    if x % 8 == 7: # Byte complete. Write and reset
                        binary_s += str(hex(compressed_bit)) + ','
                        compressed_bit = 0
                if w % 8 != 0: # Remaining pixels
                    binary_s += str(hex(compressed_bit)) + ','
                    compressed_bit = 0
                binary_s += '\n'
            
        binary_s += '};' + '\nint {}_w = {:d};\nint {}_h = {:d};'.format(function_name, w, function_name, h)

        # Output to the file
        with open(os.path.join(output_dir, function_name + '.h'), 'w') as f:
            print('//{}: {:d}x{:d} image, {}'.format(file, w, h, '3bits' if output_3bit else '1bit'), file=f)
            print('const uint8_t ' + function_name + '[] PROGMEM = {', file=f)
            print(binary_s, file=f)

        include_text += '#include "{}.h"\n'.format(function_name)
        logging.info('> exported to : {}'.format(os.path.join(os.path.basename(output_dir), function_name + '.h')))

        if TESTING:
            raw_im.show()
            output_im.show()
            break

    except IOError:
        logging.error('Failed opening {}'.format(file))



with open(os.path.join(output_dir, '_include.txt'), 'w') as f:
    print(include_text, file=f)

logging.info('Program terminated with success. Include headers in "_include.txt"')