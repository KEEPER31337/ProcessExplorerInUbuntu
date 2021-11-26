GCC = g++
OBJS = main.o window.o infowindow.o command.o cmdwindow.o

all: main

%.o: %.cpp
	$(GCC) -c $<

main: $(OBJS)
	$(GCC) -o $@ $(OBJS) -lncurses -lpthread -I. --std=c++11

clean:
	rm -f main $(OBJS)