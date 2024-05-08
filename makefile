CXX=g++

CXXFLAGS=-pthread -Wl,-rpath='$$ORIGIN'

PROGRAM=react_server

LIBRARY=st_reactor

LIBRARY_SRC=$(LIBRARY).cpp

all: $(PROGRAM)

$(PROGRAM): $(LIBRARY).so $(PROGRAM).cpp
	$(CXX) -o $@ $(PROGRAM).cpp $(LIBRARY).so $(CXXFLAGS)

$(LIBRARY).so: $(LIBRARY_SRC)
	$(CXX) -fPIC -shared -o $@ $^

clean:
	rm -f $(PROGRAM) $(LIBRARY).so

