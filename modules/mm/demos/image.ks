#!/usr/bin/env ks
""" image.ks - basic image examples



"""

import nx
import mm

img = mm.read_image("/home/cade/projects/kscript/modules/mm/assets/img1.png")

print (img)
print (img.shape)



print (nx.fft.fft2d(img))


