#!/bin/bash

#rodar o sequencial
echo "tempo sequencia:"

#GRAY SCALE SEQUENCIAl --
echo "gray scale:"

echo "img pequena:"
for i in $(seq 10); do
	./SmoothSeq Lenna.png 0 Lenna2.png -O0 -g -Wall
done

echo "img media:"
for i in $(seq 10); do
	./SmoothSeq Lenna.png 0 Lenna2.png -O0 -g -Wall
done

echo "img grande:"
for i in $(seq 10); do
	./SmoothSeq Lenna.png 0 Lenna2.png -O0 -g -Wall
done

echo "img gigante:"
for i in $(seq 10); do
	./SmoothSeq Lenna.png 0 Lenna2.png -O0 -g -Wall
done
#RGB SEQUENCIAl --
echo "rgb scale:"
echo "img pequena:"
for i in $(seq 10); do
	./SmoothSeq Lenna.png 1 Lenna2.png -O0 -g -Wall
done

echo "img media:"
for i in $(seq 10); do
	./SmoothSeq Lenna.png 1 Lenna2.png -O0 -g -Wall
done

echo "img grande:"
for i in $(seq 10); do
	./SmoothSeq Lenna.png 1 Lenna2.png -O0 -g -Wall
done

echo "img gigante:"
for i in $(seq 10); do
	./SmoothSeq Lenna.png 1 Lenna2.png -O0 -g -Wall
done








