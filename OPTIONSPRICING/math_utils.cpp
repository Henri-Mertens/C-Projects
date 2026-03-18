#include "math_utils.h"
#include <cmath>
#include "option.h"
#define PI 3.14159265358979323846


// Helper functions
double calculateD1(Option opt){
    return (std::log(opt.getUnderlyingPrice() / opt.getStrikePrice()) + (opt.getB() + opt.getVolatility() * opt.getVolatility() / 2) * opt.getTimeTillExpiration()) / (opt.getVolatility() * std::sqrt(opt.getTimeTillExpiration()));
}
double calculateD1(Option opt, double S, double time){
    return (std::log(S / opt.getStrikePrice()) + (opt.getB() + opt.getVolatility() * opt.getVolatility() / 2) * time) / (opt.getVolatility() * std::sqrt(time));
 
}

double calculateD2(Option opt){
    return calculateD1(opt) - opt.getVolatility() * std::sqrt(opt.getTimeTillExpiration());
}
double calculateD2(Option opt, double time){
    return calculateD1(opt) - opt.getVolatility() * std::sqrt(time);
}

double normalPDF(double x) {
    static const double INV_SQRT_2PI = 1.0 / std::sqrt(2.0 * PI);
    return INV_SQRT_2PI * std::exp(-0.5 * x * x);
}




double normalCDF(double value)
{
    return 0.5 * std::erfc(-value * std::sqrt(0.5));
}

//Used for roll geske whaley
double bivariateNormalCDF(double a, double b, double rho) {
    if (rho == 0.0) return normalCDF(a) * normalCDF(b);

    // 10-point Gauss-Legendre nodes/weights on [-1, 1]
    static const double gl_x[] = {
        -0.9739065285, -0.8650633667, -0.6794095683, -0.4333953941, -0.1488743390,
        0.1488743390,  0.4333953941,  0.6794095683,  0.8650633667,  0.9739065285
    };
    static const double gl_w[] = {
        0.0666713443, 0.1494513492, 0.2190863625, 0.2692667193, 0.2955242247,
        0.2955242247, 0.2692667193, 0.2190863625, 0.1494513492, 0.0666713443
    };

    // Integrate dM/dr = phi2(a,b;r) from 0 to rho
    double half = rho / 2.0;
    double sum  = 0.0;
    for (int i = 0; i < 10; i++) {
        double r    = half * (1.0 + gl_x[i]);
        double denom = 2.0 * (1.0 - r * r);
        sum += gl_w[i] * std::exp(-(a*a - 2*r*a*b + b*b) / denom)
                    / (2.0 * PI * std::sqrt(1.0 - r * r));
    }
    return normalCDF(a) * normalCDF(b) + half * sum;
}
