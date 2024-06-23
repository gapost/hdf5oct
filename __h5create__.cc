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
        H5::H5File file(filename, create_file ? H5F_ACC_TRUNC : H5F_ACC_RDWR);

        // check if location exists 
        if (file.nameExists(location)) {
            error("h5create: location '%s' already exists",location.c_str());
            return octave_value();
         }

         DataSpace fspace(H5S_SCALAR);
         size_t ndim = size.numel(); 
         bool is_scalar = (ndim==1 && hsize_t(size(0))==1) ||
                           (ndim==2 && hsize_t(size(0))==1 & hsize_t(size(1))==1);
         if (!is_scalar) {
            std::vector<hsize_t> dims(ndim), maxdims(ndim);
            for(int i=0; i<ndim; i++) {
               bool b = hsize_t(size(i)) == H5S_UNLIMITED;
               dims[ndim-1-i] = b ? 0 : hsize_t(size(i));
               maxdims[ndim-1-i] = b ? H5S_UNLIMITED : hsize_t(size(i));
            }
            fspace = DataSpace(ndim, dims.data(), maxdims.data());
         }

        /*
        * Modify dataset creation properties, i.e. enable chunking.
        */
        DSetCreatPropList dscp;
        std::vector<hsize_t> chunk_dims(ndim);

        if (has_unlimited) {            
            for(int i=0; i<ndim; i++) chunk_dims[ndim-1-i] = chunksize(i);
            dscp.setChunk( ndim, chunk_dims.data() );
        }
 
        /*
        * Set fill value for the dataset
        */
        // cparms.setFillValue( PredType::NATIVE_INT, &fill_val);
 
        /*
        * Create a new dataset within the file using dscp
        * creation properties.
        */
        LinkCreatPropList lcpl;
        lcpl.setCreateIntermediateGroup(true);
        file.createDataSet(location, h5o::oct2hdf(datatype), fspace, dscp,
                            DSetAccPropList::DEFAULT, lcpl);
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


