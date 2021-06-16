#pragma once
#include "h5ppOptional.h"
#include "h5ppTypeSfinae.h"
#include <cstring>
#include <numeric>

/*!
 * \brief A collection of functions to get information about C++ types passed by the user
 */
namespace h5pp::util {

    [[nodiscard]] inline std::string safe_str(std::string_view str) {
        // This function removes null-terminating characters inside of strings. For instance
        //      "This is \0 a string with\0 embedded null characters"
        // This happens sometimes for instance when concatenating strings that are "non-standard", i.e.
        // strings where .size() returns the "number of characters + 1", where "+1" accounts for '\0'.
        // This can easily happen by accident when mixing C-style arrays and std::string.
        // Since std::string supports embedded null characters '\0' this is normally not a problem.
        // However, when interfacing with the C-API of HDF5, all C-style strings are terminated
        // at the first occurence of '\0'. Therefore we have to make sure that there are no embedded '\0'
        // characters in the strings that we pass to the HDF5 C-API.
        // Note that this function leaves alone any null terminator that is technically in the buffer
        // but outside of .size() (where it is allowed to be!)
        if(str.empty()) return std::string(str);
        std::string tmp(str);
        size_t      start_pos = 0;
        while((start_pos = tmp.find('\0', start_pos)) != std::string::npos) {
            tmp.replace(start_pos, 1, "");
            start_pos += 1;
        }
        return tmp;
    }

    template<typename PtrType, typename DataType>
    [[nodiscard]] inline PtrType getVoidPointer(DataType &data) {
        // Get the memory address to a data buffer

        if constexpr(h5pp::type::sfinae::has_data_v<DataType>)
            return static_cast<PtrType>(data.data());
        else if constexpr(std::is_pointer_v<DataType> or std::is_array_v<DataType>)
            return static_cast<PtrType>(data);
        else
            return static_cast<PtrType>(&data);
    }

    template<typename DataType>
    [[nodiscard]] hid::h5t getH5Type() {
        //        if(h5type.has_value()) return h5type.value(); // Intercept
        namespace tc = h5pp::type::sfinae;
        /* clang-format off */
        using DecayType    = typename std::decay<DataType>::type;
        if constexpr (std::is_pointer_v<DecayType>)                                return getH5Type<typename std::remove_pointer<DecayType>::type>();
        if constexpr (std::is_reference_v<DecayType>)                              return getH5Type<typename std::remove_reference<DecayType>::type>();
        if constexpr (std::is_array_v<DecayType>)                                  return getH5Type<typename std::remove_all_extents<DecayType>::type>();
        if constexpr (tc::is_std_vector_v<DecayType>)                              return getH5Type<typename DecayType::value_type>();
        if constexpr (std::is_same_v<DecayType, short>)                            return H5Tcopy(H5T_NATIVE_SHORT);
        if constexpr (std::is_same_v<DecayType, int>)                              return H5Tcopy(H5T_NATIVE_INT);
        if constexpr (std::is_same_v<DecayType, long>)                             return H5Tcopy(H5T_NATIVE_LONG);
        if constexpr (std::is_same_v<DecayType, long long>)                        return H5Tcopy(H5T_NATIVE_LLONG);
        if constexpr (std::is_same_v<DecayType, unsigned short>)                   return H5Tcopy(H5T_NATIVE_USHORT);
        if constexpr (std::is_same_v<DecayType, unsigned int>)                     return H5Tcopy(H5T_NATIVE_UINT);
        if constexpr (std::is_same_v<DecayType, unsigned long>)                    return H5Tcopy(H5T_NATIVE_ULONG);
        if constexpr (std::is_same_v<DecayType, unsigned long long >)              return H5Tcopy(H5T_NATIVE_ULLONG);
        if constexpr (std::is_same_v<DecayType, double>)                           return H5Tcopy(H5T_NATIVE_DOUBLE);
        if constexpr (std::is_same_v<DecayType, long double>)                      return H5Tcopy(H5T_NATIVE_LDOUBLE);
        if constexpr (std::is_same_v<DecayType, float>)                            return H5Tcopy(H5T_NATIVE_FLOAT);
        if constexpr (std::is_same_v<DecayType, bool>)                             return H5Tcopy(H5T_NATIVE_UINT8);
        if constexpr (std::is_same_v<DecayType, std::string>)                      return H5Tcopy(H5T_C_S1);
        if constexpr (std::is_same_v<DecayType, char>)                             return H5Tcopy(H5T_C_S1);
        if constexpr (std::is_same_v<DecayType, std::complex<short>>)              return H5Tcopy(h5pp::type::compound::H5T_COMPLEX_SHORT);
        if constexpr (std::is_same_v<DecayType, std::complex<int>>)                return H5Tcopy(h5pp::type::compound::H5T_COMPLEX_INT);
        if constexpr (std::is_same_v<DecayType, std::complex<long>>)               return H5Tcopy(h5pp::type::compound::H5T_COMPLEX_LONG);
        if constexpr (std::is_same_v<DecayType, std::complex<long long>>)          return H5Tcopy(h5pp::type::compound::H5T_COMPLEX_LLONG);
        if constexpr (std::is_same_v<DecayType, std::complex<unsigned short>>)     return H5Tcopy(h5pp::type::compound::H5T_COMPLEX_USHORT);
        if constexpr (std::is_same_v<DecayType, std::complex<unsigned int>>)       return H5Tcopy(h5pp::type::compound::H5T_COMPLEX_UINT);
        if constexpr (std::is_same_v<DecayType, std::complex<unsigned long>>)      return H5Tcopy(h5pp::type::compound::H5T_COMPLEX_ULONG);
        if constexpr (std::is_same_v<DecayType, std::complex<unsigned long long>>) return H5Tcopy(h5pp::type::compound::H5T_COMPLEX_ULLONG);
        if constexpr (std::is_same_v<DecayType, std::complex<double>>)             return H5Tcopy(h5pp::type::compound::H5T_COMPLEX_DOUBLE);
        if constexpr (std::is_same_v<DecayType, std::complex<long double>>)        return H5Tcopy(h5pp::type::compound::H5T_COMPLEX_LDOUBLE);
        if constexpr (std::is_same_v<DecayType, std::complex<float>>)              return H5Tcopy(h5pp::type::compound::H5T_COMPLEX_FLOAT);
        if constexpr (tc::is_Scalar2_of_type<DecayType, short>())                  return H5Tcopy(h5pp::type::compound::H5T_SCALAR2_SHORT);
        if constexpr (tc::is_Scalar2_of_type<DecayType, int>())                    return H5Tcopy(h5pp::type::compound::H5T_SCALAR2_INT);
        if constexpr (tc::is_Scalar2_of_type<DecayType, long>())                   return H5Tcopy(h5pp::type::compound::H5T_SCALAR2_LONG);
        if constexpr (tc::is_Scalar2_of_type<DecayType, long long>())              return H5Tcopy(h5pp::type::compound::H5T_SCALAR2_LLONG);
        if constexpr (tc::is_Scalar2_of_type<DecayType, unsigned short>())         return H5Tcopy(h5pp::type::compound::H5T_SCALAR2_USHORT);
        if constexpr (tc::is_Scalar2_of_type<DecayType, unsigned int>())           return H5Tcopy(h5pp::type::compound::H5T_SCALAR2_UINT);
        if constexpr (tc::is_Scalar2_of_type<DecayType, unsigned long>())          return H5Tcopy(h5pp::type::compound::H5T_SCALAR2_ULONG);
        if constexpr (tc::is_Scalar2_of_type<DecayType, unsigned long long>())     return H5Tcopy(h5pp::type::compound::H5T_SCALAR2_ULLONG);
        if constexpr (tc::is_Scalar2_of_type<DecayType, double>())                 return H5Tcopy(h5pp::type::compound::H5T_SCALAR2_DOUBLE);
        if constexpr (tc::is_Scalar2_of_type<DecayType, long double>())            return H5Tcopy(h5pp::type::compound::H5T_SCALAR2_LDOUBLE);
        if constexpr (tc::is_Scalar2_of_type<DecayType, float>())                  return H5Tcopy(h5pp::type::compound::H5T_SCALAR2_FLOAT);
        if constexpr (tc::is_Scalar3_of_type<DecayType, short>())                  return H5Tcopy(h5pp::type::compound::H5T_SCALAR3_SHORT);
        if constexpr (tc::is_Scalar3_of_type<DecayType, int>())                    return H5Tcopy(h5pp::type::compound::H5T_SCALAR3_INT);
        if constexpr (tc::is_Scalar3_of_type<DecayType, long>())                   return H5Tcopy(h5pp::type::compound::H5T_SCALAR3_LONG);
        if constexpr (tc::is_Scalar3_of_type<DecayType, long long>())              return H5Tcopy(h5pp::type::compound::H5T_SCALAR3_LLONG);
        if constexpr (tc::is_Scalar3_of_type<DecayType, unsigned short>())         return H5Tcopy(h5pp::type::compound::H5T_SCALAR3_USHORT);
        if constexpr (tc::is_Scalar3_of_type<DecayType, unsigned int>())           return H5Tcopy(h5pp::type::compound::H5T_SCALAR3_UINT);
        if constexpr (tc::is_Scalar3_of_type<DecayType, unsigned long>())          return H5Tcopy(h5pp::type::compound::H5T_SCALAR3_ULONG);
        if constexpr (tc::is_Scalar3_of_type<DecayType, unsigned long long>())     return H5Tcopy(h5pp::type::compound::H5T_SCALAR3_ULLONG);
        if constexpr (tc::is_Scalar3_of_type<DecayType, double>())                 return H5Tcopy(h5pp::type::compound::H5T_SCALAR3_DOUBLE);
        if constexpr (tc::is_Scalar3_of_type<DecayType, long double>())            return H5Tcopy(h5pp::type::compound::H5T_SCALAR3_LDOUBLE);
        if constexpr (tc::is_Scalar3_of_type<DecayType, float>())                  return H5Tcopy(h5pp::type::compound::H5T_SCALAR3_FLOAT);
        if constexpr (tc::has_Scalar_v <DecayType>)                                return getH5Type<typename DecayType::Scalar>();
        if constexpr (tc::has_value_type_v <DecayType>)                            return getH5Type<typename DataType::value_type>();
        if constexpr (std::is_class_v<DataType>) return H5Tcreate(H5T_COMPOUND, sizeof(DataType)); // Last resort

        /* clang-format on */
        h5pp::logger::log->critical("getH5Type could not match the type provided: {}", type::sfinae::type_name<DecayType>());
        throw std::logic_error(h5pp::format("getH5Type could not match the type provided [{}] | size {}",
                                            type::sfinae::type_name<DecayType>(),
                                            sizeof(DecayType)));
        return hid_t(0);
    }

    template<typename DataType, size_t size>
    [[nodiscard]] constexpr size_t getArraySize([[maybe_unused]] const DataType (&arr)[size], [[maybe_unused]] bool countChars = false)
        noexcept
    /*! Returns the size of a C-style array.
    */
    {
        if constexpr(h5pp::type::sfinae::is_text_v<DataType>) {
            // A C-style char array is a null-terminated array, that has size = characters + 1
            // Here we want to return the number of characters that can fit in the array,
            // and we are not interested in the number of characters currently there.
            // To reverse this behavior, use "countChars == true".
            // To avoid strnlen, use std::string_view constructor to count chars up to (but not including) '\0'.
            if(countChars)
                return std::min(std::string_view(arr).size(), size) + 1; // Add null-terminator
            else
                return std::max(std::string_view(arr).size(), size - 1) + 1; // Include null terminator
        } else
            return size;
    }
    template<typename DataType, size_t rows, size_t cols>
    [[nodiscard]] constexpr std::array<size_t, 2> getArraySize([[maybe_unused]] const DataType (&arr)[rows][cols]) noexcept {
        return {rows, cols};
    }
    template<typename DataType, size_t rows, size_t cols, size_t depth>
    [[nodiscard]] constexpr std::array<size_t, 3> getArraySize([[maybe_unused]] const DataType (&arr)[rows][cols][depth]) noexcept{
        return {rows, cols, depth};
    }

    template<typename DataType, typename = std::enable_if_t<not std::is_base_of_v<hid::hid_base<DataType>, DataType>>>
    [[nodiscard]] size_t getCharArraySize(const DataType &data, [[maybe_unused]] bool countChars = true) noexcept {
        static_assert(h5pp::type::sfinae::is_text_v<DataType>,
                      "Template function [h5pp::util::getCharArraySize(const DataType & data)] requires type DataType to be "
                      "a text-like type such as [std::string], [std::string_view], [char *] or have a .c_str() member function");
        // With this function we are interested in the number of chars currently in a given buffer,
        // including the null terminator.
        if constexpr(h5pp::type::sfinae::has_size_v<DataType>) return data.size() + 1; // string and string_view have size without \0
        if constexpr(std::is_array_v<DataType>) return getArraySize(data, countChars); // getarraysize includes nullterm already
        if constexpr(h5pp::type::sfinae::has_c_str_v<DataType>) return strlen(data.c_str()) + 1; // strlen does not include \0
        if constexpr(std::is_pointer_v<DataType>) return strlen(data) + 1;                       // strlen does not include \0
        return 1;                                                                                // Probably a char?
    }

    template<typename DimType = std::initializer_list<hsize_t>>
    [[nodiscard]] std::vector<hsize_t> getDimVector(const DimType &dims) {
        static_assert(h5pp::type::sfinae::is_integral_iterable_or_num_v<DimType>,
                      "Template function [h5pp::util::getDimVector(const DimType & dims)] requires type DimType to be "
                      "an integral type e.g. [int,long,size_t...] or "
                      "an iterable container of integral types e.g. [std::vector<int>, std::initializer_list<size_t>] ...");
        if constexpr(std::is_same_v<DimType, std::vector<hsize_t>>)
            return dims;
        else {
            if constexpr(h5pp::type::sfinae::is_iterable_v<DimType>) {
                std::vector<hsize_t> dimVec;
                std::copy(std::begin(dims), std::end(dims), std::back_inserter(dimVec));
                return dimVec;
            } else if constexpr(std::is_integral_v<DimType>) {
                return std::vector<hsize_t>{static_cast<hsize_t>(dims)};
            } else
                static_assert(h5pp::type::sfinae::invalid_type_v<DimType>,
                              "Template function [h5pp::util::getDimVector(const DimType & dims)] failed to statically detect "
                              "an invalid type for DimsType. Please submit a bug report.");
        }
    }

    template<typename DimType = std::initializer_list<hsize_t>>
    [[nodiscard]] std::optional<std::vector<hsize_t>> getOptionalDimVector(const DimType &dims) {
        static_assert(h5pp::type::sfinae::is_integral_iterable_num_or_nullopt_v<DimType>,
                      "Template function [h5pp::util::getOptionalDimVector(const DimType & dims)] requires type DimType to be "
                      "an std::nullopt, an integral type e.g. [int,long,size_t...] or "
                      "an iterable container of integral types e.g. [std::vector<int>, std::initializer_list<size_t>] ...");
        if constexpr(std::is_same_v<DimType, std::nullopt_t>)
            return dims;
        else
            return getDimVector(dims);
    }

    [[nodiscard]] inline hsize_t getSizeFromDimensions(const std::vector<hsize_t> &dims) {
        return std::accumulate(dims.begin(), dims.end(), static_cast<hsize_t>(1), std::multiplies<>());
    }

    template<typename DataType, typename = std::enable_if_t<not std::is_base_of_v<hid::hid_base<DataType>, DataType>>>
    [[nodiscard]] hsize_t getSize(const DataType &data) {
        if constexpr(h5pp::type::sfinae::is_text_v<DataType>)
            return static_cast<hsize_t>(1); // Strings and char arrays are treated as a single unit
        if constexpr(h5pp::type::sfinae::has_size_v<DataType>) return static_cast<hsize_t>(data.size());
        if constexpr(std::is_array_v<DataType>) return static_cast<hsize_t>(getArraySize(data));
        if constexpr(std::is_pointer_v<DataType>)
            throw std::runtime_error("Failed to read data size: Pointer data has no specified dimensions");
        // Add more checks here. As it is, these two checks above handle all cases I have encountered.
        return 1; // All others should be "H5S_SCALAR" of size 1.
    }

    template<typename DataType, typename = std::enable_if_t<not std::is_base_of_v<hid::hid_base<DataType>, DataType>>>
    [[nodiscard]] constexpr int getRank() {
#ifdef H5PP_EIGEN3
        if constexpr(h5pp::type::sfinae::is_eigen_tensor_v<DataType>) return static_cast<int>(DataType::NumIndices);
        if constexpr(h5pp::type::sfinae::is_eigen_1d_v<DataType>) return 1;
        if constexpr(h5pp::type::sfinae::is_eigen_dense_v<DataType>) return 2;
#endif
        if constexpr(h5pp::type::sfinae::is_text_v<DataType>) return 0;
        if constexpr(h5pp::type::sfinae::has_NumIndices_v<DataType>) return static_cast<int>(DataType::NumIndices);
        if constexpr(std::is_array_v<DataType>) return std::rank_v<DataType>;
        if constexpr(h5pp::type::sfinae::has_size_v<DataType>) return 1;
        return 0;
    }

    [[nodiscard]] inline int getRankFromDimensions(const std::vector<hsize_t> &dims) { return static_cast<int>(dims.size()); }

    template<typename DataType, typename = std::enable_if_t<not std::is_base_of_v<hid::hid_base<DataType>, DataType>>>
    [[nodiscard]] std::vector<hsize_t> getDimensions(const DataType &data) {
        // Empty vector means H5S_SCALAR, size 1 and rank 0
        if constexpr(h5pp::type::sfinae::is_text_v<DataType>) return {};
        constexpr int rank = getRank<DataType>();
        if constexpr(rank == 0) return {};
        if constexpr(h5pp::type::sfinae::has_dimensions_v<DataType>) {
            std::vector<hsize_t> dims(
                data.dimensions().begin(),
                data.dimensions()
                    .end()); // We copy because the vectors may not be assignable or may not be implicitly convertible to hsize_t.
            if(data.dimensions().size() != rank) throw std::runtime_error("given dimensions do not match detected rank");
            if(dims.size() != rank) throw std::runtime_error("copied dimensions do not match detected rank");
            return dims;
        } else if constexpr(h5pp::type::sfinae::has_size_v<DataType> and rank == 1)
            return {static_cast<hsize_t>(data.size())};

#ifdef H5PP_EIGEN3
        else if constexpr(h5pp::type::sfinae::is_eigen_tensor_v<DataType>) {
            if(data.dimensions().size() != rank) throw std::runtime_error("given dimensions do not match detected rank");
            std::vector<hsize_t> dims(
                data.dimensions().begin(),
                data.dimensions()
                    .end()); // We copy because the vectors may not be assignable or may not be implicitly convertible to hsize_t.
            return dims;
        } else if constexpr(h5pp::type::sfinae::is_eigen_dense_v<DataType>) {
            std::vector<hsize_t> dims(rank);
            dims[0] = static_cast<hsize_t>(data.rows());
            dims[1] = static_cast<hsize_t>(data.cols());
            return dims;
        }
#endif
        else if constexpr(h5pp::type::sfinae::is_ScalarN<DataType>())
            return {};
        else if constexpr(std::is_array_v<DataType>)
            return {getArraySize(data)};
        else if constexpr(std::is_arithmetic_v<DataType> or h5pp::type::sfinae::is_std_complex_v<DataType>)
            return {};
        else if constexpr(std::is_pod_v<DataType>)
            return {};
        else if constexpr(std::is_standard_layout_v<DataType>)
            return {};
        else if constexpr(std::is_class_v<DataType>) {
            h5pp::logger::log->warn("Detected possible unsupported non-POD class. h5pp may fail.");
            return {};
        } else {
            throw std::logic_error(
                h5pp::format("getDimensions can't match the type provided [{}]", h5pp::type::sfinae::type_name<DataType>()));
        }
    }

    [[nodiscard]] inline std::optional<std::vector<hsize_t>> decideDimensionsMax(const std::vector<hsize_t> &dims,
                                                                                 std::optional<H5D_layout_t> h5_layout) {
        /* From the docs
         *  Any element of current_dims can be 0 (zero).
         *  Note that no data can be written to a dataset if the size of any dimension of its current dataspace is 0.
         *  This is sometimes a useful initial state for a dataset.
         *  Maximum_dims may be the null pointer, in which case the upper limit is the same as current_dims.
         *  Otherwise, no element of maximum_dims should be smaller than the corresponding element of current_dims.
         *  If an element of maximum_dims is H5S_UNLIMITED, the maximum size of the corresponding dimension is unlimited.
         *  Any dataset with an unlimited dimension must also be chunked; see H5Pset_chunk.
         *  Similarly, a dataset must be chunked if current_dims does not equal maximum_dims.
         */
        if(h5_layout and h5_layout.value() != H5D_CHUNKED) return std::nullopt;
        std::vector<hsize_t> dimsMax = dims;
        if(h5_layout == H5D_CHUNKED) std::fill_n(dimsMax.begin(), dims.size(), H5S_UNLIMITED);
        return dimsMax;
    }

    [[nodiscard]] inline hid::h5s getDsetSpace(const hsize_t                       size,
                                               const std::vector<hsize_t> &        dims,
                                               const H5D_layout_t &                h5Layout,
                                               std::optional<std::vector<hsize_t>> dimsMax = std::nullopt) {
        if(dims.empty() and size > 0)
            return H5Screate(H5S_SCALAR);
        else if(dims.empty() and size == 0)
            return H5Screate(H5S_NULL);
        else {
            auto num_elements = h5pp::util::getSizeFromDimensions(dims);
            if(size != num_elements)
                throw std::runtime_error(h5pp::format("Number of elements mismatch: size {} | dimensions {}", size, dims));

            // Only and chunked and datasets can be extended. The extension can happen up to the max dimension specified.
            // If the max dimension is H5S_UNLIMITED, then the dataset can grow to any dimension.
            // Conversely, if the dataset is not H5D_CHUNKED, then dims == max dims must hold always

            if(dimsMax) {
                // Here dimsMax was given by the user and we have to do some sanity checks
                // Check that the ranks match
                if(dimsMax and dimsMax->size() != dims.size())
                    throw std::runtime_error(h5pp::format("Number of dimensions (rank) mismatch: dims {} | max dims {}\n"
                                                          "\t Hint: Dimension lists must have the same number of elements",
                                                          dims,
                                                          dimsMax.value()));
                // Check that H5S_UNLIMITED is only given to H5D_CHUNKED datasets
                for(size_t idx = 0; idx < dimsMax->size(); idx++)
                    if(dimsMax.value()[idx] == H5S_UNLIMITED and h5Layout != H5D_CHUNKED)
                        throw std::runtime_error(
                            h5pp::format("Max dimensions {} has an H5S_UNLIMITED dimension at index {}. This requires H5D_CHUNKED layout",
                                         dimsMax.value(),
                                         idx));

                if(dimsMax.value() != dims) {
                    // Only H5D_CHUNKED layout can have since dimsMax != dims.
                    // Therefore give an informative error if not H5D_CHUNKED
                    if(h5Layout == H5D_COMPACT)
                        throw std::runtime_error(
                            h5pp::format("Dimension mismatch: dims {} != max dims {}. Equality is required for H5D_COMPACT layout",
                                         dims,
                                         dimsMax.value()));
                    if(h5Layout == H5D_CONTIGUOUS)
                        throw std::runtime_error(
                            h5pp::format("Dimension mismatch: dims {} != max dims {}. Equality is required for H5D_CONTIGUOUS layout",
                                         dims,
                                         dimsMax.value()));
                }

            } else if(h5Layout == H5D_CHUNKED) {
                // Here dimsMax was not given, but H5D_CHUNKED was asked for
                dimsMax = dims;
                std::fill_n(dimsMax->begin(), dims.size(), H5S_UNLIMITED);
            } else
                dimsMax = dims;

            return H5Screate_simple(static_cast<int>(dims.size()), dims.data(), dimsMax->data());
        }
    }

    /*
     * memspace is a description of the buffer in memory (i.e. where read elements will go).
     * If there is no data conversion, then data is read directly into the user supplied buffer.
     * If there is data conversion, HDF5 uses a 1MB buffer to do the conversions,
     * but we still use the user's buffer for reading data in the first place.
     * Also, you can adjust the 1MB default conversion buffer size. (see H5Pset_buffer)
     */
    [[nodiscard]] inline hid::h5s getMemSpace(const hsize_t size, const std::vector<hsize_t> &dims) {
        if(dims.empty() and size > 0)
            return H5Screate(H5S_SCALAR);
        else if(dims.empty() and size == 0)
            return H5Screate(H5S_NULL);
        else {
            auto num_elements = getSizeFromDimensions(dims);
            if(size != num_elements)
                throw std::runtime_error(h5pp::format("Number of elements mismatch: size {} | dimensions {}", size, dims));
            return H5Screate_simple(static_cast<int>(dims.size()), dims.data(), nullptr);
        }
    }

    template<typename DataType, typename = std::enable_if_t<not std::is_base_of_v<hid::hid_base<DataType>, DataType>>>
    [[nodiscard]] size_t getBytesPerElem() {
        namespace sfn   = h5pp::type::sfinae;
        using DecayType = typename std::decay<DataType>::type;
        if constexpr(std::is_pointer_v<DecayType>) return getBytesPerElem<typename std::remove_pointer<DecayType>::type>();
        if constexpr(std::is_reference_v<DecayType>) return getBytesPerElem<typename std::remove_reference<DecayType>::type>();
        if constexpr(std::is_array_v<DecayType>) return getBytesPerElem<typename std::remove_all_extents<DecayType>::type>();
        if constexpr(sfn::is_std_complex<DecayType>()) return sizeof(DecayType);
        if constexpr(sfn::is_ScalarN<DecayType>()) return sizeof(DecayType);
        if constexpr(std::is_arithmetic_v<DecayType>) return sizeof(DecayType);
        if constexpr(sfn::has_Scalar_v<DecayType>) return sizeof(typename DecayType::Scalar);
        if constexpr(sfn::has_value_type_v<DecayType>) return getBytesPerElem<typename DecayType::value_type>();
        return sizeof(std::remove_all_extents_t<DecayType>);
    }

    template<typename DataType, typename = std::enable_if_t<not std::is_base_of_v<hid::hid_base<DataType>, DataType>>>
    [[nodiscard]] size_t getBytesTotal(const DataType &data, std::optional<size_t> size = std::nullopt) {
        if constexpr(h5pp::type::sfinae::is_iterable_v<DataType> and h5pp::type::sfinae::has_value_type_v<DataType>) {
            using value_type = typename DataType::value_type;
            if constexpr(h5pp::type::sfinae::has_size_v<value_type>) { // E.g. std::vector<std::string>
                // Count all the null terminators
                size_t num      = 0;
                size_t nullterm = h5pp::type::sfinae::is_text_v<value_type> ? 1 : 0;
                for(auto &elem : data) num += elem.size() + nullterm;
                return num * h5pp::util::getBytesPerElem<value_type>();
            } else if constexpr(h5pp::type::sfinae::has_size_v<DataType>) { // E.g. std::string or std::vector<double>
                size_t num      = 0;
                size_t nullterm = h5pp::type::sfinae::is_text_v<DataType> ? 1 : 0;
                num             = data.size() + nullterm;
                return num * h5pp::util::getBytesPerElem<value_type>();
            }
        }
        auto bytesperelem = h5pp::util::getBytesPerElem<DataType>();
        if constexpr(h5pp::type::sfinae::is_text_v<DataType>) {
            if(not size) size = getCharArraySize(data) + 1; // Add null terminator
            return size.value() * bytesperelem;
        }
        if constexpr(std::is_array_v<DataType>) {
            if(not size) size = h5pp::util::getArraySize(data);
            return size.value() * bytesperelem;
        }
        if constexpr(std::is_pointer_v<DataType>) {
            if(not size)
                throw std::runtime_error("Could not determine total amount of bytes in buffer: Pointer data has no specified size");
            return size.value() * bytesperelem;
        }

        if(not size) size = h5pp::util::getSize(data);
        return size.value() * bytesperelem;
    }

    [[nodiscard]] inline H5D_layout_t decideLayout(const size_t totalBytes) {
        /*! Depending on the size of this dataset we may benefint from using either
            a contiguous layout (for big non-extendable non-compressible datasets),
            a chunked layout (for extendable and compressible datasets)
            or a compact layout (for tiny datasets).

            Contiguous
            For big non-extendable non-compressible datasets

            Chunked
            Chunking is required for enabling compression and other filters, as well as for
            creating extendible or unlimited dimension datasets. Note that a chunk always has
            the same rank as the dataset and the chunk's dimensions do not need to be factors
            of the dataset dimensions.

            Compact
            A compact dataset is one in which the raw data is stored in the object header of the dataset.
            This layout is for very small datasets that can easily fit in the object header.
            The compact layout can improve storage and access performance for files that have many very
            tiny datasets. With one I/O access both the header and data values can be read.
            The compact layout reduces the size of a file, as the data is stored with the header which
            will always be allocated for a dataset. However, the object header is 64 KB in size,
            so this layout can only be used for very small datasets.
         */
        // Otherwise we decide based on size
        if(totalBytes < h5pp::constants::maxSizeCompact)
            return H5D_COMPACT;
        else if(totalBytes < h5pp::constants::maxSizeContiguous)
            return H5D_CONTIGUOUS;
        else
            return H5D_CHUNKED;
    }

    template<typename DataType>
    [[nodiscard]] inline H5D_layout_t
        decideLayout(const DataType &data, std::optional<std::vector<hsize_t>> dsetDims, std::optional<std::vector<hsize_t>> dsetDimsMax) {
        // Here we try to compute the maximum number of bytes of this dataset, so we can decide its layout
        hsize_t size = 0;
        if(dsetDimsMax) {
            if(dsetDims and dsetDims.value() != dsetDimsMax.value()) return H5D_CHUNKED;
            for(auto &dim : dsetDimsMax.value())
                if(dim == H5S_UNLIMITED) return H5D_CHUNKED;
            size = getSizeFromDimensions(dsetDimsMax.value());
        } else if(dsetDims)
            size = getSizeFromDimensions(dsetDims.value());
        else
            size = getSize(data);
        auto bytes = size * getBytesPerElem<DataType>();
        return decideLayout(bytes);
    }

    inline std::optional<std::vector<hsize_t>> getChunkDimensions(size_t                              bytesPerElem,
                                                                  const std::vector<hsize_t> &        dims,
                                                                  std::optional<std::vector<hsize_t>> dimsMax,
                                                                  std::optional<H5D_layout_t>         layout) {
        // Here we make a naive guess for chunk dimensions
        // We try to make a square in N dimensions with a target byte size of 10kb - 1MB.
        // Here is a great read for chunking considerations https://www.oreilly.com/library/view/python-and-hdf5/9781491944981/ch04.html
        // Hard rules for chunk dimensions:
        //  * A chunk dimension cannot be larger than the corresponding max dimension
        //  * A chunk dimension can be larger or smaller than the dataset dimension
        //  *

        if(layout and layout.value() != H5D_CHUNKED) return std::nullopt;
        if(dims.empty()) return dims; // Scalar
        // We try to compute the maximum dimension to get a good estimate
        // of the data volume
        std::vector<hsize_t> dims_effective = dims;
        // Make sure the chunk dimensions have strictly nonzero volume
        for(auto &dim : dims_effective) dim = std::max<hsize_t>(1, dim);
        if(dimsMax) {
            // If max dims are given, dims that are not H5S_UNLIMITED are used as an upper bound
            // for that dimension
            if(dimsMax->size() != dims.size())
                throw std::runtime_error(h5pp::format("Could not get chunk dimensions: "
                                                      "dims {} and max dims {} have different number of elements",
                                                      dims,
                                                      dimsMax.value()));
            for(size_t idx = 0; idx < dims.size(); idx++) {
                if(dimsMax.value()[idx] < dims[idx])
                    throw std::runtime_error(h5pp::format("Could not get chunk dimensions: "
                                                          "Some elements in dims exceed max dims: "
                                                          "dims {} | max dims {}",
                                                          dims,
                                                          dimsMax.value()));
                if(dimsMax.value()[idx] != H5S_UNLIMITED) dims_effective[idx] = std::max(dims_effective[idx], dimsMax.value()[idx]);
            }
        }

        auto rank = dims.size();
        auto volumeChunkBytes =
            static_cast<size_t>(std::pow(*std::max_element(dims_effective.begin(), dims_effective.end()), rank)) * bytesPerElem;
        auto targetChunkBytes = std::max<size_t>(volumeChunkBytes, h5pp::constants::minChunkSize);
        targetChunkBytes      = std::min<size_t>(targetChunkBytes, h5pp::constants::maxChunkSize);
        targetChunkBytes      = static_cast<size_t>(std::pow(2, std::ceil(std::log2(targetChunkBytes)))); // Next nearest power of two
        auto linearChunkSize =
            static_cast<hsize_t>(std::ceil(std::pow<size_t>(targetChunkBytes / bytesPerElem, 1.0 / static_cast<double>(rank))));
        std::vector<hsize_t> chunkDims(rank, linearChunkSize);
        // Now effective dims contains either dims or dimsMax (if not H5S_UNLIMITED) at each position.
        for(size_t idx = 0; idx < chunkDims.size(); idx++) {
            if(dimsMax and dimsMax.value()[idx] == H5S_UNLIMITED)
                chunkDims[idx] = linearChunkSize;
            else if(dimsMax and dimsMax.value()[idx] != H5S_UNLIMITED)
                chunkDims[idx] = std::min(dimsMax.value()[idx], linearChunkSize);
            else
                chunkDims[idx] = linearChunkSize;
        }
        h5pp::logger::log->debug("Estimated reasonable chunk dimensions: {}", chunkDims);
        return chunkDims;
    }

    template<typename DataType>
    inline void setStringSize(const DataType &data, hsize_t &size, size_t &bytes, std::vector<hsize_t> &dims) {
        // Case 1: data is actual text, such as char* or std::string
        // Case 1A: No dimensions were given, so the dataset is scalar (no extents)
        // Case 1B: Dimensions were given. Force into scalar but only take as many bytes
        //          as implied from given dimensions
        // Case 2: data is a container of strings such as std::vector<std::string>
        if constexpr(h5pp::type::sfinae::is_text_v<DataType>) {
            if(dims.empty()) {
                // Case 1A
                bytes = h5pp::util::getBytesTotal(data);
            } else {
                // Case 1B
                hsize_t desiredSize = h5pp::util::getSizeFromDimensions(dims);
                dims                = {};
                size                = 1;
                bytes               = desiredSize * h5pp::util::getBytesPerElem<DataType>();
            }
        } else if(h5pp::type::sfinae::has_text_v<DataType>) {
            // Case 2
            bytes = h5pp::util::getBytesTotal(data);
        }
    }

    template<typename DataType>
    inline void resizeData(DataType &data, const std::vector<hsize_t> &newDims) {
        // This function may shrink a container!
#ifdef H5PP_EIGEN3
        if constexpr(h5pp::type::sfinae::is_eigen_dense_v<DataType> and h5pp::type::sfinae::is_eigen_1d_v<DataType>) {
            auto newSize = getSizeFromDimensions(newDims);
            if(newDims.size() != 1)
                h5pp::logger::log->debug("Resizing given 1-dimensional Eigen type [{}] to fit dataset dimensions {}",
                                         type::sfinae::type_name<DataType>(),
                                         newDims);
            h5pp::logger::log->debug("Resizing eigen 1d container {} -> {}",
                                     std::initializer_list<Eigen::Index>{data.size()},
                                     std::initializer_list<hsize_t>{newSize});
            data.resize(static_cast<Eigen::Index>(newSize));
        } else if constexpr(h5pp::type::sfinae::is_eigen_dense_v<DataType> and not h5pp::type::sfinae::is_eigen_1d_v<DataType>) {
            if(newDims.size() != 2)
                throw std::runtime_error(h5pp::format("Failed to resize 2-dimensional Eigen type: Dataset has dimensions {}", newDims));
            h5pp::logger::log->debug("Resizing eigen 2d container {} -> {}",
                                     std::initializer_list<Eigen::Index>{data.rows(), data.cols()},
                                     newDims);
            data.resize(static_cast<Eigen::Index>(newDims[0]), static_cast<Eigen::Index>(newDims[1]));
        } else if constexpr(h5pp::type::sfinae::is_eigen_tensor_v<DataType>) {
            if constexpr(h5pp::type::sfinae::has_resize_v<DataType>) {
                if(newDims.size() != DataType::NumDimensions)
                    throw std::runtime_error(h5pp::format("Failed to resize {}-dimensional Eigen tensor: Dataset has dimensions {}",
                                                          DataType::NumDimensions,
                                                          newDims));
                auto eigenDims = eigen::copy_dims<DataType::NumDimensions>(newDims);
                h5pp::logger::log->debug("Resizing eigen tensor container {} -> {}", data.dimensions(), newDims);
                data.resize(eigenDims);
            } else {
                auto newSize = getSizeFromDimensions(newDims);
                if(data.size() != static_cast<Eigen::Index>(newSize))
                    h5pp::logger::log->warn("Detected non-resizeable tensor container with wrong size: Given size {}. Required size {}",
                                            data.size(),
                                            newSize);
            }
        } else
#endif // H5PP_EIGEN3
            if constexpr(h5pp::type::sfinae::has_size_v<DataType> and h5pp::type::sfinae::has_resize_v<DataType>) {
            if(newDims.size() > 1)
                h5pp::logger::log->debug(
                    "Given data container is 1-dimensional but the desired dimensions are {}. Resizing to fit all the data",
                    newDims);
            auto newSize = getSizeFromDimensions(newDims);
            h5pp::logger::log->debug("Resizing 1d container {} -> {} of type [{}]",
                                     std::initializer_list<size_t>{static_cast<size_t>(data.size())},
                                     newDims,
                                     h5pp::type::sfinae::type_name<DataType>());
            data.resize(newSize);
        } else if constexpr(std::is_scalar_v<DataType> or std::is_class_v<DataType>) {
            return;
        } else {
            h5pp::logger::log->debug("Container could not be resized");
        }
    }

    template<typename h5xa,
             typename h5xb,
             // enable_if so the compiler doesn't think it can use overload with fs::path those arguments
             typename = h5pp::type::sfinae::enable_if_is_h5_loc_or_hid_t<h5xa>,
             typename = h5pp::type::sfinae::enable_if_is_h5_loc_or_hid_t<h5xb>>
    [[nodiscard]] inline bool onSameFile(const h5xa &loca, const h5xb &locb, LocationMode locMode = LocationMode::DETECT) {
        switch(locMode) {
            case LocationMode::SAME_FILE: return true;
            case LocationMode::OTHER_FILE: return false;
            case LocationMode::DETECT: {
                hid::h5f filea;
                hid::h5f fileb;
                if constexpr(std::is_same_v<h5xa, hid::h5f>)
                    filea = loca;
                else
                    filea = H5Iget_file_id(loca);
                if constexpr(std::is_same_v<h5xb, hid::h5f>)
                    fileb = locb;
                else
                    fileb = H5Iget_file_id(locb);
                return filea == fileb;
            }
            default: throw std::runtime_error("Unhandled switch case for locMode");
        }
    }

    [[nodiscard]] inline bool
        onSameFile(const h5pp::fs::path &patha, const h5pp::fs::path &pathb, LocationMode locMode = LocationMode::DETECT) {
        switch(locMode) {
            case LocationMode::SAME_FILE: return true;
            case LocationMode::OTHER_FILE: return false;
            case LocationMode::DETECT: {
                return h5pp::fs::equivalent(patha, pathb);
            }
        }
    }

    [[nodiscard]] inline LocationMode getLocationMode(const h5pp::fs::path &patha, const h5pp::fs::path &pathb) {
        if(h5pp::fs::equivalent(patha, pathb))
            return LocationMode::SAME_FILE;
        else
            return LocationMode::OTHER_FILE;
    }

}
