CMAKEARGS+=$(if $(D),-DCMAKE_BUILD_TYPE=Debug,-DCMAKE_BUILD_TYPE=Release)
CMAKEARGS+=$(if $(COV),-DOPT_COV=1)
CMAKEARGS+=$(if $(PROF),-DOPT_PROF=1)
CMAKEARGS+=$(if $(LIBCXX),-DOPT_LIBCXX=1)

all:
	cmake -B build . $(CMAKEARGS)
	$(MAKE) -C build $(if $(V),VERBOSE=1)

llvm:
	CC=clang CXX=clang++ cmake -B build . $(CMAKEARGS)
	$(MAKE) -C build $(if $(V),VERBOSE=1)



oldall: dots intersect

%:%.cpp
	clang++ $^ -o $@ -I/usr/local/include/ -std=c++1z -F/usr/local/lib  -framework QtGui -framework QtCore

dots: dots.cpp

intersect: intersect.cpp


clean:
	$(RM) -r build CMakeFiles CMakeCache.txt CMakeOutput.log

