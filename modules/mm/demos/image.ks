#!/usr/bin/env ks
""" image.ks - basic image examples



"""

import nx
import mm

print (nx)

img = mm.read_image("/home/cade/projects/kscript/modules/mm/assets/img1.png") / 255

img_r = img[:, :, 0]
print (img_r)

fft_r = nx.fft.fft2d(img_r)

print (fft_r)

ifft_r = nx.fft.ifft2d(fft_r)
print (ifft_r)






