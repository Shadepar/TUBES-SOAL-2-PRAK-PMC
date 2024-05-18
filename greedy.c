#include <stdio.h>
#include <string.h>
#include <float.h>
#include "greedy.h"

void greedy(int start,int len,double **jarak,char **kota){
    
    double lenght=0,min=DBL_MAX;
    int  i=start,lok=0;
    int *visited = malloc(len * sizeof(visited));
    for(int i=0;i<len;i++){
        visited[i] = 0;
    }
    visited[start] = 1;
    
    printf("Best route found:\n%s",kota[start]);
    for(int counter=0;counter<len-1;counter++){
        for(int j=0;j<len;j++){
            if (j!=i && visited[j] != 1){
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
        printf(" -> %s",kota[i]);
    }
    lenght += jarak[i][start];
    printf(" -> %s\n",kota[start]);
    printf("Best route distance:%f\n",lenght);
}