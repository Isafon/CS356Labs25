# Isa Fontana 835-760-710, CS356 - PA1 (C++): This builds cipher.cpp into an executable named cipher
# why is VSCode finishing the comments I make for me? Help?
CXX      := g++
CXXFLAGS := -std=gnu++17 -O2 -Wall -Wextra -pedantic
LDFLAGS  :=

TARGET   := cipher
SRCS     := cipher.cpp
OBJS     := $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)