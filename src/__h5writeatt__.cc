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

// h5writeatt(filename,location,attr,val)
DEFUN_DLD(__h5writeatt__, args, ,"__h5write__: backend for h5write\n\
Users should not use this directly. Use h5write.m instead")
{
    string filename = args(0).string_value();
    string location = args(1).string_value();
    string attrname = args(2).string_value();
    octave_value data = args(3);

    try {

        H5::File file(filename, H5::File::ReadWrite);

        h5o::data_exchange dxmem;
        if (!dxmem.assign(data)) {
            error("h5writeatt: data value val: %s",dxmem.lastError.c_str());
            return octave_value();
        }

        if (h5o::locationExists(file,location)) {
            switch (file.getObjectType(location)) {
                case H5::ObjectType::Group:
                    {
                        H5::Group g = file.getGroup(location);
                        if (!dxmem.write_as_attribute(g,attrname))
                        {
                            error("h5writeatt: could not write attr: %s",dxmem.lastError.c_str());
                            return octave_value();
                        }
                    }
                    break;
                case H5::ObjectType::Dataset:
                    {
                        H5::DataSet dset = file.getDataSet(location);
                        if (!dxmem.write_as_attribute(dset,attrname))
                        {
                            error("h5writeatt: could not write attr: %s",dxmem.lastError.c_str());
                            return octave_value();
                        }
                    }
                    break;
                case H5::ObjectType::UserDataType:
                    break;
                default:
                    break;
            }
        } else {
            error("h5writeatt: location %s does not exist",location.c_str());
            return octave_value();
        }

        return octave_value();

    }
   catch (const H5::Exception& err) {
        // catch and print any HDF5 error
        error("%s",err.what());
    }
  
  return octave_value ();

}


