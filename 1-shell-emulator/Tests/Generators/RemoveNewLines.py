text = ""
while True:
  filename = input()
  print(f"Processing {filename}")

  with open(filename, "r") as f:
    text = f.read()

  text = text.replace("\n", "")

  with open(filename, "w") as f:
    f.write(text)