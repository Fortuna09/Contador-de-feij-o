/*=============================================================
*              UNIFAL = Universidade Federal de Alfenas.
*               BACHARELADO EM CIENCIA DA COMPUTACAO.
* Trabalho...: Contagem de feijão
* Professor..: Luiz Eduardo da Silva
* Aluno......: Rafael Silva Fortuna
* Data.......: 20/05/2024
*=============================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) || defined(__WIN64__) || defined(__CYGWIN__)
#include "..\\utils\\imagelib.h"
#elif defined(__linux__)
#include "../utils/imagelib.h"
#endif


//negative
image neg(image In){
    float T[In->ml + 1];

    image Out = img_clone(In);
    for (int i = 0; i < In->ml + 1; i++)
        T[i] = i > 100 ? 0 : 1;  

    for (int i = 0; i < In->nr * In->nc; i++)
        Out->px[i] = T[In->px[i]];
    
    return Out;
}

//intensity
image intensity(image In){
    float T[In->ml + 1];

    image Out = img_clone(In);
    for (int i = 0; i < In->ml + 1; i++)
        T[i] = i > 5 ? 1 : 0;  

    for (int i = 0; i < In->nr * In->nc; i++)
        Out->px[i] = T[In->px[i]];
    
    return Out;
}


//func menor
int menor(int a, int b) {
    if (a < b) return a;
    return b;
}

//distance
void distance(image In) {
    int nr = In->nr;
    int nc = In->nc;
    int *px = In->px;
    int max_distance = 0;

    for (int i = 1; i < nr - 1; i++) {
        for (int j = 1; j < nc - 1; j++) {
            int pos = i * nc + j;
            int atual = px[pos];
            int top = px[(i - 1) * nc + j];
            int left = px[i * nc + j - 1];

            if (atual != 0) {
                px[pos] = menor(top + 1, left + 1); 
            }
        }
    }

    for (int i = nr - 2; i > 0; i--) {
        for (int j = nc - 2; j > 0; j--) {
            int pos = i * nc + j;
            int atual = px[pos];

            //neighbors
            int top = px[(i + 1) * nc + j];
            int left = px[i * nc + j + 1];

            if (atual != 0) {
                int min_ = menor(top + 1, left + 1);
                px[pos] = (atual
 < min_) ? atual
 : min_; 
                if (max_distance < px[pos])
                    max_distance = px[pos]; 
            }
        }
    }
    In->ml = max_distance;
}


int find(int parent[], int i)
{
    while (parent[i] != i)
        i = parent[i];
    return i;
}

void Union(int parent[], int i, int j)
{
    int x = find(parent, i);
    int y = find(parent, j);
    parent[y] = x;
}

//count coffes
int countcoffes(image In, int parent[]) {
     int numbers = 0;
     int rotulos_useds[1000];

    for(int i = 0; i < 1000; i++)
        rotulos_useds[i] = 0;

    for (int i = 0; i < In->nc * In->nr; i++) {
        int root = find(parent, In->px[i]);
        if (rotulos_useds[root] == 0 && root != 0) { 
            rotulos_useds[root] = 1;
            numbers++;
        }
    }
    return numbers;
}

//label
int label(image In) {
    int nr = In->nr;
    int nc = In->nc;
    int *p = In->px;

    int label = 0;
    int parent[1000];

    for (int i = 0; i < 1000; i++)
        parent[i] = i;
    for (int i = 1; i < nr; i++)
        for (int j = 1; j < nc; j++){
            int pos = i * nc + j;

            int atual = p[pos]; 
            int top = p[(i - 1) * nc + j]; 
            int left = p[pos - 1]; 

            if (atual != 0)
            {
                if (top == 0 && left == 0) {
                    p[pos] = ++label;
                }
                if (top != 0 && left == 0) p[pos] = top;
                if (top == 0 && left != 0) p[pos] = left;
                if (top != 0 && left != 0 && left == top) p[pos] = top;
                if (top != 0 && left != 0 && left != top){
                    p[pos] = left;
                    Union(parent, top, left);
                }
            }
        }
        
    for (int i = 0; i < nr * nc; i++){
        if(In->px[i] != 0)
            In->px[i] = find(parent, In->px[i]);
    }

    In->ml = label;
    return countcoffes(In, parent);
}

void msg(){
    printf("\nRafael SIlva Fortuna");
    printf("\n-------------------------------");
    printf("\nUsage:  ./contafeijao  image-name[.pgm]\n\n");
    exit(1);
}

/*-------------------------------------------------------------------------
 * main function
 *-------------------------------------------------------------------------*/
int main(int argc, char *argv[]){
    int nc, nr, ml, tp;
    char *p, nameIn[100], nameOut[100], cmd[110];
    image In, Out;
    if (argc < 2)
        msg();
//Define input/output file name
    img_name(argv[1], nameIn, nameOut, GRAY, GRAY);
//read image
    In = img_get(nameIn, GRAY);
//negative
    Out = neg(In);
//distance
    distance(Out);
//intensity
    Out = intensity(Out);
//label
    int labels = label(Out);

//Save image
    img_put(Out, nameOut, GRAY);
    printf("\n\nImage File Name: %s", argv[1]);
    printf("\n#componentes= %i\n\n", labels);

 //Show image 
    sprintf(cmd, "%s %s &", VIEW, nameOut);
    puts(cmd);
    system(cmd);

//Free memory
    img_free(In);
    img_free(Out);
    return 0;
}