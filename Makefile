CXX = g++

CXXFLAGS = -std=c++20 -I/opt/homebrew/Cellar/sfml/3.0.0/include

LDFLAGS = -L/opt/homebrew/Cellar/sfml/3.0.0/lib -lsfml-graphics -lsfml-window -lsfml-system

TARGET = sfml

SRCS = main.cpp

$(TARGET): $(SRCS)
	$(CXX) $(SRCS) $(CXXFLAGS) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)