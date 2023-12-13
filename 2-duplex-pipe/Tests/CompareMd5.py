line1 = input().split()
line2 = input().split()

if (line1[0] == line2[0]):
  print(line1[1] + " OK")
else:
  print(line1[1] + " Failed")