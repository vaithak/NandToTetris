CXX=g++
CXXFLAGS=-Iincludes/
SRCS=main.cpp src/Code.cpp src/Parser.cpp src/SymbolTable.cpp
OBJS=$(addsuffix .o, $(basename $(SRCS)))

TARGET=assembler

.PHONY: clean

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -o $@

clean:
	rm $(TARGET) $(OBJS)