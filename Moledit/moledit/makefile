CXX = g++
SVNURL = $(shell svn info | grep URL | sed "s/.*\/\(.*\)/\1/g")
SVNREV = $(shell svnversion | sed 's/:/./g')

CXXFLAGS = $(SVNDEV)

VERFILE = version

OBJ = moledit.o header.o system.o molecule.o quaternion.o randomArray.o component.o mark.o err.o
OBJD = $(OBJ:%.o=%.do)

HEADER = $(OBJ:%.o=%.h) constants.h
CPP = $(OBJ:%.o=%.cpp) random1.cpp
DEPS = $(OBJ:%.o=%.dep)

include $(VERFILE)

all: moledit

moledit: $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o moledit

$(OBJ):
	$(CXX) $(CXXFLAGS) -Wall -O -c $<

moleditd: $(OBJD)
	$(CXX) $(CXXFLAGS) $(OBJD) -o moleditd

$(OBJD):
	$(CXX) $(CXXFLAGS) -Wall -g -D DEBUG -c $< -o $@

%.dep: %.cpp %.h
	$(CXX) -MT $*.o -MM $< > $@
	$(CXX) -MT $*.do -MM $< >> $@

clean:
	rm -f $(OBJ) $(OBJD) $(DEPS)
	
clear: clean
	rm -f moledit moleditd
	
$(VERFILE):
	echo 'VERSION = $(SVNURL)' > version
	echo 'REVISION = $(SVNREV)' >> version
	echo 'SVNDEV = -D'\''REV="$(REVISION)"'\'' -D'\''VER="$(VERSION)"'\''' >> $@

tar: $(VERFILE)
	tar -cvjf 'moledit_$(SVNURL)_$(SVNREV).tar.bz2' $(HEADER) $(CPP) makefile $(VERFILE)

-include $(DEPS)