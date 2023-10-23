// --- Internal Includes ---
#include "mtx2img/mtx2img.hpp"

// --- STL Includes ---
#include <fstream>
#include <iostream>


int main(int argc, char const* const* argv)
{
    if (2 < argc) {
        const std::filesystem::path inputFilePath {std::string(argv[1])};
        const std::filesystem::path outputFilePath {std::string(argv[2])};
        std::ifstream inputFile(inputFilePath);
        mtx2img::convert(inputFile, outputFilePath, 1080, 1080);
    }
    return 0;
}
