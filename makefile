CXXFLAGS=-std=c++11
CXX=g++

MatLangCompiler.exe: CompilerProject.o
	$(CXX) $(CXXFLAGS) CompilerProject.o -o MatLangCompiler.exe


CompilerProject.o: CompilerProject.cpp
	$(CXX) $(CXXFLAGS) -c CompilerProject.cpp 
	

