#pragma once
#include <memory>   // ← add this for shared_ptr
#include <list>     // ← add this for list
#include <vector>

class Order;
using OrderPointer = std::shared_ptr<Order>;
using OrderPointers = std::list<OrderPointer>;

using Price = std::int32_t;
using Quantity = std::uint32_t;
using OrderId = std::uint64_t;
using OrderIds = std::vector<OrderId>;


class Order;
// Orders will be saved in different datastructures so use reference to ptrs to avoid copies
using OrderPointer = std::shared_ptr<Order>;
using OrderPointers = std::list<OrderPointer>;