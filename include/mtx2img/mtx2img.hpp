#pragma once

// --- STL Includes ---
#include <iosfwd>
#include <filesystem>


namespace mtx2img {


void convert(std::istream& r_stream,
             const std::filesystem::path& r_outputFilePath,
             std::size_t imageWidth,
             std::size_t imageHeight);


} // namespace mtx2img
