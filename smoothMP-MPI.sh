#!/bin/bash

echo "tempo mp-mpi:"
#rodar o MPI - MP

for j in {3..}; do
	#GRAY SCALE MP-MPI --
	echo $j
	echo "gray scale:"
	echo "img pequena:"
	for i in $(seq 10); do
		mpirun -np "$j" SmoothMPI-MP Lenna.png 0 Lenna2.png
	done
	echo "img media:"
	for i in $(seq 10); do
		mpirun -np "$j" SmoothMPI-MP Lenna.png 0 Lenna2.png
	done

	echo "img grande:"
	for i in $(seq 10); do
		mpirun -np "$j" SmoothMPI-MP Lenna.png 0 Lenna2.png
	done

	echo "img gigante:"
	for i in $(seq 10); do
		mpirun -np "$j" SmoothMPI-MP Lenna.png 0 Lenna2.png
	done


	#RGB SEQUENCIAl MP-MPI--
	echo "rgb scale:"

	echo "img pequena:"
	for i in $(seq 10); do
		mpirun -np "$j" SmoothMPI-MP Lenna.png 1 Lenna2.png
	done

	echo "img media:"
	for i in $(seq 10); do
		mpirun -np "$j" SmoothMPI-MP Lenna.png 1 Lenna2.png
	done

	echo "img grande:"
	for i in $(seq 10); do
		mpirun -np "$j" SmoothMPI-MP Lenna.png 1 Lenna2.png
	done

	echo "img gigante:"
	for i in $(seq 10); do
		mpirun -np "$j" SmoothMPI-MP Lenna.png 1 Lenna2.png
	done
done
