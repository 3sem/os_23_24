
MINSIZE = 2097152 # getconf ARG_MAX

data = ""

with open("Lorem.txt", "r") as f:
  data = f.read()

  repeats = (MINSIZE // len(data))

  data += data * repeats

with open("BigLorem.txt", "w") as f:
  f.write(data)