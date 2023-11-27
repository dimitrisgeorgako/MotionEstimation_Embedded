import numpy as np
import glob, os, re
import cv2
import matplotlib.pyplot as plt
from scipy.ndimage import rotate
from matplotlib.patches import Rectangle

file_paths = glob.glob(os.path.join(r'./get_frames', 'frame*.png'))

def sort_key(filename): # Sorting file_paths produce from os.path to be contiguous in frames
    match = re.search(r'\d+', filename)
    if match:
        return int(match.group())
    else:
        return float('inf')  # Return a large number for filenames without numeric part

file_paths = sorted(file_paths, key=sort_key)

image_array = []
counter = 1
for file_path in file_paths:
    if os.path.exists(file_path):
        image = cv2.imread(file_path)
        qcif_image = cv2.resize(image, (176, 144))
        #cv2.imwrite(f'./qcif_frames/qcif_image{counter}.png', qcif_image)
        qcif_image = cv2.cvtColor(qcif_image, cv2.COLOR_BGR2GRAY)
        cv2.imwrite(f'./qcif_frames/gray_qcif_image{counter}.png', qcif_image)
        image_array.append(qcif_image)
        counter += 1
    else:
        print('Error in locating files...')

print('Original frame pixel resolution and RGB dimensions:', image.shape)
print('QCIF last frame pixel resolution, colorless(1D):', qcif_image.shape, 'with values:')
print(qcif_image)
print()

print('~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~')
print('Converting frames of .png format file into QCIF arrays 176x144 format...')
print('Finished saving all frames...')
print('')
print('e.g Frame1 pixels:')
print(image_array[1]) 
print('with length:', len(image_array[0]), 'x', len(image_array[0][0]))

print('')
print('Pushing several frames to header file...')

# Save frames being compared
curr = image_array[1]
prev = image_array[0]
header_file = "images.h"

with open(header_file, 'w') as file:
    file.write("#include <stdio.h>\n")
    file.write("#include <stdint.h>\n")
    file.write("#include <stdlib.h>\n\n")

    file.write("/* QCIF formatted frames */\n")
    file.write("const uint8_t curr[144][176] = {\n{") 
    for i in range(len(curr)):
        for j in range(len(curr[0])):
            if(j == len(curr[0])-1):
                file.write(str(curr[i][j]))
                break
            file.write(str(curr[i][j]) + ", ")
        if(i == len(curr)-1):
            break
        file.write("},\n {")
    file.write("}\n};")
    file.write("\n\n")

    file.write("const uint8_t prev[144][176] = {\n{") 
    for i in range(len(prev)):
        for j in range(len(prev[0])):
            if(j == len(prev[0])-1):
                file.write(str(prev[i][j]))
                break
            file.write(str(prev[i][j]) + ", ")
        if(i == len(prev)-1):
            break
        file.write("},\n {")
    file.write("}\n};")
    file.write("\n")


# Plot some QCIF image frames 
for i in [30]:
    fig, ax = plt.subplots(2, 2)

    ax[0,0].imshow(plt.imread(os.path.join(r'./get_frames', f'frame{i}.png')), cmap='gray')
    ax[0,0].set_title('Original previous Image frame')

    ax[0,1].imshow(image_array[i-1], cmap='gray')
    ax[0,1].set_title('QCIF-formatted previous Image frame')

    ax[1,0].imshow(plt.imread(os.path.join(r'./get_frames', f'frame{i+1}.png')), cmap='gray')
    ax[1,0].set_title('Original current Image frame')

    ax[1,1].imshow(image_array[i], cmap='gray')
    ax[1,1].set_title('QCIF-formatted current Image frame')

    plt.show()

# Convert QCIF frames back into video .mp4
frames_dir = './qcif_frames'
output_video = 'qcif_video.mp4'

frame_files = [f for f in os.listdir(frames_dir) if f.endswith('.png')]
frame_files.sort()

first_frame = cv2.imread(os.path.join(frames_dir, frame_files[0]))
height, width, layers = first_frame.shape

fourcc = cv2.VideoWriter_fourcc(*'mp4v')  # You can also use 'XVID'
video = cv2.VideoWriter(output_video, fourcc, 10.0, (width, height))

for frame_file in frame_files:
    frame = cv2.imread(os.path.join(frames_dir, frame_file))
    video.write(frame)

video.release()