#pragma once
#include <stdexcept>
#include <format>
#include "Usings.h"
#include "Constants.h"
#include "OrderType.h"
#include "Side.h"

class Order {
private:
    OrderType ordertype_;
    Quantity initalQuantity_;
    Quantity remainingQuantity_;
    OrderId orderid_;
    Side side_;
    Price price_;
public:
    Order(OrderType ordertype, OrderId orderid, Side side, Price price, Quantity quantity)
        : orderid_(orderid), side_(side), initalQuantity_(quantity), 
          remainingQuantity_(quantity), ordertype_(ordertype), price_(price) {}

    Order(OrderId orderid, Side side, Quantity quantity)
        : Order(OrderType::Market, orderid, side, Constants::InvalidPrice, quantity) {}

    OrderId getOrderId() const { return orderid_; }
    Side getSide() const { return side_; }
    Price getPrice() const { return price_; }
    OrderType getOrdertype() const { return ordertype_; }
    Quantity getInitialQuantity() const { return initalQuantity_; }
    Quantity getRemainingQuantity() const { return remainingQuantity_; }
    Quantity getFilledQuantity() const { return getInitialQuantity() - getRemainingQuantity(); }
    bool isFilled() const { return getRemainingQuantity() == 0; }

    void fill(Quantity quantity) {
        if (quantity > getRemainingQuantity())
            throw std::logic_error(std::format("Order {} cannot be filled for more than its remaining quantity", getOrderId()));
        remainingQuantity_ -= quantity;
    }

    void MarketToGoodTillCancel(Price price) {
        if (ordertype_ != OrderType::Market)
            throw std::logic_error("Can only convert market to GTC with this function");
        ordertype_ = OrderType::GoodTillCancel;
        price_ = price;
    }
};