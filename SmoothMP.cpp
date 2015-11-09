/*	
	g++ -o SmoothMP SmoothMP.cpp `pkg-config --cflags --libs opencv` -fopenmp
	./SmoothMP image_in type_img image_out
	type_img -> 0 = GRAYSCALE
	type_img -> 1 = COLOR
*/
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
#include <time.h>
#define NTHREADS 4

using namespace cv;
using namespace std;

//matriz com a imagem de entrada
Mat in;
Mat out_p;
//border = 1 pois tamanho da máscara = 5 => floor(5/2) = 2
int border = 2;

void aplica_smooth_grayscale(int ini_i, int ini_j, int end_i, int end_j) {
	float average;
	for(int i = ini_i + border; i < end_i - border; i++) {
		for(int j = ini_j + border; j < end_j - border; j++) {
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
			out_p.at<uchar>(i - border, j - border) = (uchar)average;
		}
	}
}
void aplica_smooth_color(int ini_i, int ini_j, int end_i, int end_j) {
	float average;
	for(int i = ini_i + border; i < end_i - border; i++) {
		for(int j = ini_j + border; j < end_j - border; j++) {
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
				out_p.at<cv::Vec3b>(i - border, j - border)[color] = (uchar)average;
			}
		}
	}
}
int smooth(int tipo_img) {

	//copia imagem de entrada
	if(tipo_img == 0) {
		out_p = Mat(in.size(), CV_8U, 1);
	} else if(tipo_img == 1) {
		out_p = Mat(in.size(), CV_8UC3, 1);
	} else {
		return 0;
	}
	
	//achar a média aritmética e depois atualizar o pixel
	float average;
	omp_set_num_threads(NTHREADS);	
	//int ini_i = 0, ini_j = 0, end_i = 100, end_j = 100;
	int ini_i[4],ini_j[4],end_i[4],end_j[4];
	int meio_w, meio_h;
	//replica a borda pra solucionar o problema dos pixels de borda
	copyMakeBorder(in, in, border, border, border, border, BORDER_REPLICATE);
	//calcula o meio da imagem
	meio_h = in.size().height/2;
	meio_w = in.size().width/2;
	//calcula o inicio de cada pedaço da imagem
	ini_i[0] = 0; 							ini_j[0] = 0;
	ini_i[1] = 0; 							ini_j[1] = meio_w - border;
	ini_i[2] = meio_h - border; 			ini_j[2] = 0;
	ini_i[3] = meio_h - border; 			ini_j[3] = meio_w - border;
	//calcula o fim de cada pedaço da imagem
	end_i[0] = meio_h + border; 			end_j[0] = meio_w + border;
	end_i[1] = meio_h + border; 			end_j[1] = in.size().width;
	end_i[2] = in.size().height;		 	end_j[2] = meio_w + border;
	end_i[3] = in.size().height;			end_j[3] = in.size().width;

	//chama cada iteração do for junto em paralelo passando como parametro o 
	//inicio e o fim de cada pedaço da imagem qu	e cada thread deve rodar
	#pragma omp parallel for shared(in,out_p, end_i, end_j, ini_i, ini_j)
		for(int i = 0; i < NTHREADS; i ++) {
			if(tipo_img == 0) {
				aplica_smooth_grayscale(ini_i[i], ini_j[i], end_i[i], end_j[i]);
			} else {
				aplica_smooth_color(ini_i[i], ini_j[i], end_i[i], end_j[i]);
			}
		}
	return 1;
}


int main(int argc, char *argv[]) {
	//diz se a imagem é grayscale or color
	int tipo_img = atoi(argv[2]);
	//arquivo de entrada
	const char *fileIn, *fileOut;

	//matriz que receberá a imagem de saida
	//Mat *out;

	//le o nome da imagem
	fileIn = argv[1];
	fileOut = argv[3];
	//le e salva a imagem na matriz
	if(tipo_img == 0) {
		in = imread(fileIn, CV_LOAD_IMAGE_GRAYSCALE);
	} else if(tipo_img == 1) {
		in = imread(fileIn, CV_LOAD_IMAGE_COLOR);
	} else {
		cout << "Tipo de imagem nao suportado" << endl;
		return -1;
	}
	//caso nao consegui abrir a imagem
	if (in.empty()) {
		cout << "Nao foi possivel abrir a  imagem: " << endl;
		return -1;
	}

	//aplica algoritmo smooth e recebe a nova imagem
	if(smooth(tipo_img) == 1) {
		imwrite(fileOut, out_p);
	} else {
		printf("Nao foi possivel criar nova imagem.\n");
	}
	in.release();
	out_p.release();
    return 0;
}
