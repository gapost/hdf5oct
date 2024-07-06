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

template<class H5Obj>
octave_value read_attr(const H5Obj& obj, const string& attrname)
{
    if (!obj.hasAttribute(attrname)) {
        error("h5readatt: attribute %s does not exist",attrname.c_str());
        return octave_value();
    }
    h5o::data_exchange dx;
    H5::Attribute attr = obj.getAttribute(attrname);
    if (!dx.assign(&attr)) {
        error("h5readatt: attribute %s: %s", attrname.c_str(), 
                                            h5o::lastError.c_str());
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

        H5::File file(filename, H5::File::ReadOnly);

        if (h5o::locationExists(file,location)) {
            switch (file.getObjectType(location)) {
                case H5::ObjectType::Group:
                    {
                        H5::Group g = file.getGroup(location);
                        return read_attr(g,attrname);
                    }
                case H5::ObjectType::Dataset:
                    {
                        H5::DataSet dset = file.getDataSet(location);
                        return read_attr(dset,attrname);
                    }
                case H5::ObjectType::UserDataType:
                default:
                    break;
            }
        } else {
            error("h5readatt: location %s does not exist",location.c_str());
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


