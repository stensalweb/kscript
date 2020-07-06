#!/usr/bin/env ks
""" image.ks - basic image examples



"""

import nx
import mm

x = mm.read_image("/home/cade/projects/kscript/modules/mm/assets/img1.png") / 255
print (x)

fx = nx.fft.fftNd(2, x, none, (0, 1))
print (fx)

print (fx[:,0,0])
print (nx.fft.fft1d(fx[:,0,0]))



print (nx.fft.fftNd(3, x, (0, 1, 2)))
