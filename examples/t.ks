

func f(x) {
    y = 0
    while y < 100 {
        print (x)
        y = y + 1
    }

    ret none

    func h(a) {
        ret x ** a
    }

    print (thread.__this__(), h(3))

    ret none
}




ts = []

i = 0
while i < 20 {
    t = thread('test' + str(i), f, (i,))
    t.start()
    ts.push(t)

    i = i + 1
}

