#include "greeks.h"
#include "../math_utils.h"
#include <random>
#include <cmath>
#include "simulation.h"


//calculate e^(b-r)*t
double calcebrt(Option opt){
    return std::exp((opt.getB()-opt.getRiskFreeRate())*opt.getTimeTillExpiration());
}

//calculate e^-r*t
double calcert(Option opt){
    return std::exp(-opt.getRiskFreeRate()*opt.getTimeTillExpiration());
}


double delta(Option opt){
    double d1 = calculateD1(opt);
    double d2 = calculateD2(opt);
    if(opt.getOptionsSide() == OptionsSide::Call){
        return calcebrt(opt)*normalCDF(d1);
    }
    else{
        return calcebrt(opt)*(normalCDF(d1)-1);
    }
}

double gamma(Option opt){
    double d1 = calculateD1(opt);
    double d2 = calculateD2(opt);

    //same for call and puts
    return (calcebrt(opt)*normalPDF(d1))/(opt.getUnderlyingPrice()*opt.getVolatility()*std::sqrt(opt.getTimeTillExpiration()));
}

double theta(Option opt){
        
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

double vega(Option opt){
    //same for calls and puts
    return (opt.getUnderlyingPrice()*calcebrt(opt)*normalPDF(calculateD1(opt))*std::sqrt(opt.getTimeTillExpiration()))/100;
}

double rho(Option opt){
    double b = opt.getB();
    double ert = calcert(opt);
    double X = opt.getStrikePrice();
    double t = opt.getTimeTillExpiration();
    double d2 = calculateD2(opt);

    if(opt.getOptionsSide()==OptionsSide::Call){
        double C = simulateOptionsPrice(opt, 252, 10000);

        if(b==0){
            return -t*C/100;
        }
        else{
            return t*X*ert*normalCDF(d2)/100;
        }

    }
    else{
        double P = simulateOptionsPrice(opt, 252, 10000);

        if(b==0){
            return -t*P/100;
        }
        else{
            return -t*X*ert*normalCDF(-d2)/100;
        }
    }
}

double phi(Option opt){
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


