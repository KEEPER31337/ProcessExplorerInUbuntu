OBJS = jsoncpp.o apiv2Test.o 

all: apiv2Test hashtest

%.o: %.cpp
	gcc -c $<

apiv2Test: $(OBJS)
	g++ -o $@ $(OBJS) -lcurl -lcrypto -I.

hashtest: hashtest.cpp
	g++ -o $@ $< -lcrypto

clean: 
	rm -f apiv2Test *.o