#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <sys/time.h>

#include <iostream>
#include <vector>
#include <cmath>
#include <mpi.h>
/*
	mpic++ -o SmoothMPI SmoothMPI.cpp `pkg-config --cflags --libs opencv`
	mpirun -np 4 SmoothMPI image_in type_img image_out
*/

using namespace cv;
using namespace std;
int border = 2;
Mat *aplica_smooth_grayscale(Mat& in) {


	Mat *out = new Mat(in.size().height - 4, in.size().width - 4, CV_8U, 1);
	//replica a borda pra solucionar o problema dos pixels de borda
	//achar a média aritmética e depois atualizar o pixel
	float average;

	for(int i = border; i < in.size().height - border; i++) {
		for(int j = border; j < in.size().width - border; j++) {
			//nao sei se tenho que somar como uchar ou float
			average = in.at<uchar>(i, j) 
			+ in.at<uchar>(i + 1, j) 
			+ in.at<uchar>(i - 1, j) 
			+ in.at<uchar>(i, j + 1) 
			+ in.at<uchar>(i, j - 1) 
			+ in.at<uchar>(i + 2, j)
			+ in.at<uchar>(i - 2, j)
			+ in.at<uchar>(i, j + 2)
			+ in.at<uchar>(i, j - 2)
			+ in.at<uchar>(i + 1, j + 1) 
			+ in.at<uchar>(i + 1, j - 1) 
			+ in.at<uchar>(i + 1, j - 2) 
			+ in.at<uchar>(i + 1, j + 2)
			+ in.at<uchar>(i - 1, j + 1)
			+ in.at<uchar>(i - 1, j - 1)
			+ in.at<uchar>(i - 1, j + 2)
			+ in.at<uchar>(i - 1, j - 2)
			+ in.at<uchar>(i + 2, j + 1)
			+ in.at<uchar>(i + 2, j - 1)
			+ in.at<uchar>(i + 2, j + 2)
			+ in.at<uchar>(i + 2, j - 2)
			+ in.at<uchar>(i - 2, j + 1)
			+ in.at<uchar>(i - 2, j - 1)
			+ in.at<uchar>(i - 2, j + 2)
			+ in.at<uchar>(i - 2, j - 2);
			average = average/25;
			out->at<uchar>(i - border, j - border) = (uchar)average;
		}
	}
	return out;
}


Mat *aplica_smooth_color(Mat& in) {

	//copia imagem de entrada 
	Mat *out = new Mat(in.size().height - 4,in.size().width - 4, CV_8UC3, 1);
	//Mat aux(in->size(), CV_8UC3, 1);

	//replica a borda pra solucionar o problema dos pixels de borda

	//achar a média aritmética e depois atualizar o pixel
	float average;

	for(int i = border; i < in.size().height - border; i++) {
		for(int j = border; j < in.size().width - border; j++) {
			//nao sei se tenho que somar como uchar ou float]
			for(int color = 0; color < 3; color++) {
				average = in.at<cv::Vec3b>(i, j)[color] 
				+ in.at<cv::Vec3b>(i + 1, j)[color] 
				+ in.at<cv::Vec3b>(i - 1, j)[color]
				+ in.at<cv::Vec3b>(i, j + 1)[color] 
				+ in.at<cv::Vec3b>(i, j - 1)[color]
				+ in.at<cv::Vec3b>(i - 2, j)[color] 
				+ in.at<cv::Vec3b>(i + 2, j)[color] 
				+ in.at<cv::Vec3b>(i, j + 2)[color]
				+ in.at<cv::Vec3b>(i, j - 2)[color]
				+ in.at<cv::Vec3b>(i + 1, j + 1)[color]
				+ in.at<cv::Vec3b>(i + 1, j + 2)[color] 
				+ in.at<cv::Vec3b>(i + 1, j - 1)[color] 
				+ in.at<cv::Vec3b>(i + 1, j - 2)[color]
				+ in.at<cv::Vec3b>(i - 1, j - 1)[color] 
				+ in.at<cv::Vec3b>(i - 1, j + 1)[color]
				+ in.at<cv::Vec3b>(i - 1, j + 2)[color]
				+ in.at<cv::Vec3b>(i - 1, j - 2)[color] 
				+ in.at<cv::Vec3b>(i - 2, j - 2)[color] 
				+ in.at<cv::Vec3b>(i - 2, j + 1)[color]
				+ in.at<cv::Vec3b>(i - 2, j - 1)[color]
				+ in.at<cv::Vec3b>(i - 2, j + 2)[color]
				+ in.at<cv::Vec3b>(i + 2, j + 2)[color]
				+ in.at<cv::Vec3b>(i + 2, j + 1)[color] 
				+ in.at<cv::Vec3b>(i + 2, j - 1)[color]
				+ in.at<cv::Vec3b>(i + 2, j - 2)[color];

				average = average/25;
				out->at<cv::Vec3b>(i - border, j - border)[color] = (uchar)average;
			}

		}
	}
	return out;
}
int main(int argc, char *argv[]) {
	int tipo_img = atoi(argv[2]);
	int NUM_NODES;
	MPI_Init(&argc, &argv);
	int rank;
    MPI_Status status;
    //descobre o rank do nó
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	//descobre quantos nós a maquina suporta
    MPI_Comm_size(MPI_COMM_WORLD, &NUM_NODES);
    Mat in, crop, out;

    if(rank == 0)  {
    	//le a imagem  e salva suas dimensões para um futuro uso
    	if(tipo_img == 0) {
	    	in = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
		} else if(tipo_img == 1) {
			in = imread(argv[1], CV_LOAD_IMAGE_COLOR);
		}
		//pegar o tempo de inicio
		struct timeval inicio, fim;
    	gettimeofday(&inicio,0);

	    //calcula o tamanho de cada pedaço
		int divisao_h = in.size().height/(NUM_NODES - 1);
		int divisao_w = in.size().width;
		int resto_divisao = in.size().height%(NUM_NODES - 1);
		//variavel auxiliar pra receber os dimensões de cada pedaço e manda pra cada nó
		int dimensoes[3];
		for(int i = 1; i < NUM_NODES; i++) {

			//recorto um pedaço da imagem pra mandar para o nó i
			//if else é pra pegar o restinho da imagem que sobre quando a divisão não é inteira
			if(i != NUM_NODES - 1) {
				if(i == 1) {
					crop = in(Rect(0,divisao_h*(i - 1),divisao_w,divisao_h + 2));
					copyMakeBorder(crop, crop, 2, 0, border, border, BORDER_REPLICATE);
				} else {
					crop = in(Rect(0,divisao_h*(i - 1) - 2,divisao_w,divisao_h + 4));
					copyMakeBorder(crop, crop, 0, 0, border, border, BORDER_REPLICATE);
				}
			} else {
				crop =in(Rect(0,divisao_h*(i - 1) - 2,divisao_w,divisao_h + resto_divisao));
				copyMakeBorder(crop, crop, 0, 2, border, border, BORDER_REPLICATE);
			}
			//pego as dimensões do pedaço da imagem e salvo num tipo struct 'image' definido la em cima
			dimensoes[0] = crop.size().width;
			dimensoes[1] = crop.size().height;
			dimensoes[2] = tipo_img;
	
			// envio as dimensões para o nó
   			MPI_Send(dimensoes, 3, MPI_INT, i, 0, MPI_COMM_WORLD);
			// envio o pedaço da imagem'
   			if(tipo_img == 0){
				MPI_Send(crop.data, dimensoes[0]*dimensoes[1], MPI_UNSIGNED_CHAR, i, 1, MPI_COMM_WORLD);
   			} else if(tipo_img == 1) {
   				MPI_Send(crop.data, dimensoes[0]*dimensoes[1]*3, MPI_UNSIGNED_CHAR, i, 1, MPI_COMM_WORLD);
   			}
		}
		//apago a imagem original para economizar memória
		in.release();

		//recebe os pedaços dos nós
		for(int i = 1; i < NUM_NODES; i++) {
			MPI_Recv(dimensoes, 3, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
			if(dimensoes[2] == 0) {
				Mat pedaco(dimensoes[1], dimensoes[0] ,CV_8U);
				MPI_Recv(pedaco.data, dimensoes[0]*dimensoes[1], MPI_UNSIGNED_CHAR, i, 3, MPI_COMM_WORLD, &status);
				 //crop = pedaco(Rect(2, 2, pedaco.size().width - 2, pedaco.size().height - 2));
				if(i == 1) {
					vconcat(pedaco, out);
				} else {
					vconcat(out, pedaco, out);
				}
			} else if(dimensoes[2] == 1) {
				Mat pedaco(dimensoes[1], dimensoes[0] ,CV_8UC3);
				MPI_Recv(pedaco.data, dimensoes[0]*dimensoes[1]*3, MPI_UNSIGNED_CHAR, i, 3, MPI_COMM_WORLD, &status);
				//crop = pedaco(Rect(2, 2, pedaco.size().width - 2, pedaco.size().height - 2));
				if(i == 1) {
					vconcat(pedaco, out);
				} else {
					vconcat(out, pedaco, out);
				}
			}
		}
		//namedWindow("in", CV_WINDOW_AUTOSIZE );
		//imshow("in", out);
		//waitKey(0);

		//pega o tempo de fim, faz a diferença e imprime na tela
		gettimeofday(&fim,0);
    	float speedup = (fim.tv_sec + fim.tv_usec/1000000.0) - (inicio.tv_sec + inicio.tv_usec/1000000.0);
    	cout << "tempo MPI: " << speedup << endl;
		imwrite(argv[3], out);

	} else {
		//parte do código que roda dentro de cada nó
		int dimensoes[3];
		MPI_Recv(dimensoes, 3, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

		if(dimensoes[2] == 0) {
			Mat pedaco(Size(dimensoes[0], dimensoes[1]), CV_8U);	
			MPI_Recv(pedaco.data, dimensoes[0]*dimensoes[1], MPI_UNSIGNED_CHAR, 0, 1, MPI_COMM_WORLD, &status);
			Mat *out;
			out = aplica_smooth_grayscale(pedaco);
			pedaco.release();
			dimensoes[0] = out->size().width;
			dimensoes[1] =  out->size().height;
			MPI_Send(dimensoes, 3, MPI_INT, 0, 2, MPI_COMM_WORLD);
			MPI_Send(out->data, dimensoes[0]*dimensoes[1], MPI_UNSIGNED_CHAR, 0, 3, MPI_COMM_WORLD);

		} else if(dimensoes[2] == 1) {
			Mat pedaco(Size(dimensoes[0], dimensoes[1]), CV_8UC3);	
			MPI_Recv(pedaco.data, dimensoes[0]*dimensoes[1]*3, MPI_UNSIGNED_CHAR, 0, 1, MPI_COMM_WORLD, &status);
			Mat *out;
			out = aplica_smooth_color(pedaco);
			pedaco.release();
			dimensoes[0] = out->size().width;
			dimensoes[1] =  out->size().height;
			MPI_Send(dimensoes, 3, MPI_INT, 0, 2, MPI_COMM_WORLD);
			MPI_Send(out->data, dimensoes[0]*dimensoes[1]*3, MPI_UNSIGNED_CHAR, 0, 3, MPI_COMM_WORLD);
		}

	}
	MPI_Finalize();
	return 0;
}