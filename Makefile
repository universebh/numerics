CCFLAGS=--std=gnu++98 -pedantic -Wall -Werror -ggdb3

numerics: numerics.cpp node.h define.h test.h numint.h mcint.h
	g++ -o $@ $(CCFLAGS) $?

clean:
	rm -f numerics  *~ ./testcases/*~ 
