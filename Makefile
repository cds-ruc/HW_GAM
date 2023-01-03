CPP = g++
INCLUDE = -I./inc -I./inc/mcs -I./inc/mcs/api -I./gam/include -I./gam/lib/libcuckoo/src -I./gam/lib/libcuckoo/cityhash-1.1.1/src -I./thrid/msgpack-c/include
LIBS = ./gam/src/libgalloc.a -libverbs -lpthread ./gam/lib/libcuckoo/cityhash-1.1.1/src/.libs/libcityhash.a -lboost_thread -lboost_system
CFLAGS += -rdynamic -std=c++17 -O3 -DDHT

%.o: src/%.cpp
	$(CPP) $(CFLAGS) $(INCLUDE) -c -o $@ $^

%.o: src/mcs/%.cpp
	$(CPP) $(CFLAGS) $(INCLUDE) -c -o $@ $^

all: example.o api.o
	cd ./gam/src; make clean; make -j; cd ../../;
	$(CPP) -o $@ $^ $(LIBS)

clean:
	rm *.o benchmark