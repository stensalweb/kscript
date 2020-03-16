
N = 10


func f(x) {
    y = 0
    while y < 10 {
        # pretend like we're doing work
        sleep(.1)
        print (x)
        y = y + 1
    }

    ret none
}




ts = []

i = 0
while i < N {
    t = thread('test' + str(i), f, (i,))
    t.start()
    ts.push(t)

    i = i + 1
}

