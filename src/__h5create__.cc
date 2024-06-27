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

// __h5create__(fname,create_file,loc,sz,datatype,chunksize,fillvalue)
DEFUN_DLD(__h5create__, args, ,"__h5create__: backend for h5create\n\
Users should not use this directly. Use h5create.m instead")
{
    string filename = args(0).string_value();
    bool create_file = args(1).bool_value();
    string location = args(2).string_value();
    uint64NDArray size = args(3).uint64_array_value();
    string datatype = args(4).string_value();
    uint64NDArray chunksize = args(5).uint64_array_value();
    int fillvalue = args(6).int_value();

    bool has_unlimited = !chunksize.isempty();

    try {

        // H5::Exception::dontPrint();

        //open the hdf5 file, create it if it does not exist
        H5::File file(filename, create_file ? H5::File::Create : H5::File::ReadWrite);

         if (h5o::locationExists(file,location)) {
               error("h5create: location '%s' already exists",location.c_str());
               return octave_value();
            }        

         H5::DataSpace fspace = H5::DataSpace::Scalar();
         size_t ndim = size.numel(); 
         bool is_scalar = (ndim==1 && size_t(size(0))==1) ||
                           (ndim==2 && size_t(size(0))==1 & size_t(size(1))==1);
         if (!is_scalar) {
            vector<size_t> dims(ndim), maxdims(ndim);
            for(int i=0; i<ndim; i++) {
               bool b = size_t(size(i)) == H5::DataSpace::UNLIMITED;
               dims[ndim-1-i] = b ? 0 : size_t(size(i));
               maxdims[ndim-1-i] = size_t(size(i));
            }
            fspace = H5::DataSpace(dims, maxdims);
         }

        /*
        * Modify dataset creation properties, i.e. enable chunking.
        */
        H5::DataSetCreateProps dscp;

      if (has_unlimited) { 
         vector<hsize_t> chunk_dims(ndim);  
         for(int i=0; i<ndim; i++) chunk_dims[ndim-1-i] = chunksize(i);
            dscp.add(H5::Chunking(chunk_dims));         
      }
 
        /*
        * Set fill value for the dataset
        */
        // cparms.setFillValue( PredType::NATIVE_INT, &fill_val);
 
        /*
        * Create a new dataset within the file using dscp
        * creation properties.
        */
       file.createDataSet(location,fspace,h5o::h5type_from_spec(datatype),dscp);
    }
    catch (const H5::Exception& err) {
        // catch and print any HDF5 error
        error("%s",err.what());
    }

  
  return octave_value ();

}


