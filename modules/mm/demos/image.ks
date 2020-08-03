#!/usr/bin/env ks
""" image.ks - basic image examples



"""

import nx
import mm

# read in an image
x = mm.read_image("./modules/mm/assets/img/cade_brown_icl.jpg")

# sub sampling
subN = 1
x = x[::subN, ::subN]

# reshape to be an even size, and with 3 channels
if x.shape[0] % 2, x = x[:-1, :]
if x.shape[1] % 2, x = x[:, :-1]
if x.shape[2] > 3, x = x[:, :, :3]

# axes for image indices
axes = (0, 1)

# compute impulse response (IR)
ir = nx.zeros(x.shape)

# x, y shift
xys = [3, 5]

# shift all the channels off by given amounts, like a VHS shift
for i in range(x.shape[-1]) {
    ir[(xys[0] * i) % ir.shape[0], (xys[1] * i) % ir.shape[1], i] = 1
}

# compute FFT(x)
Fx = nx.fft.fftN(x, axes)

# compute FFT(ir)
Fir = nx.fft.fftN(ir, axes)

# now, we want to have the FFT(y)
Fy = Fx * Fir

# actually get 'y' by inversing the FFT
y = nx.fft.ifftN(Fy, axes)

# convert to absolute value
y = nx.abs(y)

# write out 'y'
mm.write_image("./out.png", y)


