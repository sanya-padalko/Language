CXX = g++

CXXFLAGS = -std=c++17 -Wall -Wextra -Iinc

TARGET = my_comp

SRCS = $(wildcard src/*.cpp)

all: build run

build:
	@echo "Компилируем проект..."
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

build_debug:
	@echo "Компилируем проект..."
	$(CXX) $(CXXFLAGS) -DDEBUG $(SRCS) -o $(TARGET)

run:
	@echo "Запуск скомпилированного компилятора:"
	./$(TARGET)

debug:
	g++ -g -std=c++17 -Wall -Wextra -Iinc src/*.cpp -o my_comp
	lldb ./my_comp

clean:
	@echo "Очистка..."
	rm -f $(TARGET)