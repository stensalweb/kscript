#!/usr/bin/env ks
""" fft.ks - simple example showing FFT usage


@author: Cade Brown <brown.cade@gmail.com>

"""

import nx

x = nx.array([5, 2, 3, 8])
print (x)

Fx = nx.fft.fftN(x, (0,))
print (Fx)

xp = nx.fft.ifftN(Fx, (0,))
print (xp)

