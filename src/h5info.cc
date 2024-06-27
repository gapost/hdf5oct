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

DEFUN_DLD(h5info, args, argout,"-*- texinfo -*-\n\
@deftypefn {Loadable Function} info = h5info (@var{filename})\n\
@deftypefnx {Loadable Function} info = h5info (@var{filename}, @var{location})\n\
\n\
info = h5info(filename) returns information about an entire HDF5 file, \
including information about the groups, datasets, and named datatypes contained within it.\n\n\
info = h5info(filename,loc) returns information about the specified location in the HDF5 file.\n\
@end deftypefn"
)
{
    int nargin = args.length ();
    if (nargin < 1 || nargin > 2)
    {
      print_usage ();
      return octave_value ();
    }
    if (! (args(0).is_string () ))
    {
      print_usage ();
      return octave_value ();
    }
    string filename = args(0).string_value();
    string location("/");
    if (nargin==2) {
         if (! (args(1).is_string () ))
        {
            print_usage ();
            return octave_value ();
        }
        location = args(1).string_value();
    }

    octave_scalar_map info;

    try {

        //open the hdf5 file with read-only access
        H5::File file(filename, H5::File::ReadOnly);

        if (h5o::locationExists(file,location)) {
            switch (file.getObjectType(location)) {
                case H5::ObjectType::Group:
                    {
                        h5o::group_info_t I;
                        I.set(file.getGroup(location),location);
                        info = I.oct_map();
                    }
                    break;
                case H5::ObjectType::Dataset:
                    {
                        h5o::dset_info_t I;
                        I.set(file.getDataSet(location),location);
                        info = I.oct_map();
                    }
                    break;
                case H5::ObjectType::UserDataType:
                    break;
                default:
                    break;
            }
        } else {
            error("h5info: location %s does not exist",location.c_str());
            return octave_value();
        }
    }
   catch (const H5::Exception& err) {
        // catch and print any HDF5 error
        error("%s",err.what());
    }
  
  octave_value ov(info);
  return ov;

}


