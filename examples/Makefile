include ../Makefile.am
LDLIBS+=-lneural++

all:
	g++ -Wall -o learnAdd learnAdd.cpp ${LDLIBS}
	g++ -Wall -o doAdd doAdd.cpp ${LDLIBS}
	g++ -Wall -o networkForSumsAndSubtractions networkForSumsAndSubtractions.cpp ${LDLIBS}
	g++ -Wall -o adderFromString adderFromString.cpp ${LDLIBS}

clean:
	rm -f learnAdd
	rm -f doAdd
	rm -f networkForSumsAndSubtractions
	rm -f adderFromString
