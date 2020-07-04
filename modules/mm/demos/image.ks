#!/usr/bin/env ks
""" image.ks - basic image examples



"""

import nx
import mm

img = mm.read_image("/home/cade/projects/kscript/modules/mm/assets/img1.png") / 255

img_r = img[:, :, 0]

print (img_r)

print (nx.fft.fft2d(img_r))

print (img_r[0])


