/*
 *
 *    Copyright (C) 2012 Tom Mullins
 *    Copyright (C) 2015 Tom Mullins, Thorsten Liebig, Stefan Großhauser
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
#else
// as a package
#include <octave/oct.h>
#endif

#if defined (HAVE_HDF5) && defined (HAVE_HDF5_18)
#include <highfive/highfive.hpp>

namespace hdf5oct {

template <typename T> struct h5traits;

struct predtype {
    static HighFive::AtomicType<double> NATIVE_DOUBLE;
    static HighFive::AtomicType<float> NATIVE_FLOAT;
    static HighFive::AtomicType<uint64_t> NATIVE_UINT64;
    static HighFive::AtomicType<int64_t> NATIVE_INT64;
    static HighFive::AtomicType<uint32_t> NATIVE_UINT32;
    static HighFive::AtomicType<int32_t> NATIVE_INT32;
    static HighFive::AtomicType<uint16_t> NATIVE_UINT16;
    static HighFive::AtomicType<int16_t> NATIVE_INT16;
    static HighFive::AtomicType<uint8_t> NATIVE_UINT8;
    static HighFive::AtomicType<int8_t> NATIVE_INT8;
    static HighFive::VariableLengthStringType DEFAULT_STRING_TYPE;
};

template<>
struct h5traits<double> {
    typedef NDArray OctaveArray;
    static OctaveArray oct_array(octave_value v) { return v.array_value(); }
    static const HighFive::DataType& predType() { return predtype::NATIVE_DOUBLE; }
};
template<>
struct h5traits<float> {
    typedef FloatNDArray OctaveArray;
    static OctaveArray oct_array(octave_value v) { return v.float_array_value(); }
    static const HighFive::DataType& predType() { return predtype::NATIVE_FLOAT; }
};
template<>
struct h5traits<uint64_t> {
    typedef uint64NDArray OctaveArray;
    static OctaveArray oct_array(octave_value v) { return v.uint64_array_value(); }
    static const HighFive::DataType& predType() { return predtype::NATIVE_UINT64; }
};
template<>
struct h5traits<uint32_t> {
    typedef uint32NDArray OctaveArray;
    static OctaveArray oct_array(octave_value v) { return v.uint32_array_value(); }
    static const HighFive::DataType& predType() { return predtype::NATIVE_UINT32; }
};
template<>
struct h5traits<uint16_t> {
    typedef uint16NDArray OctaveArray;
    static OctaveArray oct_array(octave_value v) { return v.uint16_array_value(); }
    static const HighFive::DataType& predType() { return predtype::NATIVE_UINT16; }
};
template<>
struct h5traits<uint8_t> {
    typedef uint8NDArray OctaveArray;
    static OctaveArray oct_array(octave_value v) { return v.uint8_array_value(); }
    static const HighFive::DataType& predType() { return predtype::NATIVE_UINT8; }
};
template<>
struct h5traits<int64_t> {
    typedef int64NDArray OctaveArray;
    static OctaveArray oct_array(octave_value v) { return v.int64_array_value(); }
    static const HighFive::DataType& predType() { return predtype::NATIVE_INT64; }
};
template<>
struct h5traits<int32_t> {
    typedef int32NDArray OctaveArray;
    static OctaveArray oct_array(octave_value v) { return v.int32_array_value(); }
    static const HighFive::DataType& predType() { return predtype::NATIVE_INT32; }
};
template<>
struct h5traits<int16_t> {
    typedef int16NDArray OctaveArray;
    static OctaveArray oct_array(octave_value v) { return v.int16_array_value(); }
    static const HighFive::DataType& predType() { return predtype::NATIVE_INT16; }
};
template<>
struct h5traits<int8_t> {
    typedef int8NDArray OctaveArray;
    static OctaveArray oct_array(octave_value v) { return v.int8_array_value(); }
    static const HighFive::DataType& predType() { return predtype::NATIVE_INT8; }
};

HighFive::DataType h5type_from_spec(const std::string& dtype_spec);

/**
 * @brief Check if a location exists in a HDF5 file.
 * 
 * If the location path contains intermediate groups, these are also checked.
 * 
 * @param f The HDF5 file object
 * @param loc The location path
 * @return true If the specified location exists
 * @return false If the location or any intermediate groups do not exist
 */
bool locationExists(const HighFive::File& f, const std::string& loc);

struct dspace_info_t {
    std::string extent_type;
    uint64NDArray size;
    uint64NDArray maxsize;
    void set(const HighFive::DataSpace& dspace);
    octave_scalar_map oct_map() const;
    bool needs_chunk() const;
    bool isNull() const { return extent_type=="Empty"; } 
    bool isScalar() const { return extent_type=="Scalar"; } 
    bool isSimple() const { return extent_type=="Simple"; } 
};
struct dtype_info_t {
    std::string h5class; 
    std::string octave_class;
    size_t size;
    std::string signed_status;
    std::string cset;
    std::string pading;
    void set(const HighFive::DataType& dtype);
    octave_scalar_map oct_map() const;
};
struct dset_info_t {
    std::string name;
    dtype_info_t dtype_info;
    dspace_info_t dspace_info;
    uint64NDArray chunksize;
    octave_value fillValue; 
    std::map<std::string,octave_value> attributes;  
    void set(const HighFive::DataSet& ds, const std::string& path); 
    octave_scalar_map oct_map() const;
};
struct named_dtype_info_t : public dtype_info_t {
    std::string name;
    void set(const HighFive::DataType& dtype, const std::string& path) {
        name = path;
        dtype_info_t::set(dtype);
    }
    octave_scalar_map oct_map() const {
        // octave_scalar_map m = dtype_info_t::oct_map(); // this needs fixing
        octave_scalar_map m;
        m.assign("Name",name);
        return m;
    }
};
struct group_info_t {
    std::string name;
    std::vector<group_info_t> groups;
    std::vector<dset_info_t> datasets;
    std::vector<named_dtype_info_t> datatypes;
    std::map<std::string,octave_value> attributes;
    void set(const HighFive::Group& g, const std::string& path); 
    octave_scalar_map oct_map() const;
};


struct data_exchange {
    octave_value ov;
    HighFive::DataSet* dset{nullptr};
    const HighFive::Attribute* attr{nullptr};
    HighFive::DataType dtype;
    dtype_info_t dtype_info;
    HighFive::DataSpace dspace{HighFive::DataSpace::Null()};
    dspace_info_t dspace_info;
    std::string dtype_spec;
    dim_vector dv;
    std::string lastError;
    bool set(octave_value v);
    bool set(HighFive::DataSet* ds);
    bool set(const HighFive::Attribute* attr);
    bool isCompatible(const data_exchange& dx);
    bool isValid() const {
        return !(dtype_spec.empty() || dtype_spec=="unsupported");
    }
    bool isScalar() const {
        return dv.ndims()==2 && dv(0)==1 && dv(1)==1;
    }
    bool selectHyperslab(uint64NDArray start, uint64NDArray count, uint64NDArray stride, bool tryResize);

    octave_value read();
    octave_value read_attribute();
    void write(const data_exchange& dxfile);

    template<class Derivate>
    bool write_as_attribute(Derivate& obj, const std::string& name) {
        if (obj.hasAttribute(name)) obj.deleteAttribute(name);
        HighFive::Attribute attr = obj.createAttribute(name, dspace, dtype);
        return write_as_attribute(attr);
    }

private:
    void reset();

    static HighFive::DataSpace from_dim_vector(const dim_vector& dv);
    static void h5read(const HighFive::DataSet& dset,
                        void* data, 
                        const HighFive::DataType& mem_type, const HighFive::DataSpace& mem_space,
                        const HighFive::DataSpace& file_space,
                        const HighFive::DataTransferProps & xfer_props = HighFive::DataTransferProps())
    {
        HighFive::detail::h5d_read(dset.getId(),
                        mem_type.getId(),
                        mem_space.getId(),
                        file_space.getId(),
                        xfer_props.getId(),
                        data);    
    }
    static void h5write(const HighFive::DataSet& dset,
                        const void* data, 
                        const HighFive::DataType& mem_type, const HighFive::DataSpace& mem_space,
                        const HighFive::DataSpace& file_space,
                        const HighFive::DataTransferProps & xfer_props = HighFive::DataTransferProps())
    {
        HighFive::detail::h5d_write(dset.getId(),
                        mem_type.getId(),
                        mem_space.getId(),
                        file_space.getId(),
                        xfer_props.getId(),
                        data);
    }


    bool set(const HighFive::DataType& t, const HighFive::DataSpace& s);

    template<class T>
    octave_value read_impl()
    {
        typename h5traits<T>::OctaveArray A(dv);
        h5read(*dset,A.fortran_vec(),h5traits<T>::predType(),from_dim_vector(dv),dspace);
        return octave_value(A);
    }  
    template<class T>
    octave_value read_attr_impl()
    {
        typename h5traits<T>::OctaveArray A(dv);
        attr->read(A.fortran_vec(), dtype);
        return octave_value(A);
    } 
    octave_value read_string();
    octave_value read_string_attr();

    template<typename T>
    void write_impl(const data_exchange& dxfile) 
    {
        auto A = h5traits<T>::oct_array(ov);
        h5write(*dxfile.dset,A.fortran_vec(),h5traits<T>::predType(), dspace, dxfile.dspace);
    }
    template<typename T>
    void write_attr_impl(HighFive::Attribute& att) 
    {
        auto A = h5traits<T>::oct_array(ov);
        att.write_raw(A.fortran_vec(), h5traits<T>::predType());    
    }
    void write_string(const data_exchange& dxfile);
    void write_string_attr(HighFive::Attribute& attr);
    bool write_as_attribute(HighFive::Attribute& attr);
};

template<class H5Obj>
std::map<std::string, octave_value> readAttributes(const H5Obj& obj)
{
    std::map<std::string, octave_value> M;
    for(auto& attr_name: obj.listAttributeNames()) {
        hdf5oct::data_exchange dx;
        octave_value ov;
        HighFive::Attribute attr = obj.getAttribute(attr_name);
        if (dx.set(&attr)) ov = dx.read_attribute();
        M[attr_name] = ov;  
    }
    return M;
}


} // namespace hdf5oct



#endif
