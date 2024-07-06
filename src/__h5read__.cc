/*
 *
 *    Copyright (C) 2012 Tom Mullins
 *    Copyright (C) 2015 Tom Mullins, Thorsten Liebig, Anton Starikov, Stefan Großhauser
 *    Copyright (C) 2008-2013 Andrew Collette
 *
 *    This file is part of hdf5oct.
 *
 *    hdf5oct is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    hdf5oct is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with hdf5oct.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
// integrated into the GNU Octave build
#include "oct.h"
#include "lo-ieee.h"
#else
// as a package
#include <octave/oct.h>
#include <octave/lo-ieee.h>
#include <octave/file-stat.h>
#include <octave/ls-hdf5.h>
#include <octave/oct-stream.h>
#endif

#include <cstdlib>
#include <cerrno>
#include <iostream>
#include <algorithm>
#include <string>

#include "hdf5oct.h"

using namespace std;
namespace H5 = HighFive;
namespace h5o = hdf5oct;

// data = h5read(filename,dsname,start,count,stride)
DEFUN_DLD(__h5read__, args, ,"__h5read__: backend for h5read\n\
Users should not use this directly. Use h5read.m instead")
{
    string filename = args(0).string_value();
    string location = args(1).string_value();
    uint64NDArray start = args(2).uint64_array_value();
    uint64NDArray count = args(3).uint64_array_value();
    uint64NDArray stride = args(4).uint64_array_value();

    try {

        H5::File file(filename, H5::File::ReadOnly);

        // check that location exists and that it is a dataset
        if (h5o::locationExists(file,location)) {
            if (file.getObjectType(location)!=H5::ObjectType::Dataset) {
                error("h5read: location '%s' is not a Dataset",location.c_str());
                return octave_value();
            }
        } else {
            error("h5read: location %s does not exist",location.c_str());
            return octave_value();
        }

        // Create file dx struct
        h5o::data_exchange dxfile;
        H5::DataSet dset = file.getDataSet(location);
        if (!dxfile.assign(&dset)) {
            error("h5write: dataset %s: %s", location.c_str(), 
                                              h5o::lastError.c_str());
            return octave_value();
        }

        // if requested, select hyperslab
        if (!start.isempty() && !dxfile.selectHyperslab(start,count,stride,false)) {
            error("h5write: hyperslab selection: %s",h5o::lastError.c_str());
            return octave_value();
        }

        octave_value ret = dxfile.read();

        return ret;

    }
   catch (const H5::Exception& err) {
        // catch and print any HDF5 error
        error("%s",err.what());
    }
  
  return octave_value ();

}