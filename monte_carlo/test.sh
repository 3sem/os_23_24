touch data.txt

SUM=0

for i in $(seq 1 1000); 
do
    /bin/echo -n "$i; " >> data.txt
    ./monte_carlo $i >> data.txt 

    for j in $(seq 1 2); 
    do
        ./monte_carlo $i >> data.txt 
    done
    echo >> data.txt
done
