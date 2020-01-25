CXX=g++
SRCS=lex.yy.c Analyser.cpp CompilationEngine.cpp Tokenizer.cpp
OBJS=$(addsuffix .o, $(basename $(SRCS)))

TARGET=JackAnalyzer

.PHONY: clean

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -o $@

clean:
	rm $(TARGET) $(OBJS)
