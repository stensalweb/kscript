#!ks


func sum(tup) {
    ret tup[0]
}

func f(a, b, c) {
    ret a + b + c
}

func is_prime(x) {
    if x <= 2 then ret x == 2
    if x % 2 == 0 then ret false

    i = 3
    while i * i < x {
        if x % i == 0 then ret false
        i = i + 2
    }

    ret true
}

print (is_prime(11))

x = [1, 2, 3]
print (x[1])





