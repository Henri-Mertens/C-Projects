#pragma once
#include "Usings.h"
#include "Order.h"

class OrderModify {
private:
    OrderId orderid_;
    Price price_;
    Quantity quantity_;
    Side side_;
public:
    OrderModify(OrderId orderid, Price price, Quantity quantity, Side side)
        : orderid_(orderid), price_(price), quantity_(quantity), side_(side) {}

    OrderId getOrderId() const { return orderid_; }
    Price getPrice() const { return price_; }
    Quantity getQuantity() const { return quantity_; }
    Side getSide() const { return side_; }

    OrderPointer ToOrderPointer(OrderType ordertype) const {
        return std::make_shared<Order>(ordertype, getOrderId(), getSide(), getPrice(), getQuantity());
    }
};