
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

cd build
    dd if=/dev/urandom of=input.txt bs=1048576 count=4096
    ./main
    FIRSTSUM=$(md5sum input.txt | awk '{print $1}')
    SECONDSUM=$(md5sum output.txt | awk '{print $1}')
    echo "input file hash = $FIRSTSUM"
    echo "output file hash = $SECONDSUM"

    if [ "$FIRSTSUM" = "$SECONDSUM" ];
    then
        echo "${GREEN} Test passed ${NC}"
    else
        echo "${RED} Test failed ${NC}"
    fi
