CXX = g++

CXXFLAGS = -std=c++17 -g -Wall -Wextra -Iinc

TARGET = my_comp

SRCS = $(wildcard src/*.cpp)

all: build run

build:
	@echo "Компилируем проект..."
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

run:
	@echo "Запуск скомпилированного компилятора:"
	./$(TARGET)

clean:
	@echo "Очистка..."
	rm -f $(TARGET)