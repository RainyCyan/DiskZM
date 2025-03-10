# 定义编译器
CXX = g++

# 定义编译选项
CXXFLAGS = -g -gdwarf-4 -gstrict-dwarf -fvar-tracking-assignments -std=c++17

# 定义目标文件
TARGET = testhm

# 定义源文件
SRC = test_hm.cpp

# 默认目标
all: $(TARGET)

# 链接目标文件
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# 清理生成的文件
clean:
	rm -f $(TARGET)
	rm -f *.test_data
	rm -f *.test_index
	rm -f test.metadata