#include "simulation.h"
#include "closed_form.h"
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>

double simulateEuropean(const Option& opt, OptionsSide side, int steps, int simulations, double price, double time){
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution<double> d{0.0, 1.0};

    double dt = time/steps;
    std::vector<std::vector<double>> pricePaths;
    double totalOptionsPrice = 0.0; 

    for(int i=0; i<simulations; i++){
        std::vector<double> v;
        double S0 = price;

        for(int j=0; j<steps; j++){
            double z = d(gen); 

            S0 = S0*std::exp((opt.getRiskFreeRate()-(0.5*opt.getVolatility()*opt.getVolatility()))*dt + (opt.getVolatility()*std::sqrt(dt)*z));
            v.push_back(S0);
        }
        pricePaths.push_back(v);

        if(side==OptionsSide::Call){totalOptionsPrice+=std::max((S0-opt.getStrikePrice()), 0.0);}
        else totalOptionsPrice+=std::max((opt.getStrikePrice()-S0), 0.0);
    }

    return std::exp(-opt.getRiskFreeRate()*time)*(totalOptionsPrice/simulations);
}


double simulateAmerican(const Option& opt, OptionsSide side,  int steps, int simulations){
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution<double> d{0.0, 1.0};
    double totalOptionsPrice = 0.0; 
    std::vector<std::vector<double>> pricePaths;

    double dt = opt.getTimeTillExpiration()/steps;
    
    if(opt.getTimeTillDividend()>opt.getTimeTillExpiration()){return simulateEuropean(opt, side, steps, simulations, opt.getUnderlyingPrice(), opt.getTimeTillExpiration());}


    for(int i = 0; i<simulations; i++){
        double S0 = opt.getUnderlyingPrice();
        std::vector<double> v;

        //simulate till we hit dividend date
        for(int j=0; j<(int)(opt.getTimeTillDividend()/dt); j++){
            double z = d(gen);
            S0 = S0 * std::exp((opt.getRiskFreeRate() - 0.5 * opt.getVolatility() * opt.getVolatility()) * dt + opt.getVolatility() * std::sqrt(dt) * z);
            v.push_back(S0);
        }
        pricePaths.push_back(v);

        if(side == OptionsSide::Put){

            double currPrice = calculateEuropeanPut(opt, S0, opt.getTimeTillExpiration()-opt.getTimeTillDividend());
            double timevalue = (currPrice - std::max(0.0, opt.getStrikePrice()-S0));

            //only excercise early if ime value of option < interest that can be earned on it
            if(timevalue< (std::max(0.0,opt.getStrikePrice()- S0)*std::exp(opt.getRiskFreeRate()*(opt.getTimeTillExpiration()-opt.getTimeTillDividend()))) - std::max(0.0,opt.getStrikePrice()- S0)){
                totalOptionsPrice += std::max(0.0, (opt.getStrikePrice()-S0)*std::exp(-opt.getRiskFreeRate()*(opt.getTimeTillDividend())));
            }
            else totalOptionsPrice += std::max(0.0, calculateEuropeanPut(opt, S0-opt.getDividend(), opt.getTimeTillExpiration()-opt.getTimeTillDividend())) * std::exp(-opt.getRiskFreeRate()*opt.getTimeTillDividend());
        }

        else{
            double currPrice = calculateEuropeanCall(opt, S0, opt.getTimeTillExpiration()-opt.getTimeTillDividend());
            double timevalue = (currPrice - std::max(0.0, S0-opt.getStrikePrice()));

            if(opt.getDividend() > timevalue && S0 > opt.getStrikePrice()){
                totalOptionsPrice += std::max(0.0, (S0-opt.getStrikePrice())*std::exp(-opt.getRiskFreeRate()*(opt.getTimeTillDividend())));
            }
            else totalOptionsPrice += std::max(0.0, calculateEuropeanCall(opt, S0-opt.getDividend(), opt.getTimeTillExpiration()-opt.getTimeTillDividend())) * std::exp(-opt.getRiskFreeRate()*opt.getTimeTillDividend());

        }


    }

    return totalOptionsPrice/simulations;
}


double simulateAsian(const Option& opt, OptionsSide side, int steps, int simulations){
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution<double> d{0.0, 1.0};

    double dt = opt.getTimeTillExpiration()/steps;
    std::vector<std::vector<double>> pricePaths;
    double totalOptionsPrice = 0.0;

    for(int i=0; i<simulations; i++){
        double totalUnderlyingPrice = 0.0;
        double avgPrice = 0.0; 
        std::vector<double> v;
        double S0 = opt.getUnderlyingPrice();

        for(int j=0; j<steps; j++){
            double z = d(gen); 

            S0 = S0*std::exp((opt.getRiskFreeRate()-(0.5*opt.getVolatility()*opt.getVolatility()))*dt + (opt.getVolatility()*std::sqrt(dt)*z));
            totalUnderlyingPrice+=S0;
            v.push_back(S0);
        }
        avgPrice = totalUnderlyingPrice/steps;
        pricePaths.push_back(v);

        if(side==OptionsSide::Call){
            totalOptionsPrice +=  std::max(0.0, avgPrice-opt.getStrikePrice());
        }
        else totalOptionsPrice += std::max(0.0, opt.getStrikePrice()- avgPrice);
    }

    return std::exp(-opt.getRiskFreeRate()*opt.getTimeTillExpiration())*(totalOptionsPrice/simulations);
}

double simulateBermuda(const Option& opt, OptionsSide side, int steps, int simulations){
    std::random_device rd{};
    std::mt19937 gen{rd()};
    std::normal_distribution<double> d{0.0, 1.0};

    double dt = opt.getTimeTillExpiration()/steps;
    std::vector<std::vector<double>> pricePaths;
    double totalOptionsPrice = 0.0;
    std::vector<double> exctimes = opt.getTimeTillExcercises();


    for(int i=0; i<simulations; i++){

        double S0 = opt.getUnderlyingPrice();
        bool exercised = false;

        //keep track of index of possible excercise times
        int k = 0;
        std::vector<double> v;

        for(int j=0; j<steps; j++){
            double z = d(gen);
            S0 = S0 * std::exp((opt.getRiskFreeRate() - 0.5 * opt.getVolatility() * opt.getVolatility()) * dt + opt.getVolatility() * std::sqrt(dt) * z);
            v.push_back(S0);

            //check if current step might be the optimal to excercise our option
            if(k < (int)exctimes.size() && j*dt < exctimes[k] && (j+1)*dt >= exctimes[k]){
                double texc = exctimes[k];

                k++;
                //reuse american logic
                if(side == OptionsSide::Put){

                    //to avoid recursive monte carlo, use european put to approx value
                    double currPrice = calculateEuropeanPut(opt, S0, opt.getTimeTillExpiration()-texc);
                    double timevalue = (currPrice - std::max(0.0, opt.getStrikePrice()-S0));

                    //if time value of the put is less then interest that can be earned if you exercise -> exercise
                    if(timevalue< (std::max(0.0,opt.getStrikePrice()- S0)*std::exp(opt.getRiskFreeRate()*(opt.getTimeTillExpiration()-texc))) - std::max(0.0,opt.getStrikePrice()- S0)){
                        totalOptionsPrice += std::max((opt.getStrikePrice()-S0), 0.0)*std::exp(-opt.getRiskFreeRate()*texc);
                        exercised=true;
                        break;
                    }
                    else {
                        continue;
                    }
                }

                else{
                    double currPrice = calculateEuropeanCall(opt, S0, opt.getTimeTillExpiration()-texc);
                    double timevalue = (currPrice - std::max(0.0, S0-opt.getStrikePrice()));

                    if(opt.getDividend()>timevalue){
                        totalOptionsPrice += std::max((S0-opt.getStrikePrice()), 0.0)*std::exp(-opt.getRiskFreeRate()*texc);
                        exercised=true;
                        break;
                    }
                    else {
                        continue;
                    }

                }
            }

        }

        if(!exercised){
            if(side == OptionsSide::Put)
                totalOptionsPrice += std::max(0.0, opt.getStrikePrice()-S0) * std::exp(-opt.getRiskFreeRate()*opt.getTimeTillExpiration());
            else
                totalOptionsPrice += std::max(0.0, S0-opt.getStrikePrice()) * std::exp(-opt.getRiskFreeRate()*opt.getTimeTillExpiration());
        }
        pricePaths.push_back(v);
        
    }
    return (totalOptionsPrice/simulations);
}


    



double simulateOptionsPrice(const Option& opt, int steps, int simulations){
    if(opt.getOptionsSide()==OptionsSide::Call){
        if(opt.getOptionsType()==OptionsType::European || (opt.getOptionsType()==OptionsType::American && opt.getDividend()==0)){
            return simulateEuropean(opt, OptionsSide::Call, steps, simulations, opt.getUnderlyingPrice(), opt.getTimeTillExpiration());
        }
        else if(opt.getOptionsType()==OptionsType::American){
            return simulateAmerican(opt, OptionsSide::Call, steps, simulations);
        }

        else if(opt.getOptionsType()==OptionsType::Asian){
            return simulateAsian(opt, OptionsSide::Call, steps, simulations);
        }

        else if(opt.getOptionsType()==OptionsType::Bermuda){
            return simulateBermuda(opt, OptionsSide::Call, steps, simulations);
        }
    }

    else{
        if(opt.getOptionsType()==OptionsType::European || (opt.getOptionsType()==OptionsType::American && opt.getDividend()==0)){
            return simulateEuropean(opt, OptionsSide::Put, steps, simulations, opt.getUnderlyingPrice(), opt.getTimeTillExpiration());
        }
        else if(opt.getOptionsType()==OptionsType::American){
            return simulateAmerican(opt, OptionsSide::Put, steps, simulations);
        }

        else if(opt.getOptionsType()==OptionsType::Asian){
            return simulateAsian(opt, OptionsSide::Put, steps, simulations);
        }

        else if(opt.getOptionsType()==OptionsType::Bermuda){
            return simulateBermuda(opt, OptionsSide::Put, steps, simulations);
        }
    }
    return 0;
}
