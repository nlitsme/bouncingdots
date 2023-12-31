CMAKEARGS+=$(if $(D),-DCMAKE_BUILD_TYPE=Debug,-DCMAKE_BUILD_TYPE=Release)
CMAKEARGS+=$(if $(COV),-DOPT_COV=1)
CMAKEARGS+=$(if $(PROF),-DOPT_PROF=1)
CMAKEARGS+=$(if $(LIBCXX),-DOPT_LIBCXX=1)
CMAKEARGS+=$(if $(STLDEBUG),-DOPT_STL_DEBUGGING=1)
CMAKEARGS+=$(if $(SANITIZE),-DOPT_SANITIZE=1)
CMAKEARGS+=$(if $(ANALYZE),-DOPT_ANALYZE=1)

all:
	cmake -B build . $(CMAKEARGS)
	$(MAKE) -C build $(if $(V),VERBOSE=1)

vc:
	"C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe" -G"Visual Studio 16 2019" -B build . $(CMAKEARGS)
	"C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/MSBuild/Current/Bin/amd64/MSBuild.exe" build/*.sln -t:Rebuild

llvm:
	CC=clang CXX=clang++ cmake -B build . $(CMAKEARGS)
	$(MAKE) -C build $(if $(V),VERBOSE=1)

SCANBUILD=$(firstword $(wildcard /usr/bin/scan-build*))
llvmscan:
	CC=clang CXX=clang++ cmake -B build . $(CMAKEARGS)
	$(SCANBUILD) $(MAKE) -C build $(if $(V),VERBOSE=1)

oldall: dots intersect

%:%.cpp
	clang++ $^ -o $@ -I/usr/local/include/ -std=c++1z -F/usr/local/lib  -framework QtGui -framework QtCore

dots: dots.cpp

intersect: intersect.cpp


clean:
	$(RM) -r build CMakeFiles CMakeCache.txt CMakeOutput.log

