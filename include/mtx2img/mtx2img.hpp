#pragma once

// --- STL Includes ---
#include <iosfwd> // istream
#include <vector> // vector


namespace mtx2img {


std::vector<unsigned char> convert(std::istream& r_stream,
                                   std::size_t& r_imageWidth,
                                   std::size_t& r_imageHeight,
                                   const std::string& r_colormapName);


} // namespace mtx2img
