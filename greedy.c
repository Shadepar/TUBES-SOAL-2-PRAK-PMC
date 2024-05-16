#include <stdio.h>
#include <string.h>
#include <float.h>
#include "greedy.h"

void greedy(int start,int len,double **jarak,char **kota){
    double lenght=0;
    int  i=start,lok=0;
    double min = DBL_MAX;
    char rute[len][100];
    int visited[len];
    visited[start] = 1;
    
    for(int counter=0;counter<len-1;counter++){
        for(int j=0;j<len;j++){
            if (j!=i && visited[j] == 0){
                if (jarak[i][j] < min){
                    min = jarak[i][j];
                    lok = j;
                }
            }
        }
        lenght += min;
        min = DBL_MAX;
        i = lok;
        visited[i] = 1;
        strcpy(rute[counter],kota[i]);
    }
    lenght += jarak[i][start];
    strcpy(rute[len-1],kota[start]);
    
    printf("Best route found:\n%s",kota[start]);
    for (int i = 0; i<len; i++)
    {
        printf(" -> %s",rute[i]);
    }
    printf("\n");
    printf("Best route distance:%f\n",lenght);
}