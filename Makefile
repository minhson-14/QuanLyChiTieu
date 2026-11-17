# Makefile bien dich phan mem Quan ly chi tieu ca nhan bang g++ (C++17)
# Cach dung:
#   make        -> bien dich ra QuanLyChiTieu.exe
#   make run    -> bien dich (neu can) va chay chuong trinh
#   make clean  -> xoa file thuc thi

CXX      := g++
# -static giup file .exe chay doc lap, khong can DLL cua MinGW di kem
CXXFLAGS := -std=c++17 -Wall -Wextra -static -static-libgcc -static-libstdc++ -Iinclude
SRC      := $(wildcard src/*.cpp) main.cpp
OUT      := QuanLyChiTieu.exe

all: $(OUT)

$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

run: $(OUT)
	./$(OUT)

clean:
	-rm -f $(OUT)

.PHONY: all run clean
