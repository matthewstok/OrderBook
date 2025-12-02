# OrderBook
A simple C++ Limit Order Book simulator  
by: **Matthew Stoklosinski**

## Overview

This project implements a basic financial exchange-style order book that supports:

- Adding new orders (Buy or Sell)
- Canceling existing orders
- Executing trades by matching incoming orders against the book
- Keeping an index of active orders for fast lookup

The system stores bids and asks in price-sorted containers and maintains FIFO rules so that earlier orders at a price level receive priority.

This project is designed as an educational simulation of the core mechanics of exchange matching engines, including price levels, time priority, and efficient lookups of resting orders.

## Features

- Separate Bid and Ask books
- FIFO execution priority within each price level
- Supports:
  - Adding new orders
  - Canceling existing orders
  - Executing incoming marketable orders
- O(1) order lookup through a dedicated index
- Price levels stored in ordered maps for natural price sorting
- Deques maintain FIFO scheduling for each price level

## Directory Structure

/src → .cpp source files
/include → header files
Makefile → build script
README.md → project description


## Requirements

- C++17 or newer
- Standard Template Library (STL)
- A C++ compiler such as:
  - `g++`
  - `clang++`

## Building With Make

This project uses a Makefile to compile the code.  

From the project root:

1. Open a terminal in the project directory.
2. To clean up object files and the executable:
    make clean
3. Run the command:
    make
4. After building, you can run the program from the terminal:
    ./orderbook