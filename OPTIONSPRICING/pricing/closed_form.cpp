#include "closed_form.h"
#include "../math_utils.h"
#include <cmath>

double calculateEuropeanCall(const Option& opt, double S, double time) {
    double d1 = calculateD1(opt, S, time);
    double d2 = calculateD2(opt, time);
    return S * std::exp((opt.getB()-opt.getRiskFreeRate())*time) * normalCDF(d1) - opt.getStrikePrice() * std::exp(-opt.getRiskFreeRate() * time) * normalCDF(d2);
}

double calculateEuropeanCall(const Option& opt) {
    double d1 = calculateD1(opt);
    double d2 = calculateD2(opt);
    return opt.getUnderlyingPrice() * std::exp((opt.getB()-opt.getRiskFreeRate())*opt.getTimeTillExpiration()) * normalCDF(d1) - opt.getStrikePrice() * std::exp(-opt.getRiskFreeRate() * opt.getTimeTillExpiration()) * normalCDF(d2);
}

double calculateEuropeanPut(const Option& opt, double S, double time) {
    double d1 = calculateD1(opt, S, time);
    double d2 = calculateD2(opt, time);
    return (opt.getStrikePrice()*std::exp(-opt.getRiskFreeRate()*time)*normalCDF(-d2)) - (S*(std::exp((opt.getB()-opt.getRiskFreeRate())*time))*normalCDF(-d1));
}

double calculateEuropeanPut(const Option& opt) {
    double d1 = calculateD1(opt);
    double d2 = calculateD2(opt);
    return (opt.getStrikePrice()*std::exp(-opt.getRiskFreeRate()*opt.getTimeTillExpiration())*normalCDF(-d2)) - (opt.getUnderlyingPrice()*(std::exp((opt.getB()-opt.getRiskFreeRate())*opt.getTimeTillExpiration()))*normalCDF(-d1));
}

double findCriticalPriceSStarCall(const Option& opt) {
    double low = opt.getStrikePrice();
    double high = opt.getStrikePrice() * 5.0; 
    double tol = 1e-5;
    double T_diff = opt.getTimeTillExpiration() - opt.getTimeTillDividend();

    while ((high - low) > tol) {
        double mid = low + (high - low) / 2.0;
        double holdValue = calculateEuropeanCall(opt, mid - opt.getDividend(), T_diff);
        double exerciseValue = mid - opt.getStrikePrice();
        
        if (exerciseValue > holdValue) {
            high = mid;
        } else {
            low = mid;
        }
    }
    return low + (high - low) / 2.0;
}

double calculateAmericanCall(const Option& opt){
    double T_diff = opt.getTimeTillExpiration() - opt.getTimeTillDividend();
    //should we exercise?
    //only if dividend 
    if (opt.getDividend() <= opt.getStrikePrice() * (1.0 - std::exp(-opt.getRiskFreeRate() * T_diff))){
        double S_prime = opt.getUnderlyingPrice() - opt.getDividend() * std::exp(-opt.getRiskFreeRate() * opt.getTimeTillDividend());
        return calculateEuropeanCall(opt, S_prime, opt.getTimeTillExpiration());
    }
         
    double S_star = findCriticalPriceSStarCall(opt);
    double S_prime = opt.getUnderlyingPrice() - opt.getDividend() * std::exp(-opt.getRiskFreeRate() * opt.getTimeTillDividend());
    
    double rho = std::sqrt(opt.getTimeTillDividend() / opt.getTimeTillExpiration());
    
    double a1 = (std::log(S_prime / opt.getStrikePrice()) + (opt.getRiskFreeRate() + opt.getVolatility() * opt.getVolatility() / 2) * opt.getTimeTillExpiration()) / 
                (opt.getVolatility() * std::sqrt(opt.getTimeTillExpiration()));
    double a2 = a1 - opt.getVolatility() * std::sqrt(opt.getTimeTillExpiration());
    
    double b1 = (std::log(S_prime / S_star) + (opt.getRiskFreeRate() + opt.getVolatility() * opt.getVolatility() / 2) * opt.getTimeTillDividend()) / 
                (opt.getVolatility() * std::sqrt(opt.getTimeTillDividend()));
    double b2 = b1 - opt.getVolatility() * std::sqrt(opt.getTimeTillDividend());

    double term1 = S_prime * normalCDF(b1);
    double term2 = S_prime * bivariateNormalCDF(a1, -b1, -rho);
    double term3 = opt.getStrikePrice() * std::exp(-opt.getRiskFreeRate() * opt.getTimeTillExpiration()) * bivariateNormalCDF(a2, -b2, -rho);
    double term4 = (opt.getStrikePrice() - opt.getDividend()) * std::exp(-opt.getRiskFreeRate() * opt.getTimeTillDividend()) * normalCDF(b2);

    return term1 + term2 - term3 - term4;
}

double findCriticalPriceSStarPut(const Option& opt) {
    double low = 1e-5; 
    double high = opt.getStrikePrice();
    double tol = 1e-5;
    double T_diff = opt.getTimeTillExpiration() - opt.getTimeTillDividend();

    while ((high - low) > tol) {
        double mid         = low + (high - low) / 2.0;
        double holdValue   = calculateEuropeanPut(opt, mid - opt.getDividend(), T_diff);
        double exerciseValue = opt.getStrikePrice() - mid;

        if (exerciseValue > holdValue) {
            low = mid;   
        } else {
            high = mid;
        }
    }
    return low + (high - low) / 2.0;
}

// Barone Whaley
double calculateAmericanPut(const Option& opt){
    double S_star = findCriticalPriceSStarPut(opt);

    if(opt.getUnderlyingPrice()>S_star){
        double priceEuropean = calculateEuropeanPut(opt);
        double M = 2*opt.getRiskFreeRate()/(opt.getVolatility()*opt.getVolatility());
        double N = 2*opt.getB()/(opt.getVolatility()*opt.getVolatility());
        double q = (-(N-1)-(std::sqrt((N-1)*(N-1)+4*M)))/2;

        double A = (opt.getStrikePrice()-S_star-calculateEuropeanPut(opt, S_star, opt.getTimeTillExpiration()))/std::pow(S_star, q);

        return priceEuropean + A*std::pow(opt.getUnderlyingPrice(), q);
    }
    else{
        return opt.getStrikePrice() - opt.getUnderlyingPrice();
    }
}

double calculateOptionsPriceClosedForm(const Option& opt){
    if(opt.getOptionsSide()==OptionsSide::Call){
        if(opt.getOptionsType()==OptionsType::European || (opt.getOptionsType()==OptionsType::American && opt.getDividend()==0)){
            return calculateEuropeanCall(opt);
        }
        else if(opt.getOptionsType()==OptionsType::American){
            return calculateAmericanCall(opt);
        }
    }
    else{
        if(opt.getOptionsType()==OptionsType::European || (opt.getOptionsType()==OptionsType::American && opt.getDividend()==0)){
            return calculateEuropeanPut(opt);
        }
        else{
            return calculateAmericanPut(opt);
        }
    }
    return 0;
}
