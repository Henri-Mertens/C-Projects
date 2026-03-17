#pragma once
#include "option.h"
double normalCDF(double value);
double bivariateNormalCDF(double a, double b, double rho);


double calculateD1(Option opt);
double calculateD1(Option opt, double S, double time);

double calculateD2(Option opt);
double calculateD2(Option opt, double time);

double normalPDF(double x);