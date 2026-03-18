#include "OrderBook.h"
#include <iostream>

int main() {
    OrderBook orderbook;
    const OrderId orderid = 1;
    orderbook.addOrder(std::make_shared<Order>(OrderType::GoodTillCancel, orderid, Side::Buy, 100, 10));
    std::cout << orderbook.Size() << std::endl;
    orderbook.CancelOrder(orderid);
    std::cout << orderbook.Size() << std::endl;
    return 0;
}