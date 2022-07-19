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

    // Очистка vector's
    X.clear(); H.clear();
    V.clear(); V2.clear();
    OLP.clear();
    C1.clear(); C2.clear();

    // Объявление и инициализация необходимых переменных и vector's
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

    // Основной цикл метода
    while (true) {
        // Вычисление x n+1
        xN_1 = xN + hN;

        // Проверка выхода x n+1 за правую границу
        if (xN_1 > b) {
            hN -= xN_1 - (b - Egr / 2.0);
            H[N] = hN;
            step_d++;
            continue;
        }

        // Вычисление коэффициентов k1, k2, k3, k4 для v n+1 
        k[0] = f2(xN, vN);
        k[1] = f2(xN + hN / 2.0, vN + hN / 2.0 * k[0]);
        k[2] = f2(xN + hN / 2.0, vN + hN / 2.0 * k[1]);
        k[3] = f2(xN_1, vN + hN * k[2]);

        // Вычисление v n+1
        vN_1 = vN + hN / 6.0 * (k[0] + 2.0 * k[1] + 2.0 * k[2] + k[3]);

        // Вычисление коэффициентов k1, k2, k3, k4 для v n+0.5
        //k[0] = f2(xN, vN);
        k[1] = f2(xN + hN / 4.0, vN + hN / 4.0 * k[0]);
        k[2] = f2(xN + hN / 4.0, vN + hN / 4.0 * k[1]);
        k[3] = f2(xN + hN / 2.0, vN + hN / 2.0 * k[2]);

        // Вычисление x n+0.5 и v n+0.5
        x0_5 = xN + hN / 2.0;
        v0_5 = vN + hN / 12.0 * (k[0] + 2.0 * k[1] + 2.0 * k[2] + k[3]);

        // Вычисление коэффициентов k1, k2, k3, k4 для v n+1 удв
        k[0] = f2(x0_5, v0_5);
        k[1] = f2(x0_5 + hN / 4.0, v0_5 + hN / 4.0 * k[0]);
        k[2] = f2(x0_5 + hN / 4.0, v0_5 + hN / 4.0 * k[1]);
        k[3] = f2(xN_1, v0_5 + hN / 2.0 * k[2]);

        // Вычисление v n+1 удв
        v2N_1 = v0_5 + hN / 12.0 * (k[0] + 2.0 * k[1] + 2.0 * k[2] + k[3]);

        // Вычисление контрольного значения
        S = (v2N_1 - vN_1) / 15.0;

        // Контроль локальной погрешности
        if (isControl) {
            if (abs(S) > E) {
                hN /= 2.0;
                H[N] = hN;
                step_d++;
                continue;
            } else if (abs(S) < E / 16.0) {
                hN *= 2.0;
                step_i++;
            }
        }

        // Добавление данных полученных на текущем шаге в vector's
        X.push_back(xN_1); xN = xN_1;
        V.push_back(vN_1); vN = vN_1;
        V2.push_back(v2N_1);
        H.push_back(hN);
        OLP.push_back(S * 16.0);
        C1.push_back(step_d); step_d = 0;
        C2.push_back(step_i); step_i = 0;

        // Контроль выхода на правую границу
        if (xN_1 >= b - Egr) {
            N++;
            break;
        }

        // Проверка достижения максимального количества шагов
        N++;
        if (N == Nmax) {
            break;
        }
    }
}
void rk4_method::solve(vector<double>& X, vector<double>& H, vector<vector<double>>& V, vector<vector<double>>& V2,
    vector<double>& OLP, vector<int>& C1, vector<int>& C2) {
    if (!isSystem) throw logic_error("Error function solve");

    // Очистка vector's
    X.clear(); H.clear();
    V.clear(); V.resize(2);
    V2.clear(); V2.resize(2);
    OLP.clear();
    C1.clear(); C2.clear();

    // Объявление и инициализация необходимых переменных и vector's
    vector<vector<double>> k(5);
    for (int i = 0; i < 5; i++) {
        k[i].resize(2);
    }
    double S[2];
    double S_norm;
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

    // Основной цикл метода
    while (true) {
        // Вычисление x n+1
        xN_1 = xN + hN;

        // Проверка выхода x n+1 за правую границу
        if (xN_1 > b) {
            hN -= xN_1 - (b - Egr / 2.0);
            H[N] = hN;
            step_d++;
            continue;
        }

        // Вычисление коэффициентов k1, k2, k3, k4 для v n+1 
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

        // Вычисление v n+1
        for (int i = 0; i < 2; i++) {
            vN_1[i] = vN[i] + hN / 6.0 * (k[0][i] + 2.0 * k[1][i] + 2.0 * k[2][i] + k[3][i]);
        }

        // Вычисление коэффициентов k1, k2, k3, k4 для v n+0.5
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

        // Вычисление x n+0.5 и v n+0.5
        x0_5 = xN + hN / 2.0;
        for (int i = 0; i < 2; i++) {
            v0_5[i] = vN[i] + hN / 12.0 * (k[0][i] + 2.0 * k[1][i] + 2.0 * k[2][i] + k[3][i]);
        }

        // Вычисление коэффициентов k1, k2, k3, k4 для v n+1 удв
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

        // Вычисление v n+1 удв
        for (int i = 0; i < 2; i++) {
            v2N_1[i] = v0_5[i] + hN / 12.0 * (k[0][i] + 2.0 * k[1][i] + 2.0 * k[2][i] + k[3][i]);
        }

        // Вычисление контрольного значения
        for (int i = 0; i < 2; i++) {
            S[i] = (v2N_1[i] - vN_1[i]) / 15.0;
        }
        
        // Вычисление нормы контрольного значения (норма Чебышевского)
        S_norm = max(abs(S[0]), abs(S[1]));

        // Контроль локальной погрешности
        if (isControl) {
            if (S_norm > E) {
                hN /= 2.0;
                H[N] = hN;
                step_d++;
                continue;
            }
            else if (S_norm < E / 16.0) {
                hN *= 2.0;
                step_i++;
            }
        }

        // Добавление данных полученных на текущем шаге в vector's
        X.push_back(xN_1); xN = xN_1;
        V[0].push_back(vN_1[0]);  vN[0] = vN_1[0];
        V[1].push_back(vN_1[1]);  vN[1] = vN_1[1];
        H.push_back(hN);
        V2[0].push_back(v2N_1[0]);
        V2[1].push_back(v2N_1[1]);
        OLP.push_back(S_norm * 16.0);
        C1.push_back(step_d); step_d = 0;
        C2.push_back(step_i); step_i = 0;

        // Контроль выхода на правую границу
        if (xN_1 >= b - Egr) {
            N++;
            break;
        }

        // Проверка достижения максимального количества шагов
        N++;
        if (N == Nmax) break;
    }
}