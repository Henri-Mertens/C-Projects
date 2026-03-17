#include "option.h"
#include <stdexcept>
#include <vector>
void Option::init(){
    if (underlyingType_ == UnderLyingType::Forex)
        throw std::logic_error("please provide the foreign interest rate aswell");

    else if (underlyingType_==UnderLyingType::FuturesWithFuturesSettlement){
            riskFreeRate_=0;
            b_=0;
    }
    else if(underlyingType_==UnderLyingType::FuturesWithStockSettlement){
            b_=0;
    }
    else if(underlyingType_==UnderLyingType::Stock){
            b_=riskFreeRate_; 
    }
    else throw std::logic_error("Invalid UnderLying type");
}


// Standard option, with underlying being a dividend paying stock
Option::Option(double time, double vol, double rate, double underlyingprice, double strike, double dividend, double timetilldividend,
        OptionsType type, OptionsSide side, UnderLyingType underlyingtype) :
timeTillExpiration_(time/252), volatility_(vol), 
riskFreeRate_(rate), underlyingPrice_(underlyingprice), 
strikePrice_(strike), dividend_(dividend), timeTillDividend_(timetilldividend/252),
type_(type), side_(side), underlyingType_(underlyingtype) 
{
    init();
}

//Option when underlying doesnt pay dividends
Option::Option(double time, double vol, double rate, double underlyingprice, double strike,
        OptionsType type, OptionsSide side, UnderLyingType underlyingtype) :
timeTillExpiration_(time/252), volatility_(vol), 
riskFreeRate_(rate), underlyingPrice_(underlyingprice), 
strikePrice_(strike),
type_(type), side_(side), underlyingType_(underlyingtype) 
{
    init();
}

//Options when udnerlying is forex -> add foreigninterest
Option::Option(double time, double vol, double rate, double underlyingprice, double strike, double foreigninterest, 
    OptionsType type, OptionsSide side, UnderLyingType underlyingtype) :
timeTillExpiration_(time/252), volatility_(vol), 
riskFreeRate_(rate), underlyingPrice_(underlyingprice), 
strikePrice_(strike), foreignInterest_(foreigninterest),
type_(type), side_(side), underlyingType_(underlyingtype) 
{
    b_ = riskFreeRate_-foreignInterest_;
}


// Bermuda option, with underlying being a dividend paying stock
Option::Option(double time, double vol, double rate, double underlyingprice, double strike, 
               double dividend, double timetilldividend, std::vector<double> timetillexcercises,
               OptionsType type, OptionsSide side, UnderLyingType underlyingtype) :
timeTillExpiration_(time/252), volatility_(vol), 
riskFreeRate_(rate), underlyingPrice_(underlyingprice), 
strikePrice_(strike), dividend_(dividend), timeTillDividend_(timetilldividend/252),
timeTillExcercises_(timetillexcercises),
type_(type), side_(side), underlyingType_(underlyingtype) 
{
    init();
}

//Option when we know the price and want to calculate implied vol
Option::Option(double optionsprice, int time, double rate, double underlyingprice, double strike, OptionsType type, OptionsSide side, UnderLyingType underlyingtype) :
timeTillExpiration_(time/252), optionsPrice_(optionsprice), 
riskFreeRate_(rate), underlyingPrice_(underlyingprice), 
strikePrice_(strike),
type_(type), side_(side), underlyingType_(underlyingtype) {}
