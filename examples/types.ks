#!ks

# basic types, int, str, etc
x = 3
y = 4

print (x, y, x+y, x*y)

# tuples are immutable collections of objects, whereas lists are mutable

xy = (x, y)
print (xy)
xy = [x, y]
print (xy, xy[0], xy[1])

xy[1] = y + 12394823984
print (xy, xy[0], xy[1])

# -1 is out of range, though
#print (xy[0-1])

# dictionaries pair any object -> another object

#x = dict()

#x["asdf"] = 55 + 33
#x["another"] = 28379432
#x[x["asdf"]] = 234

#print (x, x["asdf"])

# if you uncomment the following line, you will see a KeyError pop up
#print (x["non-existant"])

# you can get the type of an object by using the `type` function, and then output the name with its attr `__name__`
#print (type(x))
#print (type(x).__name__)




# -*- custom types -*-


# here's a custom type

type MyType {

    T = [1, 2, 3]

    func __init__(self, x, y) {
        self.x = x
        self.y = (1, y, y+x)
    }

    func get(self, z) {
        ret self.x + z
    }

    func __str__(self) {
        try ret "Val<" + self.x + ", " + self.y + ">", catch, ret "ERR"
    }
}

# construct it
m = MyType(2, 3)

print (m)

print (MyType.T, type(m).T)

print (m.get(5))

