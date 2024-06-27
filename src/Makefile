
octs=__h5create__.oct __h5write__.oct __h5read__.oct h5info.oct __h5writeatt__.oct __h5readatt__.oct
octsources := $(addsuffix .cc,$(basename $(OCTFILES)))

H5FLAGS=$(shell octave --eval 's="";if __octave_config_info__().build_features.HDF5==1,s="-DHAVE_HDF5";end;printf(s)') \
		$(shell octave --eval 's="";if __octave_config_info__().build_features.HDF5_18==1,s="-DHAVE_HDF5_18";end;printf(s)') \
		$(shell octave --eval 's="";if __octave_config_info__().build_features.HDF5_H==1,s="-DHAVE_HDF5_H";end;printf(s)') \
		$(shell octave --eval 's="";if __octave_config_info__().build_features.HDF5_INT2FLOAT_CONVERSIONS==1,s="-DHAVE_HDF5_INT2FLOAT_CONVERSIONS";end;printf(s)') \
		$(shell octave --eval 's="";if __octave_config_info__().build_features.HDF5_UTF8==1,s="-DHAVE_HDF5_UTF8";end;printf(s)')

INCLUDES = $(shell octave -f --eval "printf ('%s', __octave_config_info__().build_environment.HDF5_CPPFLAGS)") \
           -I./third_party/HighFive-2.9.0/include
LIBS = $(shell octave -f --eval "printf ('%s %s', __octave_config_info__().build_environment.HDF5_LDFLAGS, __octave_config_info__().build_environment.HDF5_LIBS)")

MKOCTFILE=mkoctfile -v $(H5FLAGS) $(INCLUDES) $(LIBS)

.PHONY: test clean 

all: $(octs)

hdf5oct.o : hdf5oct.cc hdf5oct.h
	$(MKOCTFILE) -c hdf5oct.cc

%.oct: %.cc hdf5oct.h hdf5oct.o
	$(MKOCTFILE) $< hdf5oct.o

clean:
	rm -f *.o *.oct 

# TESTING ###########

# a target to test the octave functions
test:
	@echo "-- Perform Tests --------------"
	rm -f test/test*.h5
	cd test && octave --silent --no-gui h5test.m
