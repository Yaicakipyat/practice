CXX = g++
# Флаги компиляции: путь к заголовкам SFML, стандарт C++17 (или 11, смотрите код)
CXXFLAGS = -Iinclude -std=c++17 -Wall -march=x86-64 -mno-avx -mno-sse4
# Флаги линковки: библиотеки SFML (порядок важен!)
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

TARGET = galaga_game

# Ищем все .cpp файлы в src и подпапках
SOURCES = $(shell find src -name '*.cpp')
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Правило компиляции .cpp в .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

rebuild: clean all

.PHONY: all clean rebuild
