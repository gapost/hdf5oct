
OCTAVE ?= octave
MKOCTFILE ?= mkoctfile

H5FLAGS=$(shell ${OCTAVE} --eval 's="";if __octave_config_info__().build_features.HDF5==1,s="-DHAVE_HDF5";end;printf(s)') \
		$(shell ${OCTAVE} --eval 's="";if __octave_config_info__().build_features.HDF5_18==1,s="-DHAVE_HDF5_18";end;printf(s)') \
		$(shell ${OCTAVE} --eval 's="";if __octave_config_info__().build_features.HDF5_H==1,s="-DHAVE_HDF5_H";end;printf(s)') \
		$(shell ${OCTAVE} --eval 's="";if __octave_config_info__().build_features.HDF5_INT2FLOAT_CONVERSIONS==1,s="-DHAVE_HDF5_INT2FLOAT_CONVERSIONS";end;printf(s)') \
		$(shell ${OCTAVE} --eval 's="";if __octave_config_info__().build_features.HDF5_UTF8==1,s="-DHAVE_HDF5_UTF8";end;printf(s)')

INCLUDES = $(shell ${OCTAVE} -f --eval "printf ('%s', __octave_config_info__().build_environment.HDF5_CPPFLAGS)") \
           -I./third_party/HighFive-2.9.0/include

LIBS = $(shell ${OCTAVE} -f --eval "printf ('%s %s', __octave_config_info__().build_environment.HDF5_LDFLAGS, __octave_config_info__().build_environment.HDF5_LIBS)")

.PHONY: clean 

all: hdf5oct.cc hdf5oct.h
	$(MKOCTFILE) -v $(H5FLAGS) $(INCLUDES) $(LIBS) hdf5oct.cc 

clean:
	rm -f hdf5oct.oct 

