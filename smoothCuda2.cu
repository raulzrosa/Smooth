/*
 SmoothSeq SmoothSeq.cpp `pkg-config --cflags --libs opencv`
	./SmoothSeq image_in type_img image_out
	type_img -> 0 = GRAYSCALE
	type_img -> 1 = COLOR
*/
	
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <sys/time.h>


using namespace cv;
using namespace std;

//Função que calcula a média de uma "matriz" 5x5 a partir de uma dada posição
__global__ void smooth( unsigned char *entrada,unsigned char *saida, int n_linhas, int n_colunas ) {
    //Calcula a posição no vetor (id_bloco * total_blocos + id_thread)
    int posicao = blockIdx.x * blockDim.x + threadIdx.x;
    //Se a posição não é maior que o limite da imagem original...
    if(posicao < (n_linhas)*(n_colunas)) {
        //soma o valor da região 5x5 em torno no pixel
        saida[posicao] =entrada[posicao]+
                        entrada[posicao+(n_colunas+4)]+
                        entrada[posicao+(2*(n_colunas+4))]+
                        entrada[posicao+(3*(n_colunas+4))]+
                        entrada[posicao+(4*(n_colunas+4))]+
                        entrada[posicao+1]+
                        entrada[posicao+(n_colunas+4)+1]+
                        entrada[posicao+(2*(n_colunas+4))+1]+
                        entrada[posicao+(3*(n_colunas+4))+1]+
                        entrada[posicao+(4*(n_colunas+4))+1]+
                        entrada[posicao+2]+
                        entrada[posicao+(n_colunas+4)+2]+
                        entrada[posicao+(2*(n_colunas+4))+2]+
                        entrada[posicao+(3*(n_colunas+4))+2]+
                        entrada[posicao+(4*(n_colunas+4))+2]+
                        entrada[posicao+3]+
                        entrada[posicao+(n_colunas+4)+3]+
                        entrada[posicao+(2*(n_colunas+4))+3]+
                        entrada[posicao+(3*(n_colunas+4))+3]+
                        entrada[posicao+(4*(n_colunas+4))+3]+
                        entrada[posicao+4]+
                        entrada[posicao+(n_colunas+4)+4]+
                        entrada[posicao+(2*(n_colunas+4))+4]+
                        entrada[posicao+(3*(n_colunas+4))+4]+
                        entrada[posicao+(4*(n_colunas+4))+4];
        //calcula a média
        saida[posicao] = saida[posicao]/25;
    }
}

int main(int argc, char *argv[]) {
	//diz se a imagem é grayscale or color
	int tipo_img = atoi(argv[2]);
	//arquivo de entrada
	const char *fileIn, *fileOut;
	
	//numero maximo de threads da placa do andromeda
    int nthreads = 1024;

    float nb;
    int numBlocks;


	//matriz com a imagem de entrada
	Mat in;
	//matriz que receberá a imagem de saida
	Mat *out;

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

    //numero de blocos é o total de pixels dividido pelo total de threads
    nb = (in.size().width*in.size().height)/nthreads;
    cout << nb << endl;
    //O cast trunca o ponto flutuante, por isso soma-se 1
    numBlocks = (int) (nb + 1.0);
	cout << numBlocks << endl;
	unsigned char *original,*final;
	
    //Malloc especial do CUDA, para os vetores originais e de saída
    //Estes vetores são passados às funções que serão calculadas pela
    //placa de vídeo
    
    int *height, *width;
    cudaMalloc(&original, (in.size().width + 4)*(in.size().height + 4));
    cudaMalloc(&final, in.size().width*in.size().height);
    cudaMalloc(&height, sizeof(int));
    cudaMalloc(&width, sizeof(int));	
    int l_height = in.size().height, l_width = in.size().width;
	//pegar o tempo de inicio
	struct timeval inicio, fim;
    gettimeofday(&inicio,0);
    
    cudaMemcpy(original, in.data,( in.size().width+4)*(in.size().height+4), cudaMemcpyHostToDevice);
	cudaMemcpy(height, &l_height, sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(width, &l_width, sizeof(int), cudaMemcpyHostToDevice);
	smooth<<<numBlocks,nthreads>>>(original,final, *height, *width);
	
	out = new Mat(in.size().height, in.size().width, CV_8U, 1);
	cudaMemcpy(out->data, final, (out->size().width)*(out->size().height),cudaMemcpyDeviceToHost);
	
	//pega o tempo de fim, faz a diferença e imprime na tela
	gettimeofday(&fim,0);
    float speedup = (fim.tv_sec + fim.tv_usec/1000000.0) - (inicio.tv_sec + inicio.tv_usec/1000000.0);
    cout << speedup << endl;
	imwrite(fileOut, *out);
	in.release();
	out->release();
    cudaFree(original);
    cudaFree(final);
    return 0;
}


//ssh grupo18b@halley.lasdpc.icmc.usp.br -p 22200
//grupo18b#raul


