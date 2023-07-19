.PHONY:all
all:calserver calclient

calclient:CalculatorClient.cc
	g++ -o $@ $^ -std=c++11
calserver:CalculatorServer.cc
	g++ -o $@ $^ -std=c++11 -lpthread

.PHONY:clean 
clean:
	rm -f calclient calserver
