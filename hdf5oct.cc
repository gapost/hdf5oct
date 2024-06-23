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


H5::DataType hdf5oct::oct2hdf(const string& datatype) {
    if (datatype == "double") return  H5::PredType::NATIVE_DOUBLE;
    else if (datatype == "single") return  H5::PredType::NATIVE_FLOAT;
    else if (datatype == "uint64") return  H5::PredType::NATIVE_UINT64;
    else if (datatype == "int64") return  H5::PredType::NATIVE_INT64;
    else if (datatype == "uint32") return  H5::PredType::NATIVE_UINT32;
    else if (datatype == "int32") return  H5::PredType::NATIVE_INT32;
    else if (datatype == "uint16") return  H5::PredType::NATIVE_UINT16;
    else if (datatype == "int16") return  H5::PredType::NATIVE_INT16;
    else if (datatype == "uint8") return  H5::PredType::NATIVE_UINT8;
    else if (datatype == "int8") return  H5::PredType::NATIVE_INT8;
    else if (datatype == "string") {
      StrType s(PredType::C_S1, H5T_VARIABLE);
      s.setCset(H5T_CSET_UTF8);
      return s;
    }
    return DataType();
}

string h5_concat_path(const string& path, const string& obj_name)
{
    string p(path);
    if (path.back() != '/') p += '/';
    p += obj_name;
    return p;
}

void hdf5oct::dspace_info_t::set(const H5::DataSpace& ds) {
    int ndim;
    switch (ds.getSimpleExtentType()) {
      case H5S_SIMPLE:
          extent_type = "Simple";
          ndim = ds.getSimpleExtentNdims();
          size = uint64NDArray(dim_vector(1,ndim));
          maxsize = uint64NDArray(dim_vector(1,ndim));
          {
              vector<hsize_t> hdims(ndim), hmaxdims(ndim);
              ds.getSimpleExtentDims(hdims.data(),hmaxdims.data());        
              for(octave_idx_type i=0; i<ndim; i++) {
                  size(i) = hdims[ndim-1-i];
                  maxsize(i) = hmaxdims[ndim-i-1]==H5S_UNLIMITED ? INFINITY : hmaxdims[ndim-i-1];
              }
          }
          break;
      case H5S_SCALAR:
          extent_type = "Scalar";
          break;
      case H5S_NULL:
          extent_type = "Empty";
          break;
      default:
          break;
    }    
}
octave_scalar_map hdf5oct::dspace_info_t::oct_map() const {
    map<string, octave_value> M;
    RowVector dmaxsize(maxsize.numel());
    for(int i=0; i<maxsize.numel(); i++) {
        dmaxsize(i) = hsize_t(maxsize(i))==H5S_UNLIMITED ? INFINITY : double(maxsize(i));
    }
    M["ExtentType"] = extent_type;
    M["Size"] = size;
    M["MaxSize"] = dmaxsize;
    return octave_scalar_map(M);
}
bool hdf5oct::dspace_info_t::needs_chunk() const {
    if (size.isempty()) return false;
    int n = size.numel(); 
    // HDF5 specs: chunking is needed if size!=maxsize OR maxsize==unlimited
    for(int i=0; i<n; i++) {
        if (size(i)!=maxsize(i) || hsize_t(maxsize(i))==H5S_UNLIMITED)
            return true;
    }
    return false;
}
void hdf5oct::dtype_info_t::set(const H5::DataType& dt) {
    switch (dt.getClass()) {
        case H5T_INTEGER:
            h5class = "int";
            {
                IntType it(dt.getId());
                bool isUnsigned = it.getSign()==0;
                signed_status = isUnsigned ? "unsigned" : "signed";
                size = it.getSize();
                if (size<=8) {
                    octave_class = isUnsigned ? "uint" : "int";
                    octave_class += to_string(size*8);
                }                 
            }
            break;
        case H5T_FLOAT:
            h5class = "float";
            {
                FloatType ft(dt.getId());
                size = ft.getSize();
                if (size==4) octave_class = "single";
                if (size==8) octave_class = "double";
            }
            break;
        case H5T_TIME:
            h5class = "time";
            break;
        case H5T_STRING:
            h5class = "string";
            {
                StrType st(dt.getId());
                size = st.getSize();
                htri_t ret = H5Tis_variable_str(st.getId());
                if (ret <0) {
                  error("Error in call to H5Tis_variable_str");
                  return;
                }
                if (ret!=0) size = H5T_VARIABLE;
                switch (st.getCset()) {
                    case H5T_CSET_ASCII:
                        cset = "ascii";
                        break;
                    case H5T_CSET_UTF8:
                        cset = "utf8";
                        break;
                    default:
                        break;
                }
                switch (st.getStrpad()) {
                    case H5T_STR_NULLTERM:
                        pading = "nullterm";
                        break;
                    case H5T_STR_NULLPAD:
                        pading = "nullpad";
                        break;
                    case H5T_STR_SPACEPAD:
                        pading = "spacepad";
                        break;
                    default:
                        break;
                }
                octave_class = "string";
            }
            break;
        case H5T_BITFIELD:    
            h5class = "bitfield";
            break;
        case H5T_OPAQUE:
            h5class = "opaque";
            break;
        case H5T_COMPOUND:
            h5class = "compound";
            break;
        case H5T_REFERENCE:
            h5class = "reference";
            break;
        case H5T_ENUM: 
            h5class = "enum";
            break;
        case H5T_VLEN:
            h5class = "vlen";
            break;
        case H5T_ARRAY:
            h5class = "array";
            break;
        case H5T_NO_CLASS:
        default:
            break;
    }    
}
octave_scalar_map hdf5oct::dtype_info_t::oct_map() const {
    map<string, octave_value> M;
    M["Class"] = h5class;
    M["OctaveClass"] = octave_class;
    if (size) 
        M["Size"] = size==H5T_VARIABLE ? octave_value("H5T_VARIABLE") : 
                                         octave_value(size);
    if (!signed_status.empty()) M["Sign"]=signed_status;
    if (!cset.empty()) M["charSet"]=cset;
    if (!pading.empty()) M["Pading"]=pading;
    return octave_scalar_map(M);
}
void hdf5oct::dset_info_t::set(const DataSet& ds, const string& path) {
    name = path;  
    DataType dt = ds.getDataType();
    dtype_info.set(dt); 
    dspace_info.set(ds.getSpace());
    if (dspace_info.needs_chunk()) {
        DSetCreatPropList dscpl = ds.getCreatePlist();
        int ndim = dspace_info.size.numel();
        chunksize = uint64NDArray(dim_vector(1,ndim));
        vector<hsize_t> hdims(ndim);
        dscpl.getChunk(ndim, hdims.data());
        for(octave_idx_type i=0; i<ndim; i++) chunksize(i) = hdims[ndim-1-i];
    }   
}
octave_scalar_map hdf5oct::dset_info_t::oct_map() const {
    // all map fields mandatory !!
    map<string, octave_value> M;
    M["Name"] = name;
    M["Datatype"] = dtype_info.oct_map();
    M["Dataspace"] = dspace_info.oct_map();
    M["ChunkSize"] = chunksize; 
    return octave_scalar_map(M);
}
void hdf5oct::group_info_t::set(const Group& g, const string& path) {
    name = path;
    hsize_t n = g.getNumObjs();
    vector<string> group_names, ds_names,dt_names,link_names;
    for(hsize_t i=0; i<n; i++) {
        H5G_obj_t type = g.getObjTypeByIdx(i);
        string obj_name = g.getObjnameByIdx(i);
        switch (type) {
            case H5G_GROUP:
                {
                    group_info_t info;
                    info.set(g.openGroup(obj_name),
                             h5_concat_path(path,obj_name));
                    groups.push_back(info);   
                }
                break;
            case H5G_DATASET:
                {
                    dset_info_t info;
                    info.set(g.openDataSet(obj_name),
                             h5_concat_path(path,obj_name)); 
                    datasets.push_back(info);   
                }
                break;
            case H5G_TYPE:
                {
                    named_dtype_info_t info;
                    info.set(g.openDataType(obj_name),
                             h5_concat_path(path,obj_name)); 
                    datatypes.push_back(info);   
                }
                break;
            case H5G_LINK:
            case H5G_UDLINK:
                link_names.push_back(obj_name);
                {
                    link_info_t info;
                    info.name = h5_concat_path(path,obj_name);
                    links.push_back(info);   
                }
                break;
            default:
            break;
        }
    }  
}
octave_scalar_map hdf5oct::group_info_t::oct_map() const {
    // all map fields mandatory !!
    map<string, octave_value> M;
    M["Name"] = name;

    octave_map omap;

    vector<string> keys{"Name","Groups","Datasets",
                        "Datatypes","Links"};
    omap = octave_map(dim_vector(groups.size(),1),keys);
    for(int i=0; i<groups.size(); i++) 
        omap.fast_elem_insert(i, groups[i].oct_map());
    M["Groups"] = omap;

    keys = {"Name","Datatype","Dataspace","ChunkSize"};
    omap = octave_map(dim_vector(datasets.size(),1),keys);
    for(int i=0; i<datasets.size(); i++) 
        omap.fast_elem_insert(i, datasets[i].oct_map());
    M["Datasets"] = omap;

    keys = {"Name"};
    omap = octave_map(dim_vector(datatypes.size(),1),keys);
    for(int i=0; i<datatypes.size(); i++) 
        omap.fast_elem_insert(i, datatypes[i].oct_map());
    M["Datatypes"] = omap;

    keys = {"Name"};
    omap = octave_map(dim_vector(links.size(),1),keys);
    for(int i=0; i<links.size(); i++) 
        omap.fast_elem_insert(i, links[i].oct_map());
    M["Links"] = omap;

    return octave_scalar_map(M);
}
octave_scalar_map hdf5oct::link_info_t::oct_map() const {
    map<string, octave_value> M;
    M["Name"] = name;
    return octave_scalar_map(M);
}

// compatible means:
// - same datatype spec
// - same extent type (Scalar/Simple)
// - same dimensionality
// - same # of elements in each dim 
bool hdf5oct::data_exchange::isCompatible(const data_exchange& dx)
{
    if (dtype_spec != dx.dtype_spec) {
        lastError = "different datatypes";
        return false;
    }

    if (dspace_info.extent_type != dx.dspace_info.extent_type) {
        lastError = "different dataspaces extent type";
        return false;
    }

    if (dspace_info.isSimple()) {
        int ndim = dspace.getSimpleExtentNdims();
        if (ndim != dx.dspace.getSimpleExtentNdims()) {
            lastError = "different dataspace dimensionality";
            return false;
        }
        if (ndim==1 && (dv(1) != dx.dv(1))) {
            lastError = "different number of elements";
            return false;
        } 
        if (ndim > 1) {
            for(int i=0; i<ndim; i++) {
                if (dv(i) != dx.dv(i)) {
                    lastError = "different number of elements";
                    return false;
                }
            }
        }
    } 
    return true;
}

void hdf5oct::data_exchange::reset()
{
    lastError = "";
    ov = octave_value();
    dset = DataSet();
    dtype = DataType();
    dtype_info = dtype_info_t();
    dspace = DataSpace();
    dspace_info = dspace_info_t();
    dtype_spec = "";    
}

bool hdf5oct::data_exchange::set(const H5::DataSet& ds)
{
    reset();
    dset = ds;
    dtype = ds.getDataType();
    dspace = ds.getSpace();

    // check datatype
    dtype_info.set(dtype); 
    if (dtype_info.h5class.empty()) {
        lastError = "Invalid HDF5 datatype";
        return false;
    }
    if (dtype_info.octave_class.empty()) {
        lastError = "Unsupported datatype '";
        lastError += dtype_info.h5class;
        lastError += "'";
        return false;
    }
    dtype_spec = dtype_info.octave_class;
    

    // check dataspace
    dspace_info.set(dspace);
    if (dspace_info.extent_type.empty() ||
        dspace_info.isNull()) {
        lastError = "Empty HDF5 dataspace";
        return false;
    }
    if (dspace_info.isScalar()) dv = dim_vector(1,1);
    if (dspace_info.isSimple()) {
        size_t n = dspace_info.size.numel();
        if (n==1) dv = dim_vector(1,dspace_info.size(0)); // 1D dataset -> [1xN] octave array
        else {
            dv.resize(n);
            for(size_t i=0; i<n; i++) dv(i) = dspace_info.size(i);
        }
    }

    return true;
}
bool hdf5oct::data_exchange::selectHyperslab(uint64NDArray start, 
uint64NDArray count, uint64NDArray stride, bool tryExtend)
{
    if (dspace_info.isScalar()) {
        lastError = "Hyperslab selection not possible for scalar datasets";
        return false;
    }

    int ndim = dspace_info.size.numel();
    vector<hsize_t> hstart(ndim), hcount(ndim), 
                    hstride(ndim), fdims(ndim);
    dspace.getSimpleExtentDims(fdims.data());
    bool need_extend = false;
    for(int i=0; i<ndim; i++) {
        int j = ndim-1-i;
        hstart[j] = hsize_t(start(i)) - 1;
        hcount[j] = count(i);
        hstride[j] = (stride.isempty()) ? 1 : hsize_t(stride(i));
        hsize_t end = hstart[j]+1+(hcount[j]-1)*hstride[j];
        if (end > fdims[j]) {
            if (tryExtend) {
                if (end < hsize_t(dspace_info.maxsize(i))) {
                    fdims[j]=end; need_extend = true; 
                }
                else {
                    lastError = "Hyperslab selection superseeds size of non-extensible dataset";
                    return false;
                }
            } else {
                lastError = "hyperslab selection beyond the dataset's size";
                return false;
            }
        }
    }
    if (need_extend) {
        dset.extend(fdims.data());
        dspace = dset.getSpace();
    }
    dspace.selectHyperslab(H5S_SELECT_SET,
        hcount.data(),hstart.data(), 
        stride.isempty() ? NULL : hstride.data());

    // adjust dim_vector 
    if (ndim==1) dv(1) = count(0);  // special case of 1D h5 -> [1xN] octave
    else for(int i=0; i<ndim; i++) dv(i) = count(i);
    return true;
}

bool hdf5oct::data_exchange::set(octave_value v) {
        
    reset();

    // find class
    if (v.isinteger()) {
        if (v.is_uint64_type()) dtype_spec = "uint64";
        else if (v.is_int64_type()) dtype_spec = "int64";
        else if (v.is_uint32_type()) dtype_spec = "uint32";
        else if (v.is_int32_type()) dtype_spec = "int32";
        else if (v.is_uint16_type()) dtype_spec = "uint16";
        else if (v.is_int16_type()) dtype_spec = "int16";
        else if (v.is_uint8_type()) dtype_spec = "uint8";
        else if (v.is_int8_type()) dtype_spec = "int8";
    }
    else if (v.isreal()) {
        if (v.is_double_type()) dtype_spec = "double";
        else if (v.is_single_type()) dtype_spec = "single";
    }
    else if (v.iscellstr()) dtype_spec = "string";
    if (dtype_spec.empty()) {
        lastError = "Unsupported Octave data of class '";
        lastError += v.class_name();
        lastError += "'";
        return false;
    }
    // get h5 datatype
    dtype = oct2hdf(dtype_spec);

    // prepare h5 memory dataspace & octave dimensions
    dspace = DataSpace(H5S_SCALAR);
    dv = v.dims();
    int ndim = dv.ndims(); 
    // find # of data elements
    hsize_t nelem = 1;
    for(int i=0; i<ndim; i++) nelem *= dv(i);
    if (nelem > 1) // if not scalar (octave 1x1)
    {
        vector<hsize_t> dims(ndim);
        for(int i=0; i<ndim; i++) dims[ndim-1-i]=dv(i);        
        dspace = DataSpace(ndim, dims.data());
    } 
    dspace_info.set(dspace);

    ov = v;

    return true;   
}

void hdf5oct::data_exchange::write(const data_exchange& dxfile) 
{
    if (dtype_spec=="double") write_impl<double>(dxfile);
    else if (dtype_spec=="single") write_impl<float>(dxfile);
    else if (dtype_spec=="uint64") write_impl<uint64_t>(dxfile);
    else if (dtype_spec=="int64") write_impl<int64_t>(dxfile);
    else if (dtype_spec=="uint32") write_impl<uint32_t>(dxfile);
    else if (dtype_spec=="int32") write_impl<int32_t>(dxfile);
    else if (dtype_spec=="uint16") write_impl<uint16_t>(dxfile);
    else if (dtype_spec=="int16") write_impl<int16_t>(dxfile);
    else if (dtype_spec=="uint8") write_impl<uint8_t>(dxfile);
    else if (dtype_spec=="int8") write_impl<int8_t>(dxfile);
    else if (dtype_spec=="string") write_string(dxfile);
}

void hdf5oct::data_exchange::write_string(const data_exchange& dxfile)
{
    Array<string> A = ov.cellstr_value();
    octave_idx_type n = A.numel();
    vector<const char*> p(n);
    for(octave_idx_type i=0; i<n; i++) p[i] = A.xelem(i).data();
    dxfile.dset.write(p.data(), dtype, dspace, dxfile.dspace);    
}

octave_value hdf5oct::data_exchange::read_string() 
{
    octave_idx_type n = 1;
    for(octave_idx_type i=0; i<dv.ndims(); i++) n *= dv.xelem(i);

    if (dtype_info.size == H5T_VARIABLE) {        
        vector<char*> p(n);
        Array<string> A(dv); 
        DataSpace memspace =  from_dim_vector(dv);  
        dset.read(p.data(), dtype, memspace, dspace);
        for(octave_idx_type i = 0; i<n; i++) A(i) = p[i] ? string(p[i]) : string();
        herr_t ret = H5Dvlen_reclaim (dtype.getId(), memspace.getId(), H5P_DEFAULT, p.data());       
        if (ret < 0) {
            lastError = "Error in call to H5Dvlen_reclaim";
            return octave_value();
        }
        return octave_value(A);
    } else {
        if (!(dv.ndims()==2 && dv.xelem(1)!=1)) {
            lastError = "loading of multidimensional char arrays is not suppoted";
            return octave_value();
        }
        octave_idx_type sz = dtype_info.size;
        dim_vector dv1(1,sz);
        charNDArray A(dv1);
        dset.read(A.fortran_vec(),dtype,from_dim_vector(dv1),dspace);
        return octave_value(A);
    }
}

octave_value hdf5oct::data_exchange::read() {
    octave_value ret;
    if (dtype_spec=="double") ret = read_impl<double>();
    else if (dtype_spec=="single") ret = read_impl<float>();
    else if (dtype_spec=="uint64") ret = read_impl<uint64_t>();
    else if (dtype_spec=="int64") ret = read_impl<int64_t>();
    else if (dtype_spec=="uint32") ret = read_impl<uint32_t>();
    else if (dtype_spec=="int32") ret = read_impl<int32_t>();
    else if (dtype_spec=="uint16") ret = read_impl<uint16_t>();
    else if (dtype_spec=="int16") ret = read_impl<int16_t>();
    else if (dtype_spec=="uint8") ret = read_impl<uint8_t>();
    else if (dtype_spec=="int8") ret = read_impl<int8_t>();
    else if (dtype_spec=="string") ret = read_string();
    return ret;    
}

DataSpace hdf5oct::data_exchange::from_dim_vector(const dim_vector& dv) {
    int ndim = dv.ndims();
    if (ndim==2 && dv(0)==1 && dv(1)==1) return DataSpace(H5S_SCALAR);
    if (ndim==1 && dv(0)==1) {
        hsize_t dim = dv(1); 
        return DataSpace(1, &dim); 
    }
    vector<hsize_t> dims(ndim);
    for(int i=0; i<ndim; i++) dims[ndim-1-i]=dv(i);        
    return DataSpace(ndim, dims.data());
}