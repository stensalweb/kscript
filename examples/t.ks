
# numerics library
import nx



x = nx.array([
    [1, 2],
    [3, 4]
])

#x = nx.array([1, 2])
#x = nx.array([1, 2, 3, 4])


print (x, z = nx.add(x, x))
print (z[-1, -1])


exit()


x = nx.array([[1, 2], [3, 4]])

print (x, z = nx.add(x, x))
print (z[0, 1])


exit()

x = nx.array([
    [1, 2],
    [3, 4]
])

print (nx.add(x, x)[1, 0])

