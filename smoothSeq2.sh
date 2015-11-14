#!/bin/bash

#rodar o sequencial
echo "tempo sequencia:"

#GRAY SCALE SEQUENCIAl --
echo "gray scale:"

echo "img pequena:"
for i in $(seq 10); do
	./SmoothSeq pequena.pgm 0 saida.pgm -O0 -g -Wall
done

echo "img media:"
for i in $(seq 10); do
	./SmoothSeq media.pgm 0 saida.pgm -O0 -g -Wall
done

echo "img grande:"
for i in $(seq 10); do
	./SmoothSeq grande.pgm 0 saida.pgm -O0 -g -Wall
done

echo "img gigante:"
for i in $(seq 10); do
	./SmoothSeq gigante.pgm 0 saida.pgm -O0 -g -Wall
done
#RGB SEQUENCIAl --
echo "rgb scale:"
echo "img pequena:"
for i in $(seq 10); do
	./SmoothSeq pequena.ppm 1 saida.ppm -O0 -g -Wall
done

echo "img media:"
for i in $(seq 10); do
	./SmoothSeq media.ppm 1 saida.ppm -O0 -g -Wall
done

echo "img grande:"
for i in $(seq 10); do
	./SmoothSeq grande.ppm 1 saida.ppm -O0 -g -Wall
done

echo "img gigante:"
for i in $(seq 10); do
	./SmoothSeq gigante.ppm 1 saida.ppm -O0 -g -Wall
done
