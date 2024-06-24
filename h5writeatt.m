# -*- texinfo -*-
# @deftypefn {Function File} { } h5writeatt (@var{filename}, @var{loc}, @var{attr}, @var{val})  
#
# Write attribute to HDF5 file.
# Writes an attribute named @var{attr} with a value of @var{val} to the specified location @var{loc} 
# in the HDF5 file named @var{filename}.
#
# @var{loc} is either a Group, Dataset or named DataType in the HDF5 file structure.
#
# @seealso{h5readatt}
# @end deftypefn
#

function h5writeatt(filename,location,attr,val)

# check number and types of arguments
if !(nargin == 4)
    print_usage();
endif
if (!ischar(filename))
  error("h5writeatt: 1st argument must be a string holding the hdf5 file name");
endif
if (!isfile(filename))
  error("h5writeatt: filename does not exist");
endif
if (!ischar(location))
  error("h5writeatt: 2nd argument must be a string holding the HDF5 object location");
endif
if (!ischar(attr))
  error("h5writeatt: 3rd argument must be a string holding the attribute name");
endif

if ischar(val), val = cellstr(val); end

__h5writeatt__(filename,location,attr,val);
