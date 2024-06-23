# -*- texinfo -*-
# @deftypefn  {Function File} { } h5disp (@var{filename})
# @deftypefnx {Function File} { } h5disp (@var{filename},@var{location})
#
# Display contents of a HDF5 file.
#
# h5disp(filename) displays the metadata that describes the structure of the specified HDF5 file.
#
# h5disp(filename,location) displays the metadata for the specified location within the file.
#
# @seealso{h5info}
# @end deftypefn


function h5disp(filename,location,sz,varargin)

if nargin<1 || nargin>2,
    print_usage();
endif

if nargin == 1
    location = "/";
endif

info = h5info(filename,location);

disp(["HDF5 " filename]);
__disph5__(info,0);

endfunction

function __disph5__(info, depth)

indent = blanks(2*depth);

if isfield(info,"Groups"), # info is a group
    disp([indent "Group '" info.Name "'"]);
    datasets = info.Datasets;
    for i=1:size(datasets,1) __disph5__(datasets(i),depth+1); endfor
    groups = info.Groups;
    for i=1:size(groups,1) __disph5__(groups(i),depth+1); endfor
elseif isfield(info,"Dataspace"), # info is a dataset
    disp([indent "Dataset '" info.Name "'"]);
    __disph5__(info.Datatype,depth+1);
    disp([indent "  Extent: [" num2str(info.Dataspace.ExtentType) "]"]);
    disp([indent "  Size: [" num2str(info.Dataspace.Size) "]"]);
    disp([indent "  MaxSize: [" num2str(double(info.Dataspace.MaxSize)) "]"]);
    if !isempty(info.ChunkSize)
        disp([indent "  ChunkSize: [" num2str(info.ChunkSize) "]"]);
    endif
elseif isfield(info,"Class"), # info is a datatype
    disp([indent "Datatype"]);
    disp([indent "  Class: '" info.Class "'"]);
    disp([indent "  OctaveClass: '" info.OctaveClass "'"]);
    if isfield(info,"Size"), disp([indent "  Size: " num2str(info.Size) ]); end
    if isfield(info,"Sign"), disp([indent "  Sign: " info.Sign]); end
    if isfield(info,"charSet"), disp([indent "  charSet: " info.charSet]); end 
    if isfield(info,"Pading"), disp([indent "  Pading: " info.Pading]); end
end

endfunction








