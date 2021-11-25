OBJS = jsoncpp.o apiv2Test.o 

all: apiv2Test hashtest

%.o: %.cpp
	gcc -c $<

apiv2Test: $(OBJS)
	g++ -o $@ $(OBJS) -lcurl -lcrypto -I.

clean: 
	rm -f apiv2Test *.o