#!/bin/bash

echo "tempo mp-mpi:"
#rodar o MPI - MP

for j in {3..16}; do
	#GRAY SCALE MP-MPI --
	echo $j
	echo "gray scale:"
	echo "img pequena:"
	for i in $(seq 10); do
		mpirun --hostfile machines.txt -np "$j" SmoothMPI-MP ESP_026359_1990_RGB.NOMAP_pequena.ppm 0 saida.ppm -O0 -g -Wall
	done

	echo "img media:"
	for i in $(seq 10); do
		mpirun --hostfile machines.txt -np "$j" SmoothMPI-MP ESP_039066_0855_RGB.NOMAP_media.ppm 0 saida.ppm -O0 -g -Wall
	done

	echo "img grande:"
	for i in $(seq 10); do
		mpirun --hostfile machines.txt -np "$j" SmoothMPI-MP ESP_042642_2635_RGB.NOMAP_grande.ppm 0 saida.ppm -O0 -g -Wall
	done

	echo "img gigante:"
	for i in $(seq 10); do
		mpirun --hostfile machines.txt -np "$j" SmoothMPI-MP ESP_042014_1760_IRB.NOMAP_gigante.ppm 0 saida.ppm -O0 -g -Wall-
	done


	#RGB SEQUENCIAl MP-MPI--
	echo "rgb scale:"

	echo "img pequena:"
	for i in $(seq 10); do
		mpirun --hostfile machines.txt -np "$j" SmoothMPI-MP ESP_026359_1990_RGB.NOMAP_pequena.ppm 1 saida.ppm -O0 -g -Wall
	done

	echo "img media:"
	for i in $(seq 10); do
		mpirun --hostfile machines.txt -np "$j" SmoothMPI-MP ESP_039066_0855_RGB.NOMAP_media.ppm 1 saida.ppm -O0 -g -Wall
	done

	echo "img grande:"
	for i in $(seq 10); do
		mpirun --hostfile machines.txt -np "$j" SmoothMPI-MP ESP_042642_2635_RGB.NOMAP_grande.ppm 1 saida.ppm -O0 -g -Wall
	done

	echo "img gigante:"
	for i in $(seq 10); do
		mpirun --hostfile machines.txt -np "$j" SmoothMPI-MP ESP_042014_1760_IRB.NOMAP_gigante.ppm 1 saida.ppm -O0 -g -Wall-
	done
done
