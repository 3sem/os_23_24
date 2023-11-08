cmake_minimum_required(VERSION 3.10)
 
project(comparison)
 
set(FIFO fifo/fifo_main.c fifo/fifo.c)
 
add_executable(fifo.out ${FIFO})
