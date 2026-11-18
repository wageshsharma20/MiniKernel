CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
SRC = main.cpp Process.cpp MemoryManager.cpp Scheduler.cpp Kernel.cpp Shell.cpp

minikernel: $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o minikernel

clean:
	rm -f minikernel
