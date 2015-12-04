/*SmoothSeq SmoothSeq.cpp `pkg-config --cflags --libs opencv`
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
#include <math.h>

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
    int l_height = in.size().height, l_width = in.size().width;

    //numero de blocos é o total de pixels dividido pelo total de threads
    numBlocks = ceil((l_height*l_width)/nthreads);

	unsigned char *original,*saida;
  
    //Malloc especial do CUDA, para os vetores originais e de saída
    //Estes vetores são passados às funções que serão calculadas pela
    //placa de vídeo
    

    cudaMalloc(&original, (l_width + 4) * (l_height + 4));
    cudaMalloc(&saida, l_width * l_height);

	//pegar o tempo de inicio
    
    struct timeval inicio, fim;
    gettimeofday(&inicio,0);
    
    cudaMemcpy(original, in.data,l_width * l_height, cudaMemcpyHostToDevice);

	smooth<<<numBlocks,nthreads>>>(original, saida, l_height, l_width);
	
	out = new Mat(l_height, l_width, CV_8U, 1);
	cudaMemcpy(out->data, saida, l_width*l_height,cudaMemcpyDeviceToHost);
	
	//pega o tempo de fim, faz a diferença e imprime na tela
	gettimeofday(&fim,0);
    float speedup = (fim.tv_sec + fim.tv_usec/1000000.0) - (inicio.tv_sec + inicio.tv_usec/1000000.0);
    cout << speedup << endl;
	imwrite(fileOut, *out);
	in.release();
	out->release();
    cudaFree(original);
    cudaFree(saida);

    return 0;
}
