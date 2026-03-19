#include "greeks.h"
#include "../math_utils.h"
#include <random>
#include <cmath>
#include "simulation.h"
#include <stdexcept>
#include <format>
	 

#define SMALL 1.0


//calculate e^(b-r)*t
double calcebrt(const Option& opt){
    return std::exp((opt.getB()-opt.getRiskFreeRate())*opt.getTimeTillExpiration());
}

//calculate e^-r*t
double calcert(const Option& opt){
    return std::exp(-opt.getRiskFreeRate()*opt.getTimeTillExpiration());
}


double delta(const Option& opt){
    double d1 = calculateD1(opt);
    if(opt.getOptionsSide() == OptionsSide::Call){
        return calcebrt(opt)*normalCDF(d1);
    }
    else{
        return calcebrt(opt)*(normalCDF(d1)-1);
    }
}

double simulateDelta(const Option& opt, double steps, double simulations){
    Option copy = opt;
    double S0 = copy.getUnderlyingPrice();
    double P0 = simulateOptionsPrice(copy, steps, simulations);

    copy.setUnderlyingPrice(S0 + SMALL);
    double P1 = simulateOptionsPrice(copy, steps, simulations);

    return (P1-P0)/SMALL;
}


double gamma(const Option& opt){
    double d1 = calculateD1(opt);

    //same for call and puts
    return (calcebrt(opt)*normalPDF(d1))/(opt.getUnderlyingPrice()*opt.getVolatility()*std::sqrt(opt.getTimeTillExpiration()));
}

double simulateGamma(const Option& opt, double steps, double simulations){

    Option copy = opt;

    double S0 = copy.getUnderlyingPrice();
    double D0 = simulateDelta(copy, steps, simulations);

    copy.setUnderlyingPrice(S0+SMALL);

    double D1 = simulateDelta(copy, steps, simulations);

    return (D1-D0)/SMALL;

}

double theta(const Option& opt){
        
    double d1 = calculateD1(opt);
    double d2 = calculateD2(opt);
    double ebrt = calcebrt(opt);

    double temp1 = (-opt.getUnderlyingPrice()*ebrt*normalPDF(d1)*opt.getVolatility())/
                    (2*std::sqrt(opt.getTimeTillExpiration()));
    double temp2 = (opt.getB()-opt.getRiskFreeRate())*opt.getUnderlyingPrice()*ebrt*normalCDF(d1);

    if(opt.getOptionsSide()==OptionsSide::Call){
        double temp3 = opt.getRiskFreeRate()*opt.getStrikePrice()*calcert(opt)*normalCDF(d2);
        return (temp1-temp2-temp3)/365;
    }
    else{
        double temp3 = opt.getRiskFreeRate()*opt.getStrikePrice()*calcert(opt)*normalCDF(-d2);
        return (temp1+temp2+temp3)/365;
    }
}

double simulateTheta(const Option& opt, double steps, double simulations){

    Option copy = opt;


    double T0 = copy.getTimeTillExpiration();
    double P0 = simulateOptionsPrice(copy, steps, simulations);

    copy.setTimeTillExpiration(T0 - 1.0/252.0);
    double P1 = simulateOptionsPrice(copy, steps, simulations);

    return (P1-P0);
}

double vega(const Option& opt){
    //same for calls and puts
    return (opt.getUnderlyingPrice()*calcebrt(opt)*normalPDF(calculateD1(opt))*std::sqrt(opt.getTimeTillExpiration()))/100;
}

double simulateVega(const Option& opt, double steps, double simulations){

    Option copy = opt;
    double V0 = copy.getVolatility();
    double P0 = simulateOptionsPrice(copy, steps, simulations);

    // 1 percentage point increase
    copy.setVolatility(V0+0.01);

    double P1 = simulateOptionsPrice(copy, steps, simulations);

    return (P1-P0);

}


double rho(const Option& opt, double steps, double simulations){
    double b = opt.getB();
    double ert = calcert(opt);
    double X = opt.getStrikePrice();
    double t = opt.getTimeTillExpiration();
    double d2 = calculateD2(opt);

    if(opt.getOptionsSide()==OptionsSide::Call){
        double C = simulateOptionsPrice(opt, steps, simulations);

        if(b==0){
            return -t*C/100;
        }
        else{
            return t*X*ert*normalCDF(d2)/100;
        }

    }
    else{
        double P = simulateOptionsPrice(opt, steps, simulations);

        if(b==0){
            return -t*P/100;
        }
        else{
            return -t*X*ert*normalCDF(-d2)/100;
        }
    }
}

double simulateRho(const Option& opt, double steps, double simulations){

    Option copy = opt;
    double R0 = copy.getRiskFreeRate();
    double P0 = simulateOptionsPrice(copy, steps, simulations);

    // 1 percentage point increase
    copy.setRiskFreeRate(R0 + 0.01);

    double P1 = simulateOptionsPrice(copy, steps, simulations);

    return (P1-P0);

}




double phi(const Option& opt){
    double d1 = calculateD1(opt);
    double t  = opt.getTimeTillExpiration();
    double S  = opt.getUnderlyingPrice();
    double ebrt = calcebrt(opt);

    if(opt.getOptionsSide() == OptionsSide::Call){
        return (-t * S * ebrt * normalCDF(d1))  / 100;
    }
    else{
        return ( t * S * ebrt * normalCDF(-d1)) / 100;
    }
}

double simulatePhi(const Option& opt, double steps, double simulations){

    if(opt.getUnderlyingType() != UnderLyingType::Forex){throw std::logic_error(std::format("Cannot calculate phi for underlying that is not forex"));}
    Option copy = opt;
    double R0 = copy.getForeignInterestRate();
    double P0 = simulateOptionsPrice(copy, steps, simulations);

    // 1 percentage point increase
    copy.setForeignInterestRate(R0 + 0.01);

    double P1 = simulateOptionsPrice(copy, steps, simulations);

    return (P1-P0);

}
