##
##    Copyright (C) 2012 Tom Mullins
##    Copyright (C) 2015 Tom Mullins, Thorsten Liebig, Anton Starikov, Stefan Großhauser
##    Copyright (C) 2008-2013 Andrew Collette
##
##    This file is part of hdf5oct.
##
##    hdf5oct is free software: you can redistribute it and/or modify
##    it under the terms of the GNU Lesser General Public License as published by
##    the Free Software Foundation, either version 3 of the License, or
##    (at your option) any later version.
##
##    hdf5oct is distributed in the hope that it will be useful,
##    but WITHOUT ANY WARRANTY; without even the implied warranty of
##    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##    GNU Lesser General Public License for more details.
##
##    You should have received a copy of the GNU Lesser General Public License
##    along with hdf5oct.  If not, see <http://www.gnu.org/licenses/>.
##

# -*- texinfo -*-
# @deftypefn {Function File} { } h5create (@var{filename}, @var{dsetname}, @var{size}, @var{key}, @var{val},...)
#
# Create a dataset with name @var{dsetname} and size @var{size}
# in the HDF5 file specified by @var{filename}.  Intermediate groups
# are created as necessary.
#
# The vector @var{size} may contain one or several Inf (or
# equivalently: zero) values.
# This will lead to unlimited maximum extent of the dataset in the
# respective dimensions and 0 initial extent.
# Note that any dataset with at least one unlimited dimension must be chunked and
# it is generally recommended for large datasets.
#
# The list of @var{key}, @var{val} arguments allows to specify
# certain properties of the dataset. Allowed settings are:
#
# @table @asis
# @item @option{Datatype}
# one of the strings: 
# @samp{double}(default) | @samp{single} | @samp{uint64} | @samp{uint32} | 
# @samp{uint16} | @samp{uint8} | @samp{int64} | @samp{int32} | @samp{int16} | 
# @samp{int8} | 'string'
# @item @option{ChunkSize}
# The value may be either a vector specifying the chunk size,
# or an empty vector [], which means no chunking (this is the default),
# or the string @samp{auto} which makes the library choose automatically
# an appropriate chunk size, as best as it can. Note that the @samp{auto}
# setting is not @sc{matlab} compatible.
# @end table
#
# @seealso{h5write}
# @end deftypefn


function h5create(filename,location,sz,varargin)

# check number and types of arguments
if ((nargin < 3))
    error("h5create: you must supply at least 3 arguments");
endif
if (!ischar(filename))
  error("h5create: 1st argument must be a string holding the hdf5 file name");
endif
create_file = 1;
if isfile(filename)
  # check valid hdf5
  create_file = 0;
endif
if (!ischar(location))
  error("h5create: 2nd argument must be a string holding the dataset location");
endif
if !isreal(sz) || !isvector(sz),
  error("h5create: 3rd argument must be a dimension vector");
endif
sz = sz(:);
# sz elements must be index or Inf
for i=1:length(sz),
  if !(isindex(sz(i)) || sz(i)==inf)
    error("h5create: 3rd argument elements must be valid index values or inf");
  endif
end

## check options
[reg, datatype, chunksize, fillvalue] = parseparams (varargin, ...
  'Datatype', 'double',...
  'ChunkSize',[],...
  'FillValue',0);

# check datatype
if !(strcmp(datatype,'double') || ...
  strcmp(datatype,'single') || ...
  strcmp(datatype,'uint64') || ...
  strcmp(datatype,'int64') || ...
  strcmp(datatype,'uint32') || ...
  strcmp(datatype,'int32') || ...
  strcmp(datatype,'uint16') || ...
  strcmp(datatype,'int16') || ...
  strcmp(datatype,'uint8') || ...
  strcmp(datatype,'int8') || ...
  strcmp(datatype,'string'))
  error("h5create: invalid 'Datatype'");
endif
# check
if !isindex(sz) && isempty(chunksize)
  error("h5create: chunksize must be defined for inf size datasets");
endif
if !isempty(chunksize) 
  if size(chunksize(:))!=size(sz),
    error("h5create: chunksize is different size from 3rd argument");
  end
  if !isindex(chunksize),
    error("h5create: invalid chunksize");
  end
  chunksize = chunksize(:);
end

if size(sz,1)==1, # convert [n] to [1xn]
  sz = [sz; 1];
  if !isempty(chunksize)
    chunksize = [chunksize; 1];
  end
end

__h5create__(filename,create_file,location,sz,datatype,chunksize,fillvalue);


