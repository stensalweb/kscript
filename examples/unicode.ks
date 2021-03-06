#!/usr/bin/env ks

# Differentiation function
func Δ(f) {
    eps = 0.00001
    func Δf(x) {
        ret (f(x + eps) - f(x)) / eps
    }

    ret Δf
}


# Exponentiation function
# x^2
# 2 * x
func sqr(x) {
    ret x ** 2.0
}

# compute derivative
deriv = Δ(sqr)

print (sqr(3), deriv(3))
