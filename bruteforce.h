#ifndef BRUTEFORCE_H
#define BRUTEFORCE_H

void bruteForce(int i, int len, double **jarak, char **kota);
void permute(int start, int i, int len, double **jarak, char **kota, int *rute, double *shortestDistance, int *shortestPath);
void swap(int *x, int *y);
#endif