CPP = g++
BOOSTPATH = -I /lib/boost_1_59_0 
LDFLAGS = ./lib/libboost_system.a ./lib/libboost_thread.a -lpthread

.SUFFIXES : .cpp .o
.cpp.o :
	$(CPP) -c $(BOOSTPATH) $<

ALL = ap_agent manager hostap.o

all: $(ALL)

ap_agent: ap_agent.o hostap.o
	$(CPP) $(BOOSTPATH) -o $@ $? $(LDFLAGS) 
#hostap.o: hostap.cpp
#	$(CPP) -c hostap.cpp

manager: manager.o
	$(CPP) $(BOOSTPATH) -o $@ $? $(LDFLAGS)

clean :
	rm -rf *.o $(ALL)

