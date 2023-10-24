// --- External Includes ---
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// --- Internal Includes ---
#include "mtx2img/mtx2img.hpp"

// --- STL Includes ---
#include <limits> // numeric_limits
#include <fstream> // ifstream
#include <utility> // pair
#include <optional> // optional
#include <span> // span
#include <ranges> // ranges::views::iota
#include <vector> // vector
#include <sstream> // stringstream


namespace mtx2img {


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
    return position;
}


void fill(std::istream& r_stream,
          std::pair<std::size_t,std::size_t> matrixSize,
          std::size_t nonzeros,
          std::span<unsigned char> image,
          std::pair<std::size_t,std::size_t> imageSize,
          std::size_t channels)
{
    if (matrixSize.first == 0
        || matrixSize.second == 0
        || imageSize.first == 0
        || imageSize.second == 0) {
        return;
    }

    const std::size_t pixelCount = imageSize.first * imageSize.second;
    assert(image.size() == pixelCount * channels);

    const std::size_t threadCount = 1ul;//getThreadCount();
    std::vector<unsigned> nnzMap(pixelCount, 0u);

    std::size_t entryCount = 0ul;
    while (true) {
        const auto maybePosition = readLine(r_stream);
        if (maybePosition.has_value()) [[likely]] {
            ++entryCount;
            const std::size_t row = maybePosition->first;
            const std::size_t column = maybePosition->second;
            const std::size_t imageRow = row * imageSize.first / matrixSize.second;
            const std::size_t imageColumn = column * imageSize.second / matrixSize.first;
            const std::size_t i_flat = std::min(imageRow * imageSize.first + imageColumn,
                                                nnzMap.size() - 1);
            ++nnzMap[i_flat];
        } else {
            break;
        }
    } // while (true)

    // Check the read number of entries
    if (entryCount != nonzeros) {
        std::stringstream message;
        message << "Expecting " << nonzeros << " entries, but read " << entryCount << '\n';
        throw std::runtime_error(message.str());
    }

    const std::size_t maxEntriesPerPixel = std::max(
        1ul,
        imageSize.first * imageSize.second / matrixSize.second / matrixSize.first
    );

    std::size_t i_value = 0;
    for (auto nnzsInPixel : nnzMap) {
        const unsigned char intensity = 255 - std::min(
            nnzsInPixel ? std::max(int(nnzsInPixel * 0xff / maxEntriesPerPixel), 0x80) : 0,
            0xff
        );
        image[i_value]   = intensity;
        image[i_value++] = intensity;
        image[i_value++] = intensity;
    }
}


void convert(std::istream& r_stream,
             const std::filesystem::path& r_outputFilePath,
             std::size_t imageWidth,
             std::size_t imageHeight)
{
    // Ignore comment lines
    while (r_stream.peek() == '%' && !r_stream.bad() && !r_stream.fail()) {
        r_stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::size_t rows, columns, nonzeros;
    r_stream >> rows;
    r_stream.get();
    r_stream >> columns;
    r_stream.get();
    r_stream >> nonzeros;
    r_stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    imageWidth = std::min(imageWidth, columns);
    imageHeight = std::min(imageHeight, rows);
    constexpr std::size_t channels = 3ul;

    std::vector<unsigned char> image(imageWidth * imageHeight * channels, 0xff);
    fill(r_stream,
         {rows, columns},
         nonzeros,
         image,
         {imageWidth, imageHeight},
         channels);

    const std::string outputFilePath = r_outputFilePath.string();
    stbi_write_png(outputFilePath.c_str(),
                   imageWidth,
                   imageHeight,
                   channels,
                   static_cast<const void*>(image.data()),
                   imageWidth * channels * sizeof(decltype(image)::value_type));
}


} // namespace mtx2img
