hdf5oct - a HDF5 wrapper for GNU Octave
=======================================

This is a [GNU Octave](https://octave.org) package for data serialization to/from HDF5 files. 
It provides an interface compatible to MATLAB's **"High-Level Functions for HDF5 files"**.

The following functions are implemented:
```
- h5create
- h5write
- h5writeatt
- h5read
- h5readatt
- h5info
- h5disp 
```

The functions can be used to export/import multidimensional array data of class

    'double','single','uint64','int64', ... 'uint8', 'int8', 'string'

# Getting started

The following short OCTAVE code snippets show how to use the package:

```matlab
>> pkg load hdf5oct % load the package
>> data = uint32(reshape(1:10,2,5)) % create data in workspace
data =

   1   3   5   7   9
   2   4   6   8  10

% create a HDF5 file with a dataset named 'D1' of matching size and datatype
>> h5create('test.h5','D1',size(data),'datatype','uint32')
% store the data to the dataset
>> h5write('test.h5','D1',data)
% read back the values
>> h5read('test.h5','D1')
ans =

   1   3   5   7   9
   2   4   6   8  10

% read the 3rd & 6th columns as a [2x2] matrix 
>> h5read('test.h5','D1',[1 3],[2 2],[1 2])
ans =

   5   9
   6  10
```
Strings are always UTF8 encoded and formatted as cell arrays:

```matlab
>> str = {"one", "δύο", "три", "neljä"}
str =
{
  [1,1] = one
  [1,2] = δύο
  [1,3] = три
  [1,4] = neljä
}
>> oneliner = "This is a single string";
>> h5create('test.h5','D2',size(str),'datatype','string')
>> h5create('test.h5','D3',1,'datatype','string') % scalar string dataset
>> h5write('test.h5','D2',str)
>> h5write('test.h5','D3',oneliner)
>> h5read('test.h5','D2')
ans =
{
  [1,1] = one
  [1,2] = δύο
  [1,3] = три
  [1,4] = neljä
}
>> h5read('test.h5','D3')
ans = This is a single string
```
The structure of the HDF5 file can be viewed with `h5disp`. `h5info` can be also used for more detail.

```matlab
>> h5disp('test.h5')
HDF5 test.h5
Group '/'
  Dataset '/D1'
    Datatype
      Class: 'int'
      OctaveClass: 'uint32'
      Size: 4
      Sign: unsigned
    Extent: [Simple]
    Size: [2  5]
    MaxSize: [2  5]
  Dataset '/D2'
    Datatype
      Class: 'string'
      OctaveClass: 'string'
      Size: H5T_VARIABLE
      charSet: utf8
      Pading: nullterm
    Extent: [Simple]
    Size: [1  4]
    MaxSize: [1  4]
  Dataset '/D3'
    Datatype
      Class: 'string'
      OctaveClass: 'string'
      Size: H5T_VARIABLE
      charSet: utf8
      Pading: nullterm
    Extent: [Scalar]
    Size: []
    MaxSize: []
```
# Array storage layout convention

In HDF5, arrays are stored in C-style, [row-major order](https://en.wikipedia.org/wiki/Row-_and_column-major_order). On the other hand, OCTAVE employs fortran-style column-major storage order.

To avoid array transposition operations during data exchange between OCTAVE and HDF5, this package employs the following convention:

|  Storage Type | Octave array size | HDF5 DataSpace dimensions |
| :---------------: | :---------------: | :-----------------------: |
| Matrix |   $[N \times M]$         |      $[M \times N]$              |
| Multidimensional Array |   $[N_1 \times N_2 \times ... \times N_m]$  | $[N_m \times N_{m-1} \times ... \times N_1]$ |
| Row Vector |  $[1 \times N]$ | $[N \times 1]$ or $[N]$  |
| Column Vector |  $[N \times 1]$ | $[1 \times N]$   |

In this manner, the data is copied directly from memory to disk without any change of order.

An OCTAVE user storing data to and reading back from HDF5 will not notice any difference. However, when a `hdf5oct`-generated HDF5 dataset is opened by another application, the arrays will appear transposed.

# INSTALLATION #########################

To install the latest snapshot run the following in OCTAVE

    pkg install "https://github.com/gapost/hdf5oct/archive/master.zip"

Currently, only Linux systems are supported. 

# DEINSTALLATION #########################

To uninstall the package run

    pkg uninstall hdf5oct

# TODO #################################

- support compression flags for h5create

- write more comprehensive tests instead of a few random choices. Also
  test for error conditions.

## License

© 2012, Tom Mullins \
© 2015, Tom Mullins, Anton Starikov, Thorsten Liebig, Stefan Großhauser \
© 2008-2013, Andrew Collette \
© 2024, George Apostolopoulos

Released under the [LGPLv3+](COPYING).
