# -*- texinfo -*-
# @deftypefn {Function File} {@var{attval}=} h5readatt (@var{filename}, @var{loc}, @var{attr})  
#
# Retrieves the value of the specified attribute named @var{attr} from the specified 
# location @var{loc} in the HDF5 file @var{filename}.
#
# @var{loc} is either a Group, Dataset or named DataType in the HDF5 file structure.
#
# @seealso{h5readatt}
# @end deftypefn
#

function attrval = h5readatt(filename,location,attr)

# check number and types of arguments
if nargin != 3,
    print_usage();
endif
if (!ischar(filename))
  error("h5readatt: 1st argument must be a string holding the hdf5 file name");
endif
if (!isfile(filename))
  error("h5readatt: filename does not exist");
endif
if (!ischar(location))
  error("h5readatt: 2nd argument must be a string holding the HDF5 object location");
endif
if (!ischar(attr))
  error("h5readatt: 3rd argument must be a string holding the attribute name");
endif

attrval = __h5readatt__(filename,location,attr);
