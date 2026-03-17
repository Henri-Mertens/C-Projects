#pragma once
#include "../option.h"

double calculateEuropeanCall(const Option& opt, double S, double time);
double calculateEuropeanCall(const Option& opt);
double calculateEuropeanPut(const Option& opt, double S, double time);
double calculateEuropeanPut(const Option& opt);
double findCriticalPriceSStarCall(const Option& opt);
double calculateAmericanCall(const Option& opt);
double findCriticalPriceSStarPut(const Option& opt);
double calculateAmericanPut(const Option& opt);
double calculateOptionsPriceClosedForm(const Option& opt);
