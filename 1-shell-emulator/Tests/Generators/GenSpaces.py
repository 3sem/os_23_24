from random import randint

with open("spaces.txt", "w") as f:
  middle = randint(0, 4096)
  for i in range(middle):
    f.write(" ")
  f.write("Pomidor")
  for i in range(4096 - middle):
    f.write(" ")