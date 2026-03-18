#pragma once
#include "Usings.h"

struct TradeInfo {
    OrderId orderid_;
    Price price_;
    Quantity quantity_;
};

class Trade {
private:
    TradeInfo bidtrade_;
    TradeInfo asktrade_;
public:
    Trade(const TradeInfo& bidtrade, const TradeInfo& asktrade)
        : bidtrade_(bidtrade), asktrade_(asktrade) {}
    const TradeInfo& getBidTrade() const { return bidtrade_; }
    const TradeInfo& getAskTrade() const { return asktrade_; }
};

using Trades = std::vector<Trade>;