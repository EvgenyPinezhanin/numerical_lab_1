#include"numerical.h"

rk4_method::rk4_method(double (*_f1)(double, double), double _x0, double _u0, 
    double _h0, int _Nmax, double _b, double _Egr, double _E, bool _control) 
    : f2(_f1), f3(2),x0(_x0), u0(_u0), h0(_h0), Nmax(_Nmax), b(_b), Egr(_Egr), E(_E), isControl(_control), isSystem(false) {}
rk4_method::rk4_method(double (*_f1)(double, double, double), double (*_f2)(double, double, double), double _x0, double _u0,
    double _ud0, double _h0, int _Nmax, double _b, double _Egr, double _E, bool _control)
    : f3(2), x0(_x0), u0(_u0), ud0(_ud0), h0(_h0), Nmax(_Nmax), b(_b), Egr(_Egr), E(_E), isControl(_control), isSystem(true) {
    f3[0] = _f1;
    f3[1] = _f2;
}

void rk4_method::setFunc(double (*_f1)(double, double)) {
    f2 = _f1;
    isSystem = false;
}
void rk4_method::setFunc(double (*_f1)(double, double, double), double (*_f2)(double, double, double)) {
    f3[0] = _f1;
    f3[1] = _f2;
    isSystem = true;
}

void rk4_method::setSC(double _x0, double _u0, double _ud0) {
    x0 = _x0;
    u0 = _u0;
    ud0 = _ud0;
}

void rk4_method::setH0(double _h0) {
    h0 = _h0;
}

void rk4_method::setControl(int _Nmax, double _b, double _Egr, double _E, bool _control) {
    Nmax = _Nmax;
    b = _b;
    Egr = _Egr;
    E = _E;
    isControl = _control;
}

void rk4_method::solve(vector<double>& X, vector<double>& H, vector<double>& V, vector<double>& V2,
    vector<double>& OLP, vector<int>& C1, vector<int>& C2) {
    if (isSystem) throw logic_error("Error function solve");
    X.clear(); H.clear();
    V.clear(); V2.clear();
    OLP.clear();
    C1.clear(); C2.clear();

    vector<double> k(5);
    double S;
    int N = 0;

    double xN = x0, xN_1;
    X.push_back(xN);
    double vN = u0;
    V.push_back(vN);
    double vN_1;
    double hN = h0;
    H.push_back(hN);
    double v2N_1;
    int step_d = 0, step_i = 0;
    double x0_5, v0_5;
    while (true) {
        xN_1 = xN + hN;
        if (xN_1 > b) {
            hN -= xN_1 - (b - Egr / 2.0);
            H.pop_back();
            H.push_back(hN);
            step_d++;
            continue;
        }

        k[0] = f2(xN, vN);
        k[1] = f2(xN + hN / 2.0, vN + hN / 2.0 * k[0]);
        k[2] = f2(xN + hN / 2.0, vN + hN / 2.0 * k[1]);
        k[3] = f2(xN_1, vN + hN * k[2]);

        vN_1 = vN + hN / 6.0 * (k[0] + 2.0 * k[1] + 2.0 * k[2] + k[3]);

        //k[0] = f2(xN, vN);
        k[1] = f2(xN + hN / 4.0, vN + hN / 4.0 * k[0]);
        k[2] = f2(xN + hN / 4.0, vN + hN / 4.0 * k[1]);
        k[3] = f2(xN + hN / 2.0, vN + hN / 2.0 * k[2]);

        x0_5 = xN + hN / 2.0;
        v0_5 = vN + hN / 12.0 * (k[0] + 2.0 * k[1] + 2.0 * k[2] + k[3]);

        k[0] = f2(x0_5, v0_5);
        k[1] = f2(x0_5 + hN / 4.0, v0_5 + hN / 4.0 * k[0]);
        k[2] = f2(x0_5 + hN / 4.0, v0_5 + hN / 4.0 * k[1]);
        k[3] = f2(xN_1, v0_5 + hN / 2.0 * k[2]);

        v2N_1 = v0_5 + hN / 12.0 * (k[0] + 2.0 * k[1] + 2.0 * k[2] + k[3]);

        S = (v2N_1 - vN_1) / 15.0;

        if (isControl) {
            if (abs(S) > E) {
                hN /= 2.0;
                step_d++;
                continue;
            } else if (abs(S) < E / 16.0) {
                hN *= 2.0;
                step_i++;
            }
        }

        X.push_back(xN_1); xN = xN_1;
        V.push_back(vN_1); vN = vN_1;
        V2.push_back(v2N_1);
        H.push_back(hN);
        OLP.push_back(S * 16.0);
        C1.push_back(step_d); step_d = 0;
        C2.push_back(step_i); step_i = 0;

        if (xN_1 >= b - Egr) {
            N++;
            break;
        }
        N++;
        if (N == Nmax) {
            break;
        }
    }
}
void rk4_method::solve(vector<double>& X, vector<double>& H, vector<vector<double>>& V, vector<vector<double>>& V2,
    vector<double>& OLP, vector<int>& C1, vector<int>& C2) {
    if (!isSystem) throw logic_error("Error function solve");
    X.clear(); H.clear();
    V.clear(); V.resize(2);
    V2.clear(); V2.resize(2);
    OLP.clear();
    C1.clear(); C2.clear();

    vector<vector<double>> k(5);
    for (int i = 0; i < 5; i++) {
        k[i].resize(2);
    }
    double S[2];
    double Sabs;
    int N = 0;

    double xN = x0, xN_1;
    X.push_back(xN);
    double vN[2] = { u0, ud0 };
    V[0].push_back(vN[0]);
    V[1].push_back(vN[1]);
    double vN_1[2];
    double hN = h0;
    H.push_back(hN);
    double v2N_1[2];
    int step_d = 0, step_i = 0;
    double x0_5, v0_5[2];
    while (true) {
        xN_1 = xN + hN;
        if (xN_1 > b) {
            hN -= xN_1 - (b - Egr / 2.0);
            step_d++;
            continue;
        }

        for (int i = 0; i < 2; i++) {
            k[0][i] = f3[i](xN, vN[0], vN[1]);
        }
        for (int i = 0; i < 2; i++) {
            k[1][i] = f3[i](xN + hN / 2.0, vN[0] + hN / 2.0 * k[0][0], vN[1] + hN / 2.0 * k[0][1]);
        }
        for (int i = 0; i < 2; i++) {
            k[2][i] = f3[i](xN + hN / 2.0, vN[0] + hN / 2.0 * k[1][0], vN[1] + hN / 2.0 * k[1][1]);
        }
        for (int i = 0; i < 2; i++) {
            k[3][i] = f3[i](xN_1, vN[0] + hN * k[2][0], vN[1] + hN * k[2][1]);
        }

        for (int i = 0; i < 2; i++) {
            vN_1[i] = vN[i] + hN / 6.0 * (k[0][i] + 2.0 * k[1][i] + 2.0 * k[2][i] + k[3][i]);
        }

        //for (int i = 0; i < 2; i++) {
        //    k[0][i] = f3[i](xN, vN[0], vN[1]);
        //}
        for (int i = 0; i < 2; i++) {
            k[1][i] = f3[i](xN + hN / 4.0, vN[0] + hN / 4.0 * k[0][0], vN[1] + hN / 4.0 * k[0][1]);
        }
        for (int i = 0; i < 2; i++) {
            k[2][i] = f3[i](xN + hN / 4.0, vN[0] + hN / 4.0 * k[1][0], vN[1] + hN / 4.0 * k[1][1]);
        }
        for (int i = 0; i < 2; i++) {
            k[3][i] = f3[i](xN + hN / 2.0, vN[0] + hN / 2.0 * k[2][0], vN[1] + hN / 2.0 * k[2][1]);
        }

        x0_5 = xN + hN / 2.0;
        for (int i = 0; i < 2; i++) {
            v0_5[i] = vN[i] + hN / 12.0 * (k[0][i] + 2.0 * k[1][i] + 2.0 * k[2][i] + k[3][i]);
        }

        for (int i = 0; i < 2; i++) {
            k[0][i] = f3[i](xN, v0_5[0], v0_5[1]);
        }
        for (int i = 0; i < 2; i++) {
            k[1][i] = f3[i](x0_5 + hN / 4.0, v0_5[0] + hN / 4.0 * k[0][0], v0_5[1] + hN / 4.0 * k[0][1]);
        }
        for (int i = 0; i < 2; i++) {
            k[2][i] = f3[i](x0_5 + hN / 4.0, v0_5[0] + hN / 4.0 * k[1][0], v0_5[1] + hN / 4.0 * k[1][1]);
        }
        for (int i = 0; i < 2; i++) {
            k[3][i] = f3[i](x0_5 + hN / 2.0, v0_5[0] + hN / 2.0 * k[2][0], v0_5[1] + hN / 2.0 * k[2][1]);
        }

        for (int i = 0; i < 2; i++) {
            v2N_1[i] = v0_5[i] + hN / 12.0 * (k[0][i] + 2.0 * k[1][i] + 2.0 * k[2][i] + k[3][i]);
        }

        for (int i = 0; i < 2; i++) {
            S[i] = (v2N_1[i] - vN_1[i]) / 15.0;
        }
        Sabs = sqrt(S[0] * S[0] + S[1] * S[1]);


        if (isControl) {
            if (Sabs > E) {
                hN /= 2.0;
                step_d++;
                continue;
            }
            else if (Sabs < E / 16.0) {
                hN *= 2.0;
                step_i++;
            }
        }

        X.push_back(xN_1); xN = xN_1;
        V[0].push_back(vN_1[0]);  vN[0] = vN_1[0];
        V[1].push_back(vN_1[1]);  vN[1] = vN_1[1];
        H.push_back(hN);
        V2[0].push_back(v2N_1[0]);
        V2[1].push_back(v2N_1[1]);
        OLP.push_back(Sabs * 16.0);
        C1.push_back(step_d); step_d = 0;
        C2.push_back(step_i); step_i = 0;

        if (xN_1 >= b - Egr) {
            N++;
            break;
        }
        N++;
        if (N == Nmax) break;
    }
}

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
                H[N] -= X[N + 1] - (x - b / 2.0);
            } else {
                break;
            }
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
                h -= X[N + 1] - (x - b / 2.0);
            } else {
                break;
            }
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
                H[N] -= X[N + 1] - (x - b / 2.0);
            } else {
                break;
            }
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
                h -= X[N + 1] - (x - b / 2.0);
            } else {
                break;
            }
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
                h -= X[N + 1] - (x - b / 2.0);
            } else {
                break;
            }
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
                h -= X[N + 1] - (x - b / 2.0);
            } else {
                break;
            }
        }
        if (N == Nmax) break;
        N++;
    }
    return N;
}