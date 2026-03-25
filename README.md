# SLOB: A programmable C++ price-time-priority limit order book

![](https://img.shields.io/badge/-C++-blue?logo=cplusplus)

SLOB (Smart Limit Order Book) is an innovative C++ price-time-priority matching engine. It extends a conventional order book with programmable orders enabling complex conditional order types, and even entire trading strategies to be executed natively within the matching engine. Orders are able to modify or cancel other orders and even themselves.

## Core primitives

SLOB supports two insertable primitives: `order`s and `trigger`s

An `order` represents executable trading intent. Orders expose the following customizable event hooks:

- `on_accepted`
- `on_resting`
- `on_before_match`
- `on_filled`
- `on_canceled`
- `on_deferred_cancel`
- `on_deferred_set_price`
- `on_deferred_set_quantity`

`trigger`s are the main building blocks for stop orders, trailing stops, and other event-driven instructions. They encode reactive behavior tied to changes in the market price, best bid, and best ask. Triggers expose the following customizable event hooks:

- `on_accepted`
- `on_resting`
- `on_triggered`
- `on_canceled`

## Deferrals

Programmable behavior creates a sequencing problem as event handlers may request new operations while the book is already processing another one.

SLOB therefore distinguishes between operations that may execute immediately and operations that must be deferred. Deferred operations are queued and later processed in FIFO order. This preserves deterministic execution and prevents nested event logic from violating price-time priority.

## Build

SLOB requires C++17 or higher.

```bash
g++ -std=c++17 -Wall example.cpp -o example.out
```
## Examples

### Basic example

This example creates an order book and a single order, inserts the order into the book, updates its price and quantity, and then cancels it.

```cpp
#include <iostream>
#include "include/slob.hpp"

int main() {
    slob::book book;
    auto order = std::make_shared<slob::order>(slob::side::ask, 1250, 100);

    book.insert(order);
    std::cout << book << '\n';

    order->set_price(1300);
    std::cout << book << '\n';

    order->set_quantity(200);
    std::cout << book << '\n';

    order->cancel();
    std::cout << book << '\n';

    return 0;
}
```

Output:

```
┌───────────BIDS───────────┬───────────ASKS───────────┐
│          PRC         QTY │          PRC         QTY │
│                          │        1,250         100 │

┌───────────BIDS───────────┬───────────ASKS───────────┐
│          PRC         QTY │          PRC         QTY │
│                          │        1,300         100 │

┌───────────BIDS───────────┬───────────ASKS───────────┐
│          PRC         QTY │          PRC         QTY │
```

### Custom order behavior

Here we define a custom order type that reprices itself on insert to 50 cents above the best ask or 50 cents below the best bid.

```cpp
#include <iostream>
#include "include/slob.hpp"

class reprice_on_insert_order : public slob::order {
public:
    using slob::order::order;

    void on_accepted() override {
        if (get_side() == slob::side::bid) {
            const std::int64_t bid_price = get_book()->get_bid_price();
            set_price(bid_price - 50);
        } else {
            const std::int64_t ask_price = get_book()->get_ask_price();
            set_price(ask_price + 50);
        }
    }
};

int main() {
    slob::book book;
    auto bid = std::make_shared<slob::order>(slob::side::bid, 1200, 100);
    auto ask = std::make_shared<slob::order>(slob::side::ask, 1250, 100);

    book.insert(bid);
    book.insert(ask);

    // set initial price to 0, the order reprices itself in on_accepted
    auto reprice_order = std::make_shared<reprice_on_insert_order>(slob::side::bid, 0, 200);
    book.insert(reprice_order);

    std::cout << book << '\n';
}
```

Output:

```
┌───────────BIDS───────────┬───────────ASKS───────────┐
│          PRC         QTY │          PRC         QTY │
│        1,200         100 │        1,250         100 │
│        1,150         200 │                          │
```

## Todos

- improve/fix documentation
- expand test coverage
- clean up header dependencies

## Contact

`curl -s https://alexrichter.xyz | grep -i -o '[A-Z0-9._%+-]\+@[A-Z0-9.-]\+\.[A-Z]\{2,4\}'`