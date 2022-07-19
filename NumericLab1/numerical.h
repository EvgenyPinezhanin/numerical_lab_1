#ifndef NUMERICAL_H
#define NUMERICAL_H

#include<cmath>
#include<vector>
#include<stdexcept>
#include<iostream>

using namespace std;

class rk4_method {
private:
	double (*f2)(double, double);
	vector<double (*)(double, double, double)> f3;

	double x0, u0, ud0;
	double h0;
	int Nmax;
	double b, Egr, E;

	bool isControl;
	bool isSystem;

public:
	rk4_method(double (*_f1)(double, double), double _x0 = 0.0, double _u0 = 0.0, 
		double _h0 = 0.001, int _Nmax = 10000, double _b = 1.0, double _Egr = 0.000001, 
		double _E = 0.00000001, bool _control = true);
	rk4_method(double (*_f1)(double, double, double), double (*_f2)(double, double, double), double _x0 = 0.0,
		double _u0 = 0.0, double _ud0 = 0.0, double _h0 = 0.001, int _Nmax = 10000, double _b = 1.0,
		double _Egr = 0.000001, double _E = 0.00000001, bool _control = true);

	void setFunc(double (*_f1)(double, double));
	void setFunc(double (*_f1)(double, double, double), double (*_f2)(double, double, double));
	void setSC(double _x0, double _u0, double _ud0 = 0.0);
	void setH0(double _h0);
	void setControl(int _Nmax, double _b, double _Egr, double _E = 0.00000001, bool _control = false);

	void solve(vector<double>& X, vector<double>& H, vector<double>& V, vector<double>& V2,
			   vector<double>& OLP = vector<double>(), vector<int>& C1 = vector<int>(), vector<int>& C2 = vector<int>());
	void solve(vector<double>& X, vector<double>& H, vector<vector<double>>& V, vector<vector<double>>& V2,
			   vector<double>& OLP = vector<double>(), vector<int>& C1 = vector<int>(), vector<int>& C2 = vector<int>());
};

#endif