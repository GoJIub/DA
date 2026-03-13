CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -g
LDFLAGS :=

TARGET := avl
SRCS := main.cpp tree.cpp
OBJS := $(SRCS:.cpp=.o)

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)
