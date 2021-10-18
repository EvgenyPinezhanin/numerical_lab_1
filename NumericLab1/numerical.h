#ifndef NUMERICAL_H
#define NUMERICAL_H

#include<cmath>

// �������� ��������� �����������, ��� ������ ���
int rk4_method_test(double h, int Nmax, double x0, double u0, double x, double eps, double b,
    double (*f)(double, double), double (*func)(double), double* X, double* V, double* V2,
    double *OLP, double* H, int *C1, int *C2, double* U, double* U_V);

// ��� ������ ���
int rk4_method_test(double h, int Nmax, double x0, double u0, double x, double b,
    double (*f)(double, double), double (*func)(double), double* X, double* V, double* V2, double *U, double* U_V);

// �������� ��������� �����������, ��� ������ ���
int rk4_method(double h, int Nmax, double x0, double u0, double x, double eps, double b, double (*f)(double, double),
    double* X, double* V, double* V2, double* OLP, double* H, int* C1, int* C2);

// ��� ������ ���
int rk4_method(double h, int Nmax, double x0, double u0, double x, double b,
    double (*f)(double, double), double* X, double* V, double* V2);

// �������� ��������� �����������, ��� ������� ��� �� ���� ���������
int rk4_method(double h, int Nmax, double x0, double u10, double u20, double x, double eps, double b,
    double (*f1)(double, double, double), double (*f2)(double, double, double), double* X, double* V_1, double* V_2,
    double* V2_1, double* V2_2, double* OLP, double* H, int* C1, int* C2);

// ��� ������� ��� �� ���� ���������
int rk4_method(double h, int Nmax, double x0, double u10, double u20, double x, double b,
    double (*f1)(double, double, double), double (*f2)(double, double, double), double* X, double* V_1, double* V_2,
    double* V2_1, double* V2_2);

#endif
