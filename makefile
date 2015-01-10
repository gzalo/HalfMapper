SOURCES=$(wildcard *.cpp *.rc)
OBJECTS=$(addprefix bin/,$(addsuffix .o,$(notdir $(basename $(SOURCES)))))
LDFLAGS= -lmingw32 -lsdlmain -lsdl -lglew32 -lopengl32 -lglu32

G++FLAGS= -O2 -Wall -pedantic -Wextra

all: hlmapper.exe

hlmapper.exe: $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

bin/%.o: %.cpp
	$(CXX) -c $< -o $@ $(G++FLAGS)

bin/%.o: %.rc
	windres $< -O coff -o $@
	
clean:
	@del bin\*.o
	@del hlmapper.exe
	
run:
	hlmapper
	
.PHONY: clean run	