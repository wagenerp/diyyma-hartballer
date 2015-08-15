
CXXFLAGS= \
	-DGLEW_STATIC -DIL_STATIC_LIB -DGLEW_NO_GLU -std=c++11 -pthread \
	-I.

LFLAGS= -ldiyyma -lSDL2 -ldevil -lopengl32 -lstdc++

TARGETS= hartballer.exe

all: $(TARGETS)

clean:
	del $(TARGETS) *.o

hartballer.exe: hartballer.o
	$(CXX) $^ -o$@ $(LFLAGS)