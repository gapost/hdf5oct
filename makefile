CXX=h5c++

octs=__h5create__.oct __h5write__.oct __h5read__.oct h5info.oct
octsources := $(addsuffix .cc,$(basename $(OCTFILES)))

H5FLAGS=$(shell octave --eval 'exit(__octave_config_info__ ("build_features").HDF5 != 1)' &> /dev/null && echo "-DHAVE_HDF5") \
        $(shell octave --eval 'exit(__octave_config_info__ ("build_features").HDF5_18 != 1)' &> /dev/null && echo "-DHAVE_HDF5_18") \
		$(shell octave --eval 'exit(__octave_config_info__ ("build_features").HDF5_H != 1)' &> /dev/null && echo "-DHAVE_HDF5_H") \
		$(shell octave --eval 'exit(__octave_config_info__ ("build_features").HDF5_INT2FLOAT_CONVERSIONS != 1)' &> /dev/null && echo "-DHAVE_HDF5_INT2FLOAT_CONVERSIONS") \
		$(shell octave --eval 'exit(__octave_config_info__ ("build_features").HDF5_UTF8 != 1)' &> /dev/null && echo "-DHAVE_HDF5_UTF8")

MKOCTFILE=CXX="$(CXX)" CXXFLAGS="-ansi -std=c++11 -shlib" mkoctfile -v $(H5FLAGS)

VERSION=0.5.0
PACKAGEFILE=hdf5oct-$(VERSION).tar.gz

.PHONY: test clean install uninstall package

all: $(octs) package

hdf5oct.o : hdf5oct.cc hdf5oct.h
	$(MKOCTFILE) -c hdf5oct.cc

%.oct: %.cc hdf5oct.h hdf5oct.o
	$(MKOCTFILE) $< hdf5oct.o

clean:
	rm -f *.o *.oct package/inst/* test/test*.h5 $(PACKAGEFILE)

install: $(PACKAGEFILE)
	@echo "-- Install Octave Package ------------"
	octave --silent --no-gui --eval "pkg install $(PACKAGEFILE)"

uninstall:
	@echo "-- Uninstall Octave Package ----------"
	octave --silent --no-gui --eval "pkg uninstall hdf5oct"

package: $(PACKAGEFILE)

cp-octave:
	cp h5read.{cc,h} $(HOME)/build/octave/libinterp/dldfcn/

$(PACKAGEFILE): $(octs)
	@echo "-- Create Octave Package Archive ------------"
	mkdir -p package/inst
	cp *.oct package/inst
	tar -czf $(PACKAGEFILE) package/

# TESTING ###########

# a target to test the octave functions
test:
	@echo "-- Perform Tests --------------"
	rm -f test/test*.h5
	cd test && octave --silent --no-gui h5test.m
