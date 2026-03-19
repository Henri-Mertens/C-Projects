#pragma once
#include "../option.h"

double calcebrt(const Option &opt);
double calcert(const Option &opt);

double delta(const Option &opt);
double simulateDelta(const Option &opt, double steps, double simulations);

double gamma(const Option &opt);
double simulateGamma(const Option &opt, double steps, double simulations);

double theta(const Option &opt);
double simulateTheta(const Option &opt, double steps, double simulations);

double vega(const Option &opt);
double simulateVega(const Option &opt, double steps, double simulations);

double rho(const Option &opt, double steps, double simulations);
double simulateRho(const Option &opt, double steps, double simulations);

double phi(const Option &opt);
double simulatePhi(const Option &opt, double steps, double simulations);