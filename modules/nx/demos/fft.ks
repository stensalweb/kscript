#!/usr/bin/env ks
""" fft.ks - simple example showing FFT usage


@author: Cade Brown <brown.cade@gmail.com>

"""

import nx

x = nx.array([5, 2, 3, 8])
print (x)

Fx = nx.fft.fftN(1, x, none, (0,))
print (Fx)

xp = nx.fft.ifftN(1, Fx, none, (0,))
print (xp)

