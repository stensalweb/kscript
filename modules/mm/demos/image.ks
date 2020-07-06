#!/usr/bin/env ks
""" image.ks - basic image examples



"""

import nx
import mm

img = mm.read_image("/home/cade/projects/kscript/modules/mm/assets/img1.png") / 255
print (img)

f_img = nx.fft.fftNd(2, img, none, (-2, -1))
print (f_img)



t = img[0]
print (t)

print (nx.fft.fftNd(2, t, none, (-2, -1)))



#if_img = nx.fft.ifftNd(3, f_img, none, (0, 1, 2))
#print (if_img)


#print (img - if_img)


