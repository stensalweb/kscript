#!/usr/bin/env ks
""" image.ks - basic image examples



"""

import nx
import mm

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

for i in range(x.shape[-1]), ir[(10 * i) % ir.shape[0], (10 * i) % ir.shape[1], i] = 1

# compute FFT(x)
Fx = nx.fft.fftN(axes, x)

# compute FFT(ir)
Fir = nx.fft.fftN(axes, ir)

for i in range(Fir.shape[0]), for j in range(Fir.shape[1]) {
    Fir[i, j, :] = Fir[i, j, :] / (1 + 100 * (float(i + j) / (Fir.shape[0] + Fir.shape[1])) ** 1.3)
}

# now, we want to have the FFT(y)
Fy = Fx * Fir

# actually get 'y' by inversing the FFT
y = nx.fft.ifftN(axes, Fy)

# write out 'y'
mm.write_image("./out.png", 255 * (y))



