#!/usr/bin/env ks
""" image.ks - basic image examples



"""

import nx
import mm

img = mm.read_image("/home/cade/projects/kscript/modules/mm/assets/img1.png") / 255

print (img)


f_img = nx.fft.fft2d(img, none, 0, 1)
print (f_img)

if_img = nx.fft.ifft2d(f_img, none, 0, 1)
print (if_img)



