#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include "AntColonyOptimization.h"

double haversine(double lat1, double lon1, double lat2, double lon2){
    lat1 = lat1*(3.14159265359/180);
    lon1 = lon1*(3.14159265359/180);
    lat2 = lat2*(3.14159265359/180);
    lon2 = lon2*(3.14159265359/180);
    return 2*6371*asin(sqrt(pow(sin((lat2-lat1)/2),2)+cos(lat1)*cos(lat2)*pow(sin((lon2-lon1)/2),2)));
}

int main(){
    clock_t t;
    char filename[20];
    char line[1000];
    char *token;
    printf("Enter list of cities file name: ");
    scanf("%s",filename);

    FILE* fp = fopen(filename, "r");
    if (fp == NULL){
        printf("There exist is no such file");
        return 0;
    }

    int len = 0;
    while (fgets(line,1000,fp)){
        len++;
    }

    if (len<6||len>15){
        printf("The amount of cities is not within boundary");
        return 0;
    }

    char** kota = malloc(len * sizeof(*kota));
    for (int i=0;i<len;i++){
        kota[i] = malloc(100 * sizeof(*kota[i]));
    }

    double latlong[len][2];
    len = 0;
    fclose(fp);
    fp = fopen(filename, "r");
    while (fgets(line,1000,fp)){
        token = strtok(line,",");
        strcpy(kota[len],token);
        token = strtok(NULL,",");
        latlong[len][0] = strtod(token,NULL);
        token = strtok(NULL,",");
        latlong[len][1] = strtod(token,NULL);
        len++;
    }

    double** jarak = malloc(len * sizeof(*jarak));
    for(int i=0;i<len;i++){
        jarak[i] = malloc(len * sizeof(*jarak[i]));
        for(int j=0;j<len;j++){
            if (i==j){
                jarak[i][j] = -1.0;
            }
            else{
                jarak[i][j] = haversine(latlong[i][0],latlong[i][1],latlong[j][0],latlong[j][1]);
            }
        }
    }

    char str[100];
    printf("Enter starting point: ");
    scanf("%s",str);

    int i=0;
    while(i != len){
        if (strcmp(str,kota[i]) == 0){
            break;
        }
        i++;
    }

    if (i==len){
        printf("There exist is no such city");
        return 0;
    }
    
    t = clock();
    AntColonyOptimization(i, len, jarak, kota);
    t = clock()-t;
    printf("Time elapsed: %.20f", ((double)t)/CLOCKS_PER_SEC);
}