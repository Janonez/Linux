# mysignal:mysignal.cc
# 	g++ -o $@ $^ -std=c++11;
# .PHONY:clean
# clean:
# 	rm -f mysignal
.PHONY:all
all:mykill loop
mykill:mykill.cc
	g++ -o $@ $^ -std=c++11;
loop:loop.cc
	g++ -o $@ $^ -std=c++11;
.PHONY:clean
clean:
	rm -f mykill loop