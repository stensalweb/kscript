#!ks

res = "sf"
i = 0

y = []

while i < 10000 {
    y = y + [i]
    #y += [i]
    #y.append(i)
    i = i + 1
}

#print (y)

z = repr(y)

#print (z)

i = 1
while i * i < 3 {
    print (i = i + 2)
}
