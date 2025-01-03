CXX = g++
CXXFLAGS = -std=c++20 -I/opt/homebrew/Cellar/sfml/3.0.0/include -I./src/graphics -I./src/physics
LDFLAGS = -L/opt/homebrew/Cellar/sfml/3.0.0/lib -lsfml-graphics -lsfml-window -lsfml-system

TARGET = sfml

# Source file(s)
SRCS = main.cpp

# Object file(s)
OBJS = $(SRCS:.cpp=.o)

# Build the target
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(CXXFLAGS) -o $(TARGET) $(LDFLAGS)

# Compile each .cpp file to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(TARGET) $(OBJS)
