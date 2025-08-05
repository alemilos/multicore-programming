from random import randint

""" with open("input.txt", "w") as writer:
    values = ""
    for i in range(10000):
        values += f' {randint(0,100000)}'
    writer.write(values)
     """
        

with open("input.txt", "w") as f:
    f.write(" ".join(str(x) for x in range(10000, 0, -1)))
