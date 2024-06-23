# -*- texinfo -*-
# @deftypefn {Function File} { } h5write (@var{filename}, @var{dsetname}, @var{data})
# @deftypefnx {Function File} { } h5write (@var{filename}, @var{dsetname}, @var{data}, @var{start}, @var{count})
# @deftypefnx {Function File} { } h5write (@var{filename}, @var{dsetname}, @var{data}, @var{start}, @var{count}, @var{stride})
#
# @code{h5write(filename,dsetname,data)} writes data to an entire dataset, dsetname,
# in the specified HDF5 file. If the dataset is fixed in size,
# the amount of data to be written must match the size of the dataset.
#
# h5write(filename,ds,data,start,count) writes a subset of data to a dataset,
# beginning at starting location start, and continuing for count elements.
# In a multidimensional dataset, count specifies a distance in each direction.
# h5write extends an extendable dataset along any unlimited dimensions,
# if necessary.
#
# h5write(filename,ds,data,start,count,stride) specifies the spacing between
# elements, stride, along each dimension of the dataset.
#
# @table @asis
# @item @var{filename}
# Filename of an existing HDF5 file, specified as a string
# @item @var{dsetname}
# Dataset name, specified as a string scalar or character vector containing
# the name of an existing dataset in the HDF5 file.
# @item @var{data}
# Data to be written to the HDF5 file.
# If a numeric datatype was specified in the corresponding call to h5create,
# then data is a numeric matrix containing floating-point or integer data.
# Data must be non sparse, and must be the same size as the HDF5 dataset
# if you do not specify start or count.
# If a dimension in the dataset is unlimited, then the data to be written can be any size along that dimension.
#
# If "string" was specified as the datatype in the corresponding call to h5create,
# data is a cell string array. The array dimensions must match those specified in the call to h5create.
# @end table
#
# @seealso{h5create}
# @end deftypefn
#

function h5write(filename,location,data,start_pos,count,stride)

# check number and types of arguments
if !((nargin == 3) || (nargin==5) || (nargin==6))
    print_usage();
endif
if (!ischar(filename))
  error("h5write: 1st argument must be a string holding the hdf5 file name");
endif
if (!isfile(filename))
  error("h5write: filename does not exist");
endif
if (!ischar(location))
  error("h5write: 2nd argument must be a string holding the dataset location");
endif

datasize = size(data);
datasize = datasize(:);

if nargin==3,
    start_pos = [];
    count = [];
    stride = [];
else
    start_pos = check_idx_vec(start_pos,datasize,'start');
    count = check_idx_vec(count,datasize,'count');
    if nargin==6,
        stride = check_idx_vec(stride,datasize,'stride');
    else
        stride = [];
    end
end

if ischar(data), data = cellstr(data); end

__h5write__(filename,location,data,start_pos,count,stride);

endfunction

function j = check_idx_vec(i, sz, lbl)

if !(isvector(i) && isindex(i)),
    error(["h5write: " lbl " must be a vector of valid index values"]);
end
j = i(:);
if length(j)==1,
  j = [j; 1];
end
if length(j) != length(sz)
    error(["h5write: size of " lbl " incompatible with data"]);
end

end




