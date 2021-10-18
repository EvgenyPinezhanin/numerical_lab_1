#include"numerical.h"

// контроль локальной погрешности, для одного ОДУ
int rk4_method_test(double h, int Nmax, double x0, double u0, double x, double eps, double b,
    double (*f)(double, double), double (*func)(double), double* X, double* V, double* V2,
    double* OLP, double* H, int* C1, int* C2, double* U, double* U_V) {
    int N = 1;
    double S;

    for (int i = 0; i < Nmax + 1; i++) {
        C1[i] = 0; C2[i] = 0;
    }

    X[0] = x0;
    V[0] = u0;
    U_V[0] = 0;
    H[0] = h;
    OLP[0] = 0.0;
    double x0_5, v0_5;
    double k1, k2, k3, k4;
    while (true) {
        X[N] = X[N - 1] + H[N - 1];

        k1 = f(X[N - 1], V[N - 1]);
        k2 = f(X[N - 1] + H[N - 1] / 2, V[N - 1] + H[N - 1] / 2 * k1);
        k3 = f(X[N - 1] + H[N - 1] / 2, V[N - 1] + H[N - 1] / 2 * k2);
        k4 = f(X[N], V[N - 1] + H[N - 1] * k3);

        V[N] = V[N - 1] + H[N - 1] / 6 * (k1 + 2 * k2 + 2 * k3 + k4);
        U[N] = func(X[N]);
        U_V[N] = abs(U[N] - V[N]);

        //k1 = f(X[N - 1], V[N - 1]);
        k2 = f(X[N - 1] + H[N - 1] / 4, V[N - 1] + H[N - 1] / 4 * k1);
        k3 = f(X[N - 1] + H[N - 1] / 4, V[N - 1] + H[N - 1] / 4 * k2);
        k4 = f(X[N - 1] + H[N - 1] / 2, V[N - 1] + H[N - 1] / 2 * k3);

        x0_5 = X[N - 1] + H[N - 1] / 2;
        v0_5 = V[N - 1] + H[N - 1] / 12 * (k1 + 2 * k2 + 2 * k3 + k4);

        k1 = f(x0_5, v0_5);
        k2 = f(x0_5 + H[N - 1] / 4, v0_5 + H[N - 1] / 4 * k1);
        k3 = f(x0_5 + H[N - 1] / 4, v0_5 + H[N - 1] / 4 * k2);
        k4 = f(X[N], v0_5 + H[N - 1] / 2 * k3);

        V2[N] = v0_5 + H[N - 1] / 12 * (k1 + 2 * k2 + 2 * k3 + k4);

        S = (V2[N] - V[N]) / 15.0;
        OLP[N] = S * 16.0;

        if ((eps / (16.0)) <= abs(S) && abs(S) <= eps) {
            H[N] = H[N - 1];
        }
        else if (abs(S) < (eps / (16.0))) {
            H[N] = 2 * H[N - 1];
            C2[N]++;
        }
        else {
            H[N - 1] = H[N - 1] / 2;
            C1[N]++;
            N--;
        }

        if (X[N] > x - b) {
            if (X[N] >= x) {
                N--;
            }
            break;
        }
        if (N == Nmax) break;
        N++;
    }
    return N;
}

// для одного ОДУ
int rk4_method_test(double h, int Nmax, double x0, double u0, double x, double b,
    double (*f)(double, double), double (*func)(double), double* X, double* V, double* V2, double* U, double* U_V) {
    int N = 1;

    X[0] = x0;
    V[0] = u0;
    U_V[0] = 0;
    double x0_5, v0_5;
    double k1, k2, k3, k4;
    while (true) {
        X[N] = X[N - 1] + h;

        k1 = f(X[N - 1], V[N - 1]);
        k2 = f(X[N - 1] + h / 2, V[N - 1] + h / 2 * k1);
        k3 = f(X[N - 1] + h / 2, V[N - 1] + h / 2 * k2);
        k4 = f(X[N], V[N - 1] + h * k3);

        V[N] = V[N - 1] + h / 6 * (k1 + 2 * k2 + 2 * k3 + k4);
        U[N] = func(X[N]);
        U_V[N] = abs(U[N] - V[N]);

        //k1 = f(X[N - 1], V[N - 1]);
        k2 = f(X[N - 1] + h / 4, V[N - 1] + h / 4 * k1);
        k3 = f(X[N - 1] + h / 4, V[N - 1] + h / 4 * k2);
        k4 = f(X[N - 1] + h / 2, V[N - 1] + h / 2 * k3);

        x0_5 = X[N - 1] + h / 2;
        v0_5 = V[N - 1] + h / 12 * (k1 + 2 * k2 + 2 * k3 + k4);

        k1 = f(x0_5, v0_5);
        k2 = f(x0_5 + h / 4, v0_5 + h / 4 * k1);
        k3 = f(x0_5 + h / 4, v0_5 + h / 4 * k2);
        k4 = f(X[N], v0_5 + h / 2 * k3);

        V2[N] = v0_5 + h / 12 * (k1 + 2 * k2 + 2 * k3 + k4);

        if (X[N] > x - b) {
            if (X[N] >= x) {
                N--;
            }
            break;
        }
        if (N == Nmax) break;
        N++;
    }
    return N;
}

// контроль локальной погрешности, для одного ОДУ
int rk4_method(double h, int Nmax, double x0, double u0, double x, double eps, double b, double (*f)(double, double),
    double* X, double* V, double* V2, double* OLP, double* H, int* C1, int* C2) {
    int N = 1;
    double S;

    for (int i = 0; i < Nmax + 1; i++) {
        C1[i] = 0; C2[i] = 0;
    }

    X[0] = x0;
    V[0] = u0;
    H[0] = h;
    OLP[0] = 0.0;
    double x0_5, v0_5;
    double k1, k2, k3, k4;
    while (true) {
        X[N] = X[N - 1] + H[N - 1];

        k1 = f(X[N - 1], V[N - 1]);
        k2 = f(X[N - 1] + H[N - 1] / 2, V[N - 1] + H[N - 1] / 2 * k1);
        k3 = f(X[N - 1] + H[N - 1] / 2, V[N - 1] + H[N - 1] / 2 * k2);
        k4 = f(X[N], V[N - 1] + H[N - 1] * k3);

        V[N] = V[N - 1] + H[N - 1] / 6 * (k1 + 2 * k2 + 2 * k3 + k4);

        //k1 = f(X[N - 1], V[N - 1]);
        k2 = f(X[N - 1] + H[N - 1] / 4, V[N - 1] + H[N - 1] / 4 * k1);
        k3 = f(X[N - 1] + H[N - 1] / 4, V[N - 1] + H[N - 1] / 4 * k2);
        k4 = f(X[N - 1] + H[N - 1] / 2, V[N - 1] + H[N - 1] / 2 * k3);

        x0_5 = X[N - 1] + H[N - 1] / 2;
        v0_5 = V[N - 1] + H[N - 1] / 12 * (k1 + 2 * k2 + 2 * k3 + k4);

        k1 = f(x0_5, v0_5);
        k2 = f(x0_5 + H[N - 1] / 4, v0_5 + H[N - 1] / 4 * k1);
        k3 = f(x0_5 + H[N - 1] / 4, v0_5 + H[N - 1] / 4 * k2);
        k4 = f(X[N], v0_5 + H[N - 1] / 2 * k3);

        V2[N] = v0_5 + H[N - 1] / 12 * (k1 + 2 * k2 + 2 * k3 + k4);

        S = (V2[N] - V[N]) / 15.0;
        OLP[N] = S * 16;

        if ((eps / (16.0)) <= abs(S) && abs(S) <= eps) {
            H[N] = H[N - 1];
        }
        else if (abs(S) < (eps / (16.0))) {
            H[N] = 2 * H[N - 1];
            C2[N]++;
        }
        else {
            H[N - 1] = H[N - 1] / 2;
            C1[N]++;
            N--;
        }

        if (X[N] > x - b) {
            if (X[N] >= x) {
                N--;
            }
            break;
        }
        if (N == Nmax) break;
        N++;
    }
    return N;
}

// для одного ОДУ
int rk4_method(double h, int Nmax, double x0, double u0, double x, double b,
    double (*f)(double, double), double* X, double* V, double* V2) {
    int N = 1;

    X[0] = x0;
    V[0] = u0;
    double x0_5, v0_5;
    double k1, k2, k3, k4;
    while (true) {
        X[N] = X[N - 1] + h;

        k1 = f(X[N - 1], V[N - 1]);
        k2 = f(X[N - 1] + h / 2, V[N - 1] + h / 2 * k1);
        k3 = f(X[N - 1] + h / 2, V[N - 1] + h / 2 * k2);
        k4 = f(X[N], V[N - 1] + h * k3);

        V[N] = V[N - 1] + h / 6 * (k1 + 2 * k2 + 2 * k3 + k4);

        //k1 = f(X[N - 1], V[N - 1]);
        k2 = f(X[N - 1] + h / 4, V[N - 1] + h / 4 * k1);
        k3 = f(X[N - 1] + h / 4, V[N - 1] + h / 4 * k2);
        k4 = f(X[N - 1] + h / 2, V[N - 1] + h / 2 * k3);

        x0_5 = X[N - 1] + h / 2;
        v0_5 = V[N - 1] + h / 12 * (k1 + 2 * k2 + 2 * k3 + k4);

        k1 = f(x0_5, v0_5);
        k2 = f(x0_5 + h / 4, v0_5 + h / 4 * k1);
        k3 = f(x0_5 + h / 4, v0_5 + h / 4 * k2);
        k4 = f(X[N], v0_5 + h / 2 * k3);

        V2[N] = v0_5 + h / 12 * (k1 + 2 * k2 + 2 * k3 + k4);

        if (X[N] > x - b) {
            if (X[N] >= x) {
                N--;
            }
            break;
        }
        if (N == Nmax) break;
        N++;
    }
    return N;
}

// контроль локальной погрешности, для системы ОДУ из двух уравнений
int rk4_method(double h, int Nmax, double x0, double u10, double u20, double x, double eps, double b,
    double (*f1)(double, double, double), double (*f2)(double, double, double), double* X, double* V_1, double* V_2,
    double* V2_1, double* V2_2, double* OLP, double* H, int* C1, int* C2) {
    int N = 1;
    double S, S1, S2;

    for (int i = 0; i < Nmax + 1; i++) {
        C1[i] = 0; C2[i] = 0;
    }

    X[0] = x0;
    V_1[0] = u10;
    V_2[0] = u20;
    H[0] = h;
    double x0_5, v0_51, v0_52;
    double k11, k12, k21, k22, k31, k32, k41, k42;
    while (true) {
        X[N] = X[N - 1] + H[N - 1];

        k11 = f1(X[N - 1], V_1[N - 1], V_2[N - 1]);
        k12 = f2(X[N - 1], V_1[N - 1], V_2[N - 1]);
        k21 = f1(X[N - 1] + H[N - 1] / 2, V_1[N - 1] + H[N - 1] / 2 * k11, V_2[N - 1] + H[N - 1] / 2 * k12);
        k22 = f2(X[N - 1] + H[N - 1] / 2, V_1[N - 1] + H[N - 1] / 2 * k11, V_2[N - 1] + H[N - 1] / 2 * k12);
        k31 = f1(X[N - 1] + H[N - 1] / 2, V_1[N - 1] + H[N - 1] / 2 * k21, V_2[N - 1] + H[N - 1] / 2 * k22);
        k32 = f2(X[N - 1] + H[N - 1] / 2, V_1[N - 1] + H[N - 1] / 2 * k21, V_2[N - 1] + H[N - 1] / 2 * k22);
        k41 = f1(X[N], V_1[N - 1] + H[N - 1] * k31, V_2[N - 1] + H[N - 1] * k32);
        k42 = f2(X[N], V_1[N - 1] + H[N - 1] * k31, V_2[N - 1] + H[N - 1] * k32);

        V_1[N] = V_1[N - 1] + H[N - 1] / 6 * (k11 + 2 * k21 + 2 * k31 + k41);
        V_2[N] = V_2[N - 1] + H[N - 1] / 6 * (k12 + 2 * k22 + 2 * k32 + k42);

        //k11 = f1(X[N - 1], V_1[N - 1], V_2[N - 1]);
        //k12 = f2(X[N - 1], V_1[N - 1], V_2[N - 1]);
        k21 = f1(X[N - 1] + H[N - 1] / 4, V_1[N - 1] + H[N - 1] / 4 * k11, V_2[N - 1] + H[N - 1] / 4 * k12);
        k22 = f2(X[N - 1] + H[N - 1] / 4, V_1[N - 1] + H[N - 1] / 4 * k11, V_2[N - 1] + H[N - 1] / 4 * k12);
        k31 = f1(X[N - 1] + H[N - 1] / 4, V_1[N - 1] + H[N - 1] / 4 * k21, V_2[N - 1] + H[N - 1] / 4 * k22);
        k32 = f2(X[N - 1] + H[N - 1] / 4, V_1[N - 1] + H[N - 1] / 4 * k21, V_2[N - 1] + H[N - 1] / 4 * k22);
        k41 = f1(X[N - 1] + H[N - 1] / 2, V_1[N - 1] + H[N - 1] / 2 * k31, V_2[N - 1] + H[N - 1] / 2 * k32);
        k42 = f2(X[N - 1] + H[N - 1] / 2, V_1[N - 1] + H[N - 1] / 2 * k31, V_2[N - 1] + H[N - 1] / 2 * k32);

        x0_5 = X[N - 1] + H[N - 1] / 2;
        v0_51 = V_1[N - 1] + H[N - 1] / 12 * (k11 + 2 * k21 + 2 * k31 + k41);
        v0_52 = V_2[N - 1] + H[N - 1] / 12 * (k12 + 2 * k22 + 2 * k32 + k42);

        k11 = f1(x0_5, v0_51, v0_52);
        k12 = f2(x0_5, v0_51, v0_52);
        k21 = f1(x0_5 + H[N - 1] / 4, v0_51 + H[N - 1] / 4 * k11, v0_52 + H[N - 1] / 4 * k12);
        k22 = f2(x0_5 + H[N - 1] / 4, v0_51 + H[N - 1] / 4 * k11, v0_52 + H[N - 1] / 4 * k12);
        k31 = f1(x0_5 + H[N - 1] / 4, v0_51 + H[N - 1] / 4 * k21, v0_52 + H[N - 1] / 4 * k22);
        k32 = f2(x0_5 + H[N - 1] / 4, v0_51 + H[N - 1] / 4 * k21, v0_52 + H[N - 1] / 4 * k22);
        k41 = f1(X[N], v0_51 + H[N - 1] / 2 * k31, v0_52 + H[N - 1] / 2 * k32);
        k42 = f2(X[N], v0_51 + H[N - 1] / 2 * k31, v0_52 + H[N - 1] / 2 * k32);

        V2_1[N] = v0_51 + H[N - 1] / 12 * (k11 + 2 * k21 + 2 * k31 + k41);
        V2_2[N] = v0_52 + H[N - 1] / 12 * (k12 + 2 * k22 + 2 * k32 + k42);

        S1 = (V2_1[N] - V_1[N]) / 15.0;
        S2 = (V2_2[N] - V_2[N]) / 15.0;
        S = sqrt(S1 * S1 + S2 * S2);
        OLP[N] = S * 16;

        if ((eps / (16.0)) <= abs(S) && abs(S) <= eps) {
            H[N] = H[N - 1];
        }
        else if (abs(S) < (eps / (16.0))) {
            H[N] = 2 * H[N - 1];
            C2[N]++;
        }
        else {
            H[N - 1] = H[N - 1] / 2;
            C1[N]++;
            N--;
        }

        if (X[N] > x - b) {
            if (X[N] >= x) {
                N--;
            }
            break;
        }
        if (N == Nmax) break;
        N++;
    }
    return N;
}

// для системы ОДУ из двух уравнений
int rk4_method(double h, int Nmax, double x0, double u10, double u20, double x, double b,
    double (*f1)(double, double, double), double (*f2)(double, double, double), double* X, double* V_1, double* V_2,
    double* V2_1, double* V2_2) {
    int N = 1;

    X[0] = x0;
    V_1[0] = u10;
    V_2[0] = u20;
    double x0_5, v0_51, v0_52;
    double k11, k12, k21, k22, k31, k32, k41, k42;
    while (true) {
        X[N] = X[N - 1] + h;

        k11 = f1(X[N - 1], V_1[N - 1], V_2[N - 1]);
        k12 = f2(X[N - 1], V_1[N - 1], V_2[N - 1]);
        k21 = f1(X[N - 1] + h / 2, V_1[N - 1] + h / 2 * k11, V_2[N - 1] + h / 2 * k12);
        k22 = f2(X[N - 1] + h / 2, V_1[N - 1] + h / 2 * k11, V_2[N - 1] + h / 2 * k12);
        k31 = f1(X[N - 1] + h / 2, V_1[N - 1] + h / 2 * k21, V_2[N - 1] + h / 2 * k22);
        k32 = f2(X[N - 1] + h / 2, V_1[N - 1] + h / 2 * k21, V_2[N - 1] + h / 2 * k22);
        k41 = f1(X[N], V_1[N - 1] + h * k31, V_2[N - 1] + h * k32);
        k42 = f2(X[N], V_1[N - 1] + h * k31, V_2[N - 1] + h * k32);

        V_1[N] = V_1[N - 1] + h / 6 * (k11 + 2 * k21 + 2 * k31 + k41);
        V_2[N] = V_2[N - 1] + h / 6 * (k12 + 2 * k22 + 2 * k32 + k42);

        //k11 = f1(X[N - 1], V_1[N - 1], V_2[N - 1]);
        //k12 = f2(X[N - 1], V_1[N - 1], V_2[N - 1]);
        k21 = f1(X[N - 1] + h / 4, V_1[N - 1] + h / 4 * k11, V_2[N - 1] + h / 4 * k12);
        k22 = f2(X[N - 1] + h / 4, V_1[N - 1] + h / 4 * k11, V_2[N - 1] + h / 4 * k12);
        k31 = f1(X[N - 1] + h / 4, V_1[N - 1] + h / 4 * k21, V_2[N - 1] + h / 4 * k22);
        k32 = f2(X[N - 1] + h / 4, V_1[N - 1] + h / 4 * k21, V_2[N - 1] + h / 4 * k22);
        k41 = f1(X[N - 1] + h / 2, V_1[N - 1] + h / 2 * k31, V_2[N - 1] + h / 2 * k32);
        k42 = f2(X[N - 1] + h / 2, V_1[N - 1] + h / 2 * k31, V_2[N - 1] + h / 2 * k32);

        x0_5 = X[N - 1] + h / 2;
        v0_51 = V_1[N - 1] + h / 12 * (k11 + 2 * k21 + 2 * k31 + k41);
        v0_52 = V_2[N - 1] + h / 12 * (k12 + 2 * k22 + 2 * k32 + k42);

        k11 = f1(x0_5, v0_51, v0_52);
        k12 = f2(x0_5, v0_51, v0_52);
        k21 = f1(x0_5 + h / 4, v0_51 + h / 4 * k11, v0_52 + h / 4 * k12);
        k22 = f2(x0_5 + h / 4, v0_51 + h / 4 * k11, v0_52 + h / 4 * k12);
        k31 = f1(x0_5 + h / 4, v0_51 + h / 4 * k21, v0_52 + h / 4 * k22);
        k32 = f2(x0_5 + h / 4, v0_51 + h / 4 * k21, v0_52 + h / 4 * k22);
        k41 = f1(X[N], v0_51 + h / 2 * k31, v0_52 + h / 2 * k32);
        k42 = f2(X[N], v0_51 + h / 2 * k31, v0_52 + h / 2 * k32);

        V2_1[N] = v0_51 + h / 12 * (k11 + 2 * k21 + 2 * k31 + k41);
        V2_2[N] = v0_52 + h / 12 * (k12 + 2 * k22 + 2 * k32 + k42);

        if (X[N] > x - b) {
            if (X[N] >= x) {
                N--;
            }
            break;
        }
        if (N == Nmax) break;
        N++;
    }
    return N;
}