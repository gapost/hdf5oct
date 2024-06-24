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
using namespace H5;
namespace h5o = hdf5oct;

string datatype(const H5::DataType& dt) {
    H5T_class_t dtc = dt.getClass();
    switch (dt.getClass()) {
        case H5T_INTEGER:
            {
                H5::IntType idt(dt.getId());
                string s;
                if (idt.getSign() == H5T_SGN_NONE) s = "uint";
                else s="int";
                size_t sz = idt.getSize();
                if (sz<=8) {
                    s += to_string(sz*8);
                    return s;    
                }    
            }
            break;
        case H5T_FLOAT:
            {
                size_t sz = dt.getSize();
                if (sz==8) return "double";
                else if (sz==4) return "single";
            }
            break;
        case H5T_STRING:
            if (H5Tis_variable_str(dt.getId())>0) return "string";
        default:
            break;
    }
    return "unknown";
}

// h5write(filename,ds,data,start,count,stride)
DEFUN_DLD(__h5write__, args, ,"__h5write__: backend for h5write\n\
Users should not use this directly. Use h5write.m instead")
{
    string filename = args(0).string_value();
    string location = args(1).string_value();
    octave_value data = args(2);
    uint64NDArray start = args(3).uint64_array_value();
    uint64NDArray count = args(4).uint64_array_value();
    uint64NDArray stride = args(5).uint64_array_value();

    try {

        H5File file(filename, H5F_ACC_RDWR);

        // check that location exists and is a dataset
        if (file.nameExists(location)) {
            H5O_info_t obj_info;
            file.getObjinfo(location,obj_info);
            if (obj_info.type!=H5O_TYPE_DATASET) {
                error("h5write: location '%s' is not a Dataset",location.c_str());
                return octave_value();
            }
        } else {
            error("h5write: location %s does not exist",location.c_str());
            return octave_value();
        }

        // Create file dx struct
        h5o::data_exchange dxfile;
        if (!dxfile.set(file.openDataSet(location))) {
            error("h5write: dataset %s: %s", location.c_str(), 
                                              dxfile.lastError.c_str());
            return octave_value();
        }

        // Create octave data dx struct
        h5o::data_exchange dxmem;
        if (!dxmem.set(data)) {
            error("h5write: octave data: %s",dxmem.lastError.c_str());
            return octave_value();
        }

        if (dxmem.dtype_spec=="string" && dxfile.dtype_info.size != H5T_VARIABLE) {
            error("h5write: export of string data is supported only to variable size HDF5 string datasets");
            return octave_value();
        }

        if (!start.isempty() && !dxfile.selectHyperslab(start,count,stride,true)) {
            error("h5write: hyperslab selection: %s",dxfile.lastError.c_str());
            return octave_value();
        }    

        if (!dxmem.isCompatible(dxfile)) {
            error("h5write: incompatible dataset and octave data: %s", dxmem.lastError.c_str());
            return octave_value();
        }

        dxmem.write(dxfile);

    }
   // catch failure caused by the H5File operations
   catch( FileIException e )
   {
      error("%s",e.getCDetailMsg());
   }
 
   // catch failure caused by the DataSet operations
   catch( DataSetIException e )
   {
      error("%s",e.getCDetailMsg());
   }
 
   // catch failure caused by the DataSpace operations
   catch( DataSpaceIException e )
   {
      error("%s",e.getCDetailMsg());
   }
 
   // catch failure caused by the DataSpace operations
   catch( DataTypeIException e )
   {
      error("%s",e.getCDetailMsg());
   }
  
  return octave_value ();

}

