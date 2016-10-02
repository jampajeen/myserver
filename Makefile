PROGRAM = myserver
#INCLUDE = -I/usr/local/gcc-6.2.0/include/c++/6.2.0  -I/usr/local/opt/openssl/include
LIBDIRS = -L/usr/local/gcc-6.2.0/lib
LIBS = -levent -lcrypto -lssl
CXXSOURCES = $(wildcard *.cpp)
SRCS = 	$(CXXSOURCES)
CXXOBJECTS = $(SRCS:.cpp=.o) #pugixml.o 
LWS_LIB_OBJECTS := $(filter-out server.o connection.o, $(CXXOBJECTS))	
LWS_LIB = server.a
CXXFLAGS = -Ddebug -std=c++11 -arch x86_64 -fPIC -g -Ddebug -Wno-format #-O2 -Wall -I/usr/include -std=c++11
CXX = /usr/local/gcc-6.2.0/bin/g++-6.2.0
LDFLAGS = $(LIBDIRS) $(LIBS)

all: $(PROGRAM)
$(PROGRAM): $(CXXOBJECTS)
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -o $@ $(CXXOBJECTS)
#pugixml.o:
#	$(CXX) $(LDFLAGS) $(CXXFLAGS) -c -o pugixml.o pugixml/pugixml.cpp
clean:
	$(RM) -f $(CXXOBJECTS) $(PROGRAM) *.so
run:
	./$(PROGRAM)