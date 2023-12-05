CXX=clang++ -std=c++17
CFLAGS= -g -O3 `llvm-config --cppflags --ldflags --system-libs --libs all` \
-Wno-unused-function -Wno-unknown-warning-option -fno-rtti

mccomp: mccomp.cpp loaddata.cpp loaddata.h parser.cpp parser.h lexer.cpp lexer.h astnodes.h astnodes.cpp
	$(CXX) mccomp.cpp loaddata.cpp parser.cpp lexer.cpp astnodes.cpp $(CFLAGS) -o mccomp

clean:
	rm -rf mccomp