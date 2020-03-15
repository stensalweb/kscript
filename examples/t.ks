
print ("Cade Brown")

print ("Hello World")

ts = []

i = 0
while i < 1000 {
    t = thread('test' + i, print, (i, ))
    t.start()
    i = i + 1
}



