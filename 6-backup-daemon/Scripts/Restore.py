import os
import sys

def restore(history_path, line, dst_path, stage_id):
  _, ftype, stype, path, id = line.split()
  print(line)

  if (ftype == "DIR"):
    if (stype == "CREATE"):
      os.mkdir(dst_path + path)
    elif (stype == "DELETE"):
      os.rmdir(dst_path + path)
    else: 
      print(f"Wrong type for dir: {stype}")
      quit()

  if (ftype == "REG"):
    if (stype == "CREATE"):
      os.system(f"cp {history_path + id} {dst_path + path}")
    elif (stype == "DELETE"):
      os.system(f"rm {dst_path + path}")
    elif (stype == "MODIFY"):
      os.system(f"patch {dst_path + path} {history_path + id}")
    else:
      print(f"Wrong type for file: {stype}")

  return id

def main():
  if (len(sys.argv) != 4):
    print(f"Usage: BACKUP_PATH/ DST_PATH/ STAGE_ID")
    quit()

  HISTORY_PATH = sys.argv[1]
  dst_path = sys.argv[2]
  stage_id = sys.argv[3]

  stages_path = HISTORY_PATH + "STAGES"

  with open(stages_path, "r") as f:
    for line in f.readlines():
      id = restore(HISTORY_PATH, line, dst_path, stage_id)
      if id == stage_id: break


if __name__ == "__main__":
  main()