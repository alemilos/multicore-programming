from random import randint

""" with open("input.txt", "w") as writer:
    values = ""
    for i in range(10000):
        values += f' {randint(0,100000)}'
    writer.write(values)
     """
        

with open("input.txt", "w") as f:
    f.write(" ".join(str(randint(0,1000)) for _ in range(100000)))
