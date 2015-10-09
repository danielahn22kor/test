CPP = g++
CPPFLAGS = -I ./boost_1_59_0 
LDFLAGS = libboost_system.a libboost_thread.a -lpthread

.SUFFIXES : .cpp .o
.cpp.o :
	$(CPP) -c $(CPPFLAGS) $<

ALL = ap_agent_boost manager_boost hostap.o

all: $(ALL)

ap_agent_boost: ap_agent_boost.o hostap.o
	$(CPP) $(CPPFLAGS) -o $@ $? $(LDFLAGS) 
#hostap.o: hostap.cpp
#	$(CPP) -c hostap.cpp

manager_boost: manager_boost.o
	$(CPP) $(CPPFLAGS) -o $@ $? $(LDFLAGS)

clean :
	rm -rf *.o $(ALL)

