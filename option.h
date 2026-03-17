#pragma once
#include "enums.h"
#include <vector>
class Option{

    private:
        double timeTillExpiration_;
        double volatility_ = -100;
        double riskFreeRate_;
        double underlyingPrice_;
        double strikePrice_;
        double optionsPrice_;
        double b_;
        double foreignInterest_;
        double dividend_ = 0;
        double timeTillDividend_ = 0;
        std::vector<double> timeTillExcercises_;

        OptionsType type_;
        OptionsSide side_;
        UnderLyingType underlyingType_;

        void init();

    public:
        Option(double time, double vol, double rate, double underlyingprice, double strike, double dividend, double timetilldividend,
                OptionsType type, OptionsSide side, UnderLyingType underlyingtype);
        Option(double time, double vol, double rate, double underlyingprice, double strike,
                OptionsType type, OptionsSide side, UnderLyingType underlyingtype);
        Option(double time, double vol, double rate, double underlyingprice, double strike, double foreigninterest, 
            OptionsType type, OptionsSide side, UnderLyingType underlyingtype);
        Option(double optionsprice, int time, double rate, double underlyingprice, double strike, OptionsType type, OptionsSide side, UnderLyingType underlyingtype);

        Option(double time, double vol, double rate, double underlyingprice, double strike, double dividend, double timetilldividend, std::vector<double> timetillexcercises,
                OptionsType type, OptionsSide side, UnderLyingType underlyingtype);

        double getTimeTillExpiration() const {return timeTillExpiration_;}
        double getVolatility() const {return volatility_;}
        double getRiskFreeRate() const {return riskFreeRate_;}
        double getUnderlyingPrice() const {return underlyingPrice_;}
        double getStrikePrice() const {return strikePrice_;}
        double getB() const {return b_;}
        double getDividend() const {return dividend_;}
        double getTimeTillDividend() const {return timeTillDividend_;}
        std::vector<double> getTimeTillExcercises() const {return timeTillExcercises_;}
        OptionsType getOptionsType() const {return type_;}
        OptionsSide getOptionsSide() const {return side_;}
        UnderLyingType getUnderlyingType() const {return underlyingType_;}

        void setTimeTillExpiration(int timetillexpiration){timeTillExpiration_=timetillexpiration;}
        void setVolatility(double vol){volatility_=vol;}
        void setRiskFreeRate(double riskfreerate){riskFreeRate_=riskfreerate;}
        void setUnderlyingPrice(double price){underlyingPrice_=price;}
        void setStrikePrice(double strike){strikePrice_=strike;}
        void setB(double b){b_=b;}
        void setTimeTillExcercises(std::vector<double> exc){timeTillExcercises_=exc;}
        void setOptionsType(OptionsType type){type_=type;}
        void setOptionsSide(OptionsSide side){side_=side;}
        void setUnderlyingType(UnderLyingType underlyingtype){underlyingType_=underlyingtype;}
        
};
