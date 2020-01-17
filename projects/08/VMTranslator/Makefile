CXX=g++
SRCS=VMTranslator.cpp CodeWriter.cpp Parser.cpp
OBJS=$(addsuffix .o, $(basename $(SRCS)))

TARGET=VMTranslator

.PHONY: clean

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -o $@

clean:
	rm $(TARGET) $(OBJS)