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

octave_value read_attr(const H5Object& obj, const string& attrname)
{
    if (!obj.attrExists(attrname)) {
        error("h5readatt: attribute %s does not exist",attrname.c_str());
        return octave_value();
    }
    h5o::data_exchange dx;
    if (!dx.set(obj.openAttribute(attrname))) {
        error("h5readatt: attribute %s: %s", attrname.c_str(), 
                                            dx.lastError.c_str());
        return octave_value();
    }
    return dx.read_attribute();
}

// h5readatt(filename,location,attr,val)
DEFUN_DLD(__h5readatt__, args, ,"__h5write__: backend for h5write\n\
Users should not use this directly. Use h5write.m instead")
{
    string filename = args(0).string_value();
    string location = args(1).string_value();
    string attrname = args(2).string_value();

    try {

        H5File file(filename, H5F_ACC_RDONLY);

        h5o::data_exchange dxfile;
        Attribute attr;

        if (file.nameExists(location)) {
            H5O_info_t obj_info;
            file.getObjinfo(location,obj_info);
            switch (obj_info.type) {
                case H5O_TYPE_GROUP:
                    return read_attr(file.openGroup(location),attrname);
                case H5O_TYPE_DATASET:
                    return read_attr(file.openDataSet(location),attrname);
                case H5O_TYPE_NAMED_DATATYPE:
                    return read_attr(file.openDataType(location),attrname);
                default:
                    break;
            }
        } else {
            error("h5readatt: location %s does not exist",location.c_str());
            return octave_value();
        }

        return octave_value();

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

   // catch failure caused by the DataSpace operations
   catch( AttributeIException e )
   {
      error("%s",e.getCDetailMsg());
   }

   // catch failure caused by the location operations
   catch( LocationException e )
   {
      error("%s",e.getCDetailMsg());
   }
  
  return octave_value ();

}

