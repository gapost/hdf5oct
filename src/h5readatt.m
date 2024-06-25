# -*- texinfo -*-
# @deftypefn {Function File} {@var{att_val}=} h5readatt (@var{filename},
# @var{location}, @var{attr})
#
# Retrieves the value of the specified attribute named @var{attr}
# from the specified location @var{location} in the HDF5 file @var{filename}.
#
# @table @asis
# @item @var{filename}
# Filename of an existing HDF5 file, specified as a string
# @item @var{location}
# The path to an existing node in the HDF5 file, which can be either a Group,
# a Dataset or a Named DataType. The root group, "/", is also valid.
# @item @var{attr}
# Name of attribute, specified as a string scalar.
# @item @var{att_val}
# Value of the attribute read from the file.
# @end table
#
# @seealso{h5writeatt}
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
