GCC = g++
OBJS = main.o window.o infowindow.o cmdwindow.o command.o 

all: main

%.o: %.cpp
	$(GCC) -c $<

main: $(OBJS)
	$(GCC) -o $@ $(OBJS) -lncurses -lpthread -I.

clean:
	rm -f main $(OBJS)