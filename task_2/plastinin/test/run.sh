INPUT_FILE="file.txt"
OUTPUT_FILE="file1.txt"

dd if=/dev/urandom of=$INPUT_FILE bs=1048576 count=4096
time ../build/task_2 $INPUT_FILE $OUTPUT_FILE

MD1=0
MD2=0

md5sum $INPUT_FILE
md5sum $OUTPUT_FILE

rm -rf $INPUT_FILE
rm -rf $OUTPUT_FILE

