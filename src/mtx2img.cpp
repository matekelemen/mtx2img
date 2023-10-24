// --- Internal Includes ---
#include "mtx2img/mtx2img.hpp"

// --- STL Includes ---
#include <array> // array
#include <vector> // vector
#include <limits> // numeric_limits
#include <fstream> // ifstream
#include <utility> // pair
#include <optional> // optional
#include <span> // span
#include <ranges> // ranges::views::iota
#include <format> // format
#include <cassert> // assert

#ifndef NDEBUG
    #include <iostream>
#endif


namespace mtx2img {


constexpr std::size_t CHANNELS = 3ul;


std::vector<std::array<unsigned char, CHANNELS>> makeColormap(const std::string& r_colormapName) {
    if (r_colormapName == "kindlmann") {
        // Extended Kindlmann colormap for 256 values
        // Source: https://www.kennethmoreland.com/color-advice
        return {
            {0,0,0},
            {5,0,4},
            {9,0,9},
            {13,1,13},
            {16,1,17},
            {19,1,21},
            {22,1,24},
            {24,1,27},
            {26,1,30},
            {27,2,34},
            {28,2,38},
            {29,2,42},
            {29,2,46},
            {30,2,50},
            {30,3,53},
            {30,3,57},
            {30,3,61},
            {29,3,65},
            {29,3,68},
            {28,3,72},
            {27,4,75},
            {27,4,79},
            {26,4,82},
            {25,4,85},
            {24,4,88},
            {23,4,92},
            {22,5,95},
            {21,5,98},
            {20,5,101},
            {19,5,103},
            {18,5,106},
            {18,5,109},
            {17,5,111},
            {14,5,115},
            {8,6,119},
            {6,8,120},
            {6,11,120},
            {6,15,119},
            {6,18,118},
            {6,22,116},
            {5,25,114},
            {5,28,112},
            {5,31,109},
            {5,33,107},
            {5,36,104},
            {5,38,101},
            {5,40,99},
            {5,42,96},
            {5,44,94},
            {4,46,91},
            {4,48,89},
            {4,49,87},
            {4,51,85},
            {4,52,83},
            {4,54,81},
            {4,55,79},
            {4,57,77},
            {4,58,76},
            {4,59,74},
            {3,61,73},
            {3,62,71},
            {3,63,70},
            {3,65,69},
            {3,66,67},
            {3,67,66},
            {3,68,65},
            {3,69,64},
            {3,71,63},
            {3,72,61},
            {4,73,60},
            {4,74,58},
            {4,75,56},
            {4,77,55},
            {4,78,53},
            {4,79,51},
            {4,80,49},
            {4,81,47},
            {4,82,45},
            {4,84,43},
            {4,85,41},
            {4,86,39},
            {4,87,36},
            {4,88,34},
            {4,89,31},
            {4,91,29},
            {4,92,26},
            {5,93,24},
            {5,94,21},
            {5,95,18},
            {5,96,15},
            {5,97,13},
            {5,98,10},
            {5,100,8},
            {5,101,6},
            {6,102,5},
            {8,103,5},
            {10,104,5},
            {11,105,5},
            {13,106,5},
            {15,107,5},
            {17,108,5},
            {20,109,5},
            {22,110,5},
            {26,111,5},
            {29,112,5},
            {32,112,5},
            {36,113,5},
            {40,114,5},
            {43,115,6},
            {47,116,6},
            {51,116,6},
            {55,117,6},
            {59,118,6},
            {63,118,6},
            {67,119,6},
            {71,119,6},
            {76,120,6},
            {80,120,6},
            {84,121,6},
            {88,121,6},
            {92,122,6},
            {97,122,6},
            {101,122,6},
            {105,123,6},
            {109,123,6},
            {113,123,6},
            {118,123,6},
            {122,123,6},
            {126,123,6},
            {130,123,6},
            {135,123,7},
            {139,123,7},
            {144,123,7},
            {149,122,7},
            {154,122,7},
            {160,121,8},
            {165,120,8},
            {171,119,8},
            {177,118,8},
            {183,117,9},
            {189,115,9},
            {196,113,9},
            {202,111,10},
            {209,108,10},
            {216,105,10},
            {222,102,11},
            {229,98,11},
            {236,94,11},
            {243,90,12},
            {244,91,27},
            {245,92,37},
            {245,94,46},
            {245,96,52},
            {246,98,58},
            {246,99,63},
            {246,101,67},
            {246,103,71},
            {246,105,74},
            {246,107,77},
            {247,109,79},
            {247,111,83},
            {247,112,87},
            {247,114,91},
            {247,115,96},
            {248,117,101},
            {248,118,106},
            {248,120,112},
            {248,121,118},
            {249,122,123},
            {249,123,129},
            {249,125,135},
            {249,126,141},
            {249,127,147},
            {249,129,153},
            {249,130,158},
            {249,131,164},
            {249,132,169},
            {249,133,175},
            {249,134,180},
            {249,135,186},
            {249,137,191},
            {249,138,196},
            {249,139,201},
            {250,140,206},
            {250,141,211},
            {250,142,216},
            {250,143,221},
            {250,144,225},
            {250,145,230},
            {250,146,235},
            {250,147,239},
            {250,148,243},
            {250,149,248},
            {248,152,250},
            {245,155,250},
            {242,159,250},
            {239,162,251},
            {236,165,251},
            {234,168,251},
            {232,171,251},
            {230,173,251},
            {229,176,251},
            {227,178,251},
            {226,180,251},
            {225,182,251},
            {224,184,252},
            {224,186,252},
            {223,188,252},
            {223,190,252},
            {222,191,252},
            {222,193,252},
            {222,195,252},
            {222,196,252},
            {223,198,252},
            {223,199,252},
            {223,201,252},
            {224,202,252},
            {224,204,253},
            {225,205,253},
            {226,207,253},
            {226,208,253},
            {227,209,253},
            {228,211,253},
            {229,212,253},
            {229,213,253},
            {230,215,253},
            {231,216,253},
            {231,218,253},
            {231,219,253},
            {232,221,253},
            {232,222,253},
            {232,224,253},
            {232,225,254},
            {233,227,254},
            {233,228,254},
            {233,230,254},
            {233,231,254},
            {233,233,254},
            {233,234,254},
            {234,236,254},
            {234,237,254},
            {234,239,254},
            {235,240,254},
            {235,242,254},
            {236,243,254},
            {237,245,254},
            {237,246,254},
            {238,247,254},
            {239,249,254},
            {240,250,254},
            {242,251,254},
            {243,252,254},
            {245,253,255},
            {248,254,255},
            {251,255,255},
            {255,255,255}
        };
    } else if (r_colormapName == "binary") {
        return {
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            {255,255,255}
        };
    } else {
        throw std::runtime_error(std::format(
            "Error: invalid colormap: {}\n",
            r_colormapName
        ));
    }
}


std::optional<std::pair<std::size_t,std::size_t>> readLine(std::istream& r_stream)
{
    constexpr std::streamsize ignoreSize = std::numeric_limits<std::streamsize>::max();
    std::pair<std::size_t,std::size_t> position;

    r_stream >> position.first;
    if (r_stream.eof() || r_stream.bad()) {
        r_stream.clear();
        r_stream.ignore(ignoreSize, '\n');
        return {};
    }

    r_stream.get();

    r_stream >> position.second;
    if (r_stream.eof() || r_stream.bad()) {
        r_stream.clear();
        r_stream.ignore(ignoreSize, '\n');
        return {};
    }

    r_stream.ignore(ignoreSize, '\n');

    #ifndef NDEBUG
        // Check indices in debug mode
        // => matrix market indices begin with 1,
        //    so it's usually safe to subtract 1 from them.
        if (position.first == 0ul) {
            std::cerr << "mtx2img: WARNING: unexpected 0-based row index!\n";
        }
        if (position.first == 0ul) {
            std::cerr << "mtx2img: WARNING: unexpected 0-based column index!\n";
        }
    #endif

    // Convert (row,column) indices to 0-based indices
    return std::make_pair(
        std::max(position.first, 1ul) - 1ul,
        std::max(position.second, 1ul) - 1ul
    );
}


void fill(std::istream& r_stream,
          std::pair<std::size_t,std::size_t> matrixSize,
          std::size_t nonzeros,
          std::span<unsigned char> image,
          std::pair<std::size_t,std::size_t> imageSize,
          const std::string& r_colormapName)
{
    if (matrixSize.first == 0
        || matrixSize.second == 0
        || imageSize.first == 0
        || imageSize.second == 0) {
        return;
    }

    // Choose colormap
    const auto colormap = makeColormap(r_colormapName);

    const std::size_t pixelCount = imageSize.first * imageSize.second;
    assert(image.size() == pixelCount * CHANNELS);

    std::vector<unsigned> nnzMap(pixelCount, 0u);
    std::size_t entryCount = 0ul;
    decltype(nnzMap)::value_type maxNnzCount = 0u;
    while (true) {
        const auto maybePosition = readLine(r_stream);
        if (maybePosition.has_value()) [[likely]] {
            ++entryCount;
            const std::size_t row = maybePosition->first;
            const std::size_t column = maybePosition->second;
            const std::size_t imageRow = row * imageSize.second / matrixSize.first;
            const std::size_t imageColumn = column * imageSize.first / matrixSize.second;
            const std::size_t i_flat = imageRow * imageSize.first + imageColumn;

            #ifndef NDEBUG
                // Print index violations in debug mode
                if (nnzMap.size() <= i_flat) {
                    std::cerr << "mtx2img: WARNING: index violation at "
                              << "(" << row << "," << column << ") => "
                              << "(" << imageRow << "," << imageColumn << ") => "
                              << i_flat << " (image size: " << nnzMap.size() << ")\n";
                    continue;
                }
            #endif

            maxNnzCount = std::max(maxNnzCount, ++nnzMap[i_flat]);
        } else {
            break;
        }
    } // while (true)

    // Check the read number of entries
    if (entryCount != nonzeros) {
        throw std::runtime_error(std::format(
            "Expecting {} entries, but read {}\n",
            nonzeros,
            entryCount
        ));
    }

    if (maxNnzCount == 0) {
        return;
    }

    for (std::size_t i_pixel=0ul; i_pixel<pixelCount; ++i_pixel) {
        const std::size_t intensity = std::min<std::size_t>(0xff, 0xff - 0xff * nnzMap[i_pixel] / maxNnzCount);
        const auto& r_color = colormap[intensity];
        const std::size_t i_imageBegin = CHANNELS * i_pixel;
        for (std::size_t i_component=0; i_component<CHANNELS; ++i_component) {
            image[i_imageBegin + i_component] = r_color[i_component];
        }
    }
}


std::vector<unsigned char> convert(std::istream& r_stream,
                                   std::size_t& r_imageWidth,
                                   std::size_t& r_imageHeight,
                                   const std::string& r_colormapName)
{
    std::vector<unsigned char> image;

    // Ignore comment lines
    while (r_stream.peek() == '%' && !r_stream.bad() && !r_stream.fail()) {
        #ifdef NDEBUG
            r_stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        #else
            // Print the header in debug mode.
            std::cout << "mtx2img: ";
            std::string buffer(512, '\0');
            do {
                r_stream.clear();
                r_stream.getline(buffer.data(), buffer.size());
                std::cout << buffer.substr(0, buffer.find('\0'));
            } while (r_stream.fail());
            std::cout << '\n';
        #endif
    }

    std::size_t rows, columns, nonzeros;
    r_stream >> rows;
    r_stream.get();
    r_stream >> columns;
    r_stream.get();
    r_stream >> nonzeros;
    r_stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    #ifndef NDEBUG
        // Print matrix properties in debug mode
        std::cout << "mtx2img: input matrix properties:\n"
                  << "mtx2img:     " << rows << " rows\n"
                  << "mtx2img:     " << columns << " columns\n"
                  << "mtx2img:     " << nonzeros << " entries\n";

        // Print changes to the output dimension in debug mode
        if (columns < r_imageWidth) {
            std::cout << "mtx2img: restricting image width from " << r_imageWidth << " to " << columns << '\n';
        }
        if (rows < r_imageHeight) {
            std::cout << "mtx2img: restricting image height from " << r_imageHeight << " to " << rows << '\n';
        }
    #endif

    // Restrict output image size
    r_imageWidth = std::min(r_imageWidth, columns);
    r_imageHeight = std::min(r_imageHeight, rows);

    // Resize image to final size and initialize it to full white
    image.resize(r_imageWidth * r_imageHeight * CHANNELS, 0xff);

    // Parse input stream and fill output image buffer
    fill(
        r_stream,                       // <== input stream
        {rows, columns},                // <== matrix dimensions
        nonzeros,                       // <== number of nonzero entries in matrix
        image,                          // <== buffer
        {r_imageWidth, r_imageHeight},  // <== buffer dimensions
        r_colormapName                  // <== name of the colormap to use
    );

    return image;
}


} // namespace mtx2img
