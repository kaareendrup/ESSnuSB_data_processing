#
# Config variables
#
LIBNAME		= WCSimToEsbRoot
LIBRARY		= lib$(LIBNAME).so
LIBOBJS		= WCSimRootTools.o \
	 	  WCSimEnumerations.o \
	 	  WCSimRootEvent.o \
	 	  WCSimRootGeom.o \
	 	  PMTubeHit.o \
	 	  EventInfo.o \
		  WCSimToEsbRootCint.o

# ROOT and BRAT variables
#
ROOTCINT	= rootcint 
ROOTINCDIR	= $(shell root-config --incdir)
ROOTCFLAGS	= $(shell root-config --cflags)
ROOTLIBS	= $(shell root-config --glibs)
ROOTINCS	= -I./ 
ACCPATH         = $(shell pwd)
LIBS		= -L./ -l$(LIBNAME) $(ROOTLIBS) 
INCLUDES	= -I./  

# 
# Preprocessor, Compiler, and Linker variables 
#
CPPFLAGS	= $(INCLUDES) $(ROOTCFLAGS)
CXX		= g++
CXXFLAGS	= -c -g -Wall -fPIC -o  
LD		= g++
LDFLAGS		= -rdynamic -Wl,-rpath,$(ACCPATH) $(LIBS) -o 
SOFLAGS         = -shared  -Wl,-soname,
LN		= ln -fs 
OUTOPT		= -o

#
# Rules
#
%Cint.cxx:Include.h LinkDef.h
	rootcint -f $@ -c -I$(ROOTINCDIR) -I./ $^

%.o:%.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $@ $<

%.o:%.cxx
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $@ $<

%.so:
	$(LD) $(SOFLAGS)$@ $(OUTOPT) $@ $^

%:%.o
	$(LD) $(LDFLAGS) $@ $^


#
# Targets 
#
all:	$(LIBRARY)

install: all
	@echo "cannot install, don't know your install dir"
clean:
	rm -f core *~ *.o *Cint.* $(LIBRARY)* *.pcm

realclean: clean
	rm -f $(LIBRARY)*

installhw: all
	rsync -a libCustomLibs.so /home/sukcius/Software/Herwig/lib/
	rsync -a CustomLibsCint_rdict.pcm /home/sukcius/Software/Herwig/lib/

#
# Dependencies 
#
$(LIBRARY):$(LIBOBJS)
