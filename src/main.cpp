#include "OrderBook.h"
#include <cassert>

int main() {
    OrderBook book;

    Order buy {100, 50, "AAPL", 1, Side::BUY};
    Order sell {90, 50, "AAPL", 2, Side::SELL};

    book.add(buy);
    book.add(sell);

    book.execute(sell);

    book.printBook();

    // assert(book.getVolumeAtPrice(100) == 0); // BUY order filled
    // assert(book.getVolumeAtPrice(90) == 0);  // SELL order filled

    return 0;
}
