#pragma once

// --- STL Includes ---
#include <iosfwd> // istream
#include <vector> // vector
#include <stdexcept> // runtime_error


namespace mtx2img {


namespace format {


enum class Object
{
    Matrix,         // <== supported
    Vector          // <== unsupported, planned
}; // enum class Object


enum class Format
{
    Coordinate,     // <== supported (sparse representation)
    Array           // <== unsupported, planned (dense representation)
}; // enum class Format


enum class Data
{
    Real,           // <== supported
    Integer,        // <== supported
    Complex,        // <== supported
    Pattern         // <== supported
}; // enum class Data


enum class Structure
{
    General,        // <== supported
    Symmetric,      // <== supported
    SkewSymmetric,  // <== supported
    Hermitian       // <== supported
}; // enum class Structure


} // namespace format


std::vector<unsigned char> convert(std::istream& r_stream,
                                   std::size_t& r_imageWidth,
                                   std::size_t& r_imageHeight,
                                   const std::string& r_colormapName);


#define MTX2IMG_DEFINE_EXCEPTION(exceptionName)         \
    struct exceptionName : public std::runtime_error {  \
        using std::runtime_error::runtime_error;        \
    }


MTX2IMG_DEFINE_EXCEPTION(ParsingException);


MTX2IMG_DEFINE_EXCEPTION(InvalidFormat);


MTX2IMG_DEFINE_EXCEPTION(UnsupportedFormat);


#undef MTX2IMG_DEFINE_EXCEPTION


} // namespace mtx2img
