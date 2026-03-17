#pragma once

enum class OptionsType{ 
    American, 
    European,
    // https://quant.stackexchange.com/questions/76315/closed-form-equation-for-geometric-asian-call-option
    Asian,
    Bermuda,
};

enum class OptionsSide{
    Call,
    Put,
};

enum class UnderLyingType{
    Stock,
    FuturesWithFuturesSettlement,
    FuturesWithStockSettlement,
    Forex,
};
