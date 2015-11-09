#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <mpi.h>
/*
	mpic++ -o SmoothMPI SmoothMPI.cpp `pkg-config --cflags --libs opencv`
	mpirun -np 4 SmoothMPI
*/

//#define NUM_NODES 4
#define MASTER 0
//	g++ -o cropImage cropImage.cpp `pkg-config --cflags --libs opencv`
			//crop[i].release();
			/*printf("%d %d %d\n",i,crop[i].size().height,crop[i].size().width);
			namedWindow("in", CV_WINDOW_AUTOSIZE );
			imshow("in", crop[i]);
			waitKey(0);*/
using namespace cv;
using namespace std;

Mat *aplica_smooth_grayscale(Mat& in) {

	//border = 1 pois tamanho da máscara = 5 => floor(5/2) = 2
	int border = 2;
	Mat *out = new Mat(in.size(), CV_8U, 1);
	//replica a borda pra solucionar o problema dos pixels de borda
	copyMakeBorder(in, in, border, border, border, border, BORDER_REPLICATE);
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
		//	printf("%f ",average);	
			out->at<uchar>(i - border, j - border) = (uchar)average;
		}
	}
	return out;
}


Mat *aplica_smooth_color(Mat& in) {

	//copia imagem de entrada 
	Mat *out = new Mat(in.size(), CV_8UC3, 1);
	//Mat aux(in->size(), CV_8UC3, 1);
	//border = 1 pois tamanho da máscara = 5 => floor(5/2) = 2
	int border = 2;
	
	//replica a borda pra solucionar o problema dos pixels de borda
	copyMakeBorder(in, in, border, border, border, border, BORDER_REPLICATE);

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
    int h_img_entrada, w_img_entrada;

    //printf("NUMERO DE NOS = %d\n",NUM_NODES);
    if(rank == 0)  {
    	//le a imagem  e salva suas dimensões para um futuro uso
    	if(tipo_img == 0) {
	    	in = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
		} else if(tipo_img == 1) {
			in = imread(argv[1], CV_LOAD_IMAGE_COLOR);
		}
	    h_img_entrada = in.size().height;
	    w_img_entrada = in.size().width;
	    printf("entrada -> h:%d w:%d\n", h_img_entrada,w_img_entrada);
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
				crop =in(Rect(0,divisao_h*(i - 1),divisao_w,divisao_h));
			} else {
				crop =in(Rect(0,divisao_h*(i - 1),divisao_w,divisao_h + resto_divisao));
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
		//out(h_img_entrada, w_img_entrada,  CV_8U, 1);
	
		for(int i = 1; i < NUM_NODES; i++) {
			MPI_Recv(dimensoes, 3, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
			//printf("%d %d\n", dimensoes[0],dimensoes[1]);
			if(dimensoes[2] == 0) {
				Mat pedaco(dimensoes[1], dimensoes[0] ,CV_8U);
				MPI_Recv(pedaco.data, dimensoes[0]*dimensoes[1], MPI_UNSIGNED_CHAR, i, 3, MPI_COMM_WORLD, &status);
				if(i == 1) {
					vconcat(pedaco, out);
				} else {
					vconcat(out, pedaco, out);
				}
			} else if(dimensoes[2] == 1) {
				Mat pedaco(dimensoes[1], dimensoes[0] ,CV_8UC3);
				MPI_Recv(pedaco.data, dimensoes[0]*dimensoes[1]*3, MPI_UNSIGNED_CHAR, i, 3, MPI_COMM_WORLD, &status);
				if(i == 1) {
					vconcat(pedaco, out);
				} else {
					vconcat(out, pedaco, out);
				}
			}
		}
			namedWindow("in", CV_WINDOW_AUTOSIZE );
			imshow("in", out);
			waitKey(0);
		//printf("MASTER SHOW DE BOLA\n");
	} else {
		//parte do código que roda dentro de cada nó
		int dimensoes[3];
		MPI_Recv(dimensoes, 3, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		printf("w:%d h:%d\n", dimensoes[0],dimensoes[1]);
		if(dimensoes[2] == 0) {
			Mat pedaco(Size(dimensoes[0], dimensoes[1]), CV_8U);	
			MPI_Recv(pedaco.data, dimensoes[0]*dimensoes[1], MPI_UNSIGNED_CHAR, 0, 1, MPI_COMM_WORLD, &status);
			Mat *out;
			out = aplica_smooth_grayscale(pedaco);
			pedaco.release();

			MPI_Send(dimensoes, 3, MPI_INT, 0, 2, MPI_COMM_WORLD);
			MPI_Send(out->data, dimensoes[0]*dimensoes[1], MPI_UNSIGNED_CHAR, 0, 3, MPI_COMM_WORLD);

		} else if(dimensoes[2] == 1) {
			Mat pedaco(Size(dimensoes[0], dimensoes[1]), CV_8UC3);	
			MPI_Recv(pedaco.data, dimensoes[0]*dimensoes[1]*3, MPI_UNSIGNED_CHAR, 0, 1, MPI_COMM_WORLD, &status);
			Mat *out;
			out = aplica_smooth_color(pedaco);
			pedaco.release();
			MPI_Send(dimensoes, 3, MPI_INT, 0, 2, MPI_COMM_WORLD);
			MPI_Send(out->data, dimensoes[0]*dimensoes[1]*3, MPI_UNSIGNED_CHAR, 0, 3, MPI_COMM_WORLD);
		}
		//namedWindow("in", CV_WINDOW_AUTOSIZE );
		//imshow("in", mat);
		//waitKey(0);
	}

	MPI_Finalize();
	return 0;
}