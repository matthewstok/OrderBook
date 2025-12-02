CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2 -Iinclude

SRCS = src/main.cpp src/OrderBook.cpp
OBJS = $(SRCS:.cpp=.o)

TARGET = orderbook

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
