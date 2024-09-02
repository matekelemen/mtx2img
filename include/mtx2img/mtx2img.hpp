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
    Array           // <== supported (dense representation)
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


enum class Aggregation
{
    Count,      // <== count the number of nonzeros referencing each pixel
    Sum,        // <== sum the values of each entry referencing a pixel
    Max         // <== largest absolute value of entries referencing the same pixel
}; // enum class Aggregation


std::vector<unsigned char> convert(std::istream& rStream,
                                   std::size_t& rImageWidth,
                                   std::size_t& rImageHeight,
                                   const Aggregation aggregation,
                                   const std::string& rColormapName);


#define MTX2IMG_DEFINE_EXCEPTION(exceptionName)         \
    struct exceptionName : public std::runtime_error {  \
        using std::runtime_error::runtime_error;        \
    }


MTX2IMG_DEFINE_EXCEPTION(ParsingException);


MTX2IMG_DEFINE_EXCEPTION(InvalidFormat);


MTX2IMG_DEFINE_EXCEPTION(UnsupportedFormat);


#undef MTX2IMG_DEFINE_EXCEPTION


} // namespace mtx2img
