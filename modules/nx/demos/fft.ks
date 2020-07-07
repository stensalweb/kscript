#!/usr/bin/env ks
""" fft.ks - simple example showing FFT usage


@author: Cade Brown <brown.cade@gmail.com>

"""

import nx

x = nx.array(range(4))
print (x)

Fx = nx.fft.fftNd(1, x)
print (Fx)

