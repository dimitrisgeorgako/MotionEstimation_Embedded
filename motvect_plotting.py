import numpy as np
import os, argparse
import matplotlib.pyplot as plt
from scipy.ndimage import rotate
from matplotlib.patches import Rectangle

# Parse block size as argument 
parser = argparse.ArgumentParser(description='This .py script plots motion_vectors')
parser.add_argument('arg1', type=int, help='Requires latest block size working on...')
    
args = parser.parse_args()
B = args.arg1
block_size = (B, B)

# Read the text file
filename = 'mot_vect.txt'
MotVect = np.loadtxt(filename, dtype=int, delimiter=' ')

mvx = []
mvy = []
for i in range(int(len(MotVect)/2)):
    mvx.append(MotVect[i])
for i in range(int(len(MotVect)/2), len(MotVect)):
    mvy.append(MotVect[i])

mvx = np.array(mvx)
mvy = np.array(mvy)

print('Motion vector shapes are:', mvx.shape, mvy.shape)

# Display the original and rotated images
fig, ax = plt.subplots(1, 2, figsize=(8, 4))

image = plt.imread(os.path.join(r'./qcif_frames', 'gray_qcif_image1.png'))
ax[0].imshow(image, cmap='gray')
ax[0].set_title('Previous Motion Frame')

for i in range(0, image.shape[0], block_size[0]):
    for j in range(0, image.shape[1], block_size[1]):
        rect = Rectangle((j, i), block_size[1], block_size[0], linewidth=1, edgecolor='r', facecolor='none')
        ax[0].add_patch(rect)

image = plt.imread(os.path.join(r'./qcif_frames', 'gray_qcif_image2.png'))

ax[1].imshow(image, cmap='gray')
ax[1].set_title('Current Motion Frame')

for i in range(0, image.shape[0], block_size[0]):
    for j in range(0, image.shape[1], block_size[1]):
        rect = Rectangle((j, i), block_size[1], block_size[0], linewidth=1, edgecolor='r', facecolor='none')
        ax[1].add_patch(rect)

block_x = []
block_y = []
for i in range(int(block_size[0]/2), (len(mvx)+1)*block_size[0], block_size[0]):
    for j in range(int(block_size[1]/2), (len(mvx[0])+1)*block_size[1], block_size[1]):
        block_x.append(i)
        block_y.append(j)

block_x_resized = []
for item in block_x:
    if item not in block_x_resized:
        block_x_resized.append(item)

block_y_resized = []
for item in block_y:
    if item not in block_y_resized:
        block_y_resized.append(item)

block_x_resized = block_x_resized[:-1]
block_y_resized = block_y_resized[:-1]

block_x = []
for i in range(len(block_y_resized)):
    block_x.append(block_x_resized)

block_y = []
for j in range(len(block_y_resized)):
    for i in range(len(block_x_resized)):
        block_y.append(block_y_resized[j])

if(B<=8):
    mvx=mvx/2.5
    mvy=mvy/2.5

ax[0].quiver(
    block_y,            # X-coordinates of arrow start points
    block_x,            # Y-coordinates of arrow start points
    -mvx,               # X-components of arrow vectors
    -mvy,               # Y-components of arrow vectors
    color='red',        # Arrow color
    scale=1,            # Scaling factor for arrow length
    scale_units='xy',   # Use the same scale for both x and y directions
    angles='xy',        # Use the same angle convention for both x and y directions
    width=0.004,     # Width of the arrows
)

plt.show()