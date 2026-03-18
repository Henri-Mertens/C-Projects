#pragma once
#include <map>
#include <unordered_map>
#include <numeric>
#include "Order.h"
#include "OrderModify.h"
#include "Trade.h"
#include "OrderBookLevelInfos.h"



class OrderBook{
    private:
       struct OrderEntry{
        OrderPointer order_{nullptr};
        OrderPointers::iterator location_;

       };

       std::map<Price, OrderPointers, std::greater<Price>> bids_;
       std::map<Price, OrderPointers, std::less<Price>> asks_;
       std::unordered_map<OrderId, OrderEntry> orders_;

       bool canMatch(Side side, Price price) const{

            if(side==Side::Buy){
                if(asks_.empty()){
                    return false;
                }
                const auto& [bestAskPrice, _] = *asks_.begin();

                return price>= bestAskPrice;
            }

            else if (side == Side::Sell){
                if(bids_.empty()){
                    return false;
                }
                const auto& [bestBidPrice, _] = *bids_.begin();
                return price<=bestBidPrice;
            }
            else{
                throw std::logic_error(std::format("Side is not valid, only valid sides are Sell/Buy."));
            }
        }

       //Used for fill or kill, entire order shoule be bale to be filled.
       bool canMatchFullOrder(Side side, Price price, Quantity quantity) const{

            Quantity availibleQuantity = 0;

            if(side==Side::Buy){
                if(asks_.empty()){
                    return false;
                }
                
                for(const auto& [BestAskPrice, askorderpointers]: asks_){
                    if(BestAskPrice>price){break;}

                    for(const auto& orders : askorderpointers){
                        availibleQuantity+=orders->getRemainingQuantity();
                    }
                    if(availibleQuantity>=quantity){
                        return true;
                    }
                }

                return false;
            }

            else if(side==Side::Sell){
                if(bids_.empty()){
                    return false;
                }
                
                for(const auto& [BestBidPrice, bidorderpointers]: bids_){
                    if(BestBidPrice<price){break;}

                    for(const auto& orders : bidorderpointers){
                        availibleQuantity+=orders->getRemainingQuantity();
                    }
                    if(availibleQuantity>=quantity){
                        return true;
                    }
                }

                return false;
            }
            else{
                throw std::logic_error(std::format("Side is not valid, only valid sides are Sell/Buy."));
            }
        }

        Trades MatchOrders(){
            Trades trades;
            trades.reserve(orders_.size());

            while(true){
                if(bids_.empty() || asks_.empty()){
                    break;
                }

                // Take all the orders at their best price 
                auto& [bestBidPrice, bestBidOrders] = *bids_.begin();
                auto& [bestAskPrice, bestAskOrders] = *asks_.begin();

                if(bestBidPrice<bestAskPrice){
                    break;
                }

                while(bestBidOrders.size() && bestAskOrders.size()){

                    // Of the orders at their best price, take the first one (chronoglogically)
                    auto bidOrder = bestBidOrders.front();
                    auto askOrder = bestAskOrders.front();

                    Quantity quantity = std::min(bidOrder->getRemainingQuantity(), askOrder->getRemainingQuantity());

                    bidOrder->fill(quantity);
                    askOrder->fill(quantity);

                    if(bidOrder->isFilled()){
                        bestBidOrders.pop_front();
                        orders_.erase(bidOrder->getOrderId());
                        
                    }

                    if(askOrder->isFilled()){
                        bestAskOrders.pop_front();
                        orders_.erase(askOrder->getOrderId());
                    }

                    if(bestBidOrders.empty()){
                        bids_.erase(bidOrder->getPrice());
                    }

                    if(bestAskOrders.empty()){
                        asks_.erase(askOrder->getPrice());
                    }

                    TradeInfo tradeinfobid = TradeInfo(bidOrder->getOrderId(), bidOrder->getPrice(), quantity);
                    TradeInfo tradeinfoask = TradeInfo(askOrder->getOrderId(), askOrder->getPrice(), quantity);

                    Trade trade = Trade(tradeinfobid, tradeinfoask);
                    trades.push_back(trade);



                }

            }


            if(!bids_.empty()){

                auto& [bestBidPrice, bestBidOrders] = *bids_.begin();

                if (bestBidOrders.front()->getOrdertype() == OrderType::FillAndKill){
                    CancelOrder(bestBidOrders.front()->getOrderId());
                }

            }

            if(!asks_.empty()){

                auto& [bestAskPrice, bestAskOrders] = *asks_.begin();
                if (bestAskOrders.front()->getOrdertype() == OrderType::FillAndKill){
                    // bestAskOrders.remove(orderpointer);
                    CancelOrder(bestAskOrders.front()->getOrderId());
                }

            }

            return trades;
        }


    
    public:

        Trades addOrder(OrderPointer order){

            if(orders_.contains(order->getOrderId())){
                return {};
            }


            if(order->getOrdertype() == OrderType::Market){
                if(order->getSide()==Side::Buy && !asks_.empty()){
                    const auto& [worstAsk, _] = *asks_.rbegin();

                    order->MarketToGoodTillCancel(worstAsk);
                }
                else if (order->getSide()==Side::Sell && !bids_.empty()){
                    const auto& [worstBid, _] = *bids_.rbegin();
                    order->MarketToGoodTillCancel(worstBid);
                }

                else{
                    return {};
                }
            }

            if (order->getOrdertype() == OrderType::FillOrKill && !canMatchFullOrder(order->getSide(), order->getPrice(), order->getRemainingQuantity())){
                return {};
            }

            if(order->getOrdertype() == OrderType::FillAndKill && !canMatch(order->getSide(), order->getPrice())){
                return {};
            }

            OrderPointers::iterator iterator;
            if(order->getSide()==Side::Buy){
                auto& orders = bids_[order->getPrice()];
                orders.push_back(order);
                iterator = std::next(orders.begin(), orders.size()-1);

            }
            else if (order->getSide()==Side::Sell){
                auto& orders = asks_[order->getPrice()];
                orders.push_back(order);
                iterator = std::next(orders.begin(), orders.size()-1);
            }

            orders_.insert({order->getOrderId(), OrderEntry(order, iterator)});
            return MatchOrders();

        }

        void CancelOrder(OrderId orderid){
            if(!orders_.contains(orderid)){
                return ;
            }

            const auto& [order, iterator] = orders_.at(orderid);
            orders_.erase(orderid);

            if(order->getSide()==Side::Sell){
                auto& sellorders = asks_.at(order->getPrice());
                sellorders.erase(iterator);

                if(sellorders.empty()){
                    asks_.erase(order->getPrice());
                }
            }

            else if(order->getSide()==Side::Buy){
                auto& buyorders = bids_.at(order->getPrice());
                buyorders.erase(iterator);

                if(buyorders.empty()){
                    bids_.erase(order->getPrice());
                }
            }

        }

        Trades MatchOrder(OrderModify order){
            if(!orders_.contains(order.getOrderId())){
                return {};
            }

            const auto& [existingOrder, _] = orders_.at(order.getOrderId());
            CancelOrder(order.getOrderId());
            return addOrder(order.ToOrderPointer(existingOrder->getOrdertype()));
        }

        std::size_t Size() const {return orders_.size();}

        OrderBookLevelInfos getOrderInfos() const{
            LevelInfos bidinfos, askinfos;

            bidinfos.reserve(orders_.size());
            askinfos.reserve(orders_.size());


            auto CreateLevelInfos = [] (Price price, const OrderPointers& orders) {

                return LevelInfo{ price, std::accumulate(orders.begin(), orders.end(), (Quantity)0, 
                    [] (std::size_t runningSum, const OrderPointer& order)
                    {return runningSum + order->getRemainingQuantity(); })
                    };

            };

            for(const auto& [price, orders] : bids_){
                bidinfos.push_back(CreateLevelInfos(price, orders));
            }

            for(const auto& [price, orders] : asks_){
                askinfos.push_back(CreateLevelInfos(price, orders));
            } 
            
            return OrderBookLevelInfos{bidinfos, askinfos};

        }
};