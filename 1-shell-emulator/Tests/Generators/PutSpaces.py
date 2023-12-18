from random import randint

MIN_SPACES = 1
MAX_SPACES = 5

output = " " * randint(MIN_SPACES - 1, MAX_SPACES)

with open(input(), "r") as f:
  text = f.read()

  tokens = text.split(" ")

  for token in tokens:
    output += token
    output += " " * randint(MIN_SPACES, MAX_SPACES)

with open("result.txt", "w") as f:
  f.write(output)