# -*- texinfo -*-
# @deftypefn {Function File} { } h5writeatt (@var{filename}, @var{location}, @var{attr}, @var{val})
#
# Write attribute to HDF5 file.
#
# @table @asis
# @item @var{filename}
# Filename of an existing HDF5 file, specified as a string
# @item @var{location}
# The path to an existing node in the HDF5 file, which can be either a Group,
# a Dataset or a Named DataType. The root group, "/", is also valid.
# @item @var{attr}
# Name of attribute, specified as a string scalar. If the attribute does not exist,
# h5writeatt creates the attribute with the name specified. If the specified
# attribute already exists, it will be overwritten.
# @item @var{val}
# Value of the attribute to be written. It can be a scalar or array variable,
# numeric or string (UTF8 by default). The HDF5 standard suggests that
# attributes should be small in size.
# @end table
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
