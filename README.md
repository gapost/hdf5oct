hdf5oct - a HDF5 wrapper for GNU Octave
=======================================

This is a package for GNU Octave for reading hdf5 files. It
provides an interface compatible to MATLAB's **High-Level Functions for HDF5 files**.

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

# INSTALLATION #########################

To install the latest snapshot run the following in OCTAVE

    pkg install "https://github.com/gapost/hdf5oct/archive/master.zip"

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
© 2008-2013 Andrew Collette \
© 2024 George Apostolopoulos

Released under the LGPLv3.0
