#pragma once
#include "../option.h"

double simulateEuropean(const Option& opt, OptionsSide side, int steps, int simulations, double price, double time);
double simulateAmerican(const Option& opt, OptionsSide side, double steps, double simulations);
double simulateAsian(const Option& opt, OptionsSide side, double steps, double simulations);
double simulateBermuda(const Option& opt, OptionsSide side, int steps, int simulations);
double simulateOptionsPrice(const Option& opt, int steps, int simulations);
