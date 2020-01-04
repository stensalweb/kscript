#!ks

func is_prime(x) {
    if x <= 2 then ret x == 2
    if x % 2 == 0 then ret false

    i = 3
    while i * i <= x {
        if x % i == 0 then ret false
        i = i + 2
    }

    ret true
}

print (is_prime(100000000000031))

