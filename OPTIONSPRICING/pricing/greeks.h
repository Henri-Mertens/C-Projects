#pragma once
#include "../option.h"

// Delta, Gamma, Vega, Theta, Rho
// TODO: Implement Greeks


double calcebrt(Option opt);
double calcert(Option opt);
double delta(Option opt);
double gamma(Option opt);
double theta(Option opt);
double vega(Option opt);
double rho(Option opt);

double phi(Option opt);