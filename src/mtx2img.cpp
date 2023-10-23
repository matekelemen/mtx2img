// --- External Includes ---
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// --- Internal Includes ---
#include "mtx2img/mtx2img.hpp"

// --- STL Includes ---
#include <limits>
#include <fstream>
#include <utility> // pair
#include <optional>
#include <span>
#include <ranges>
#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <cstdlib>
#include <atomic>
#include <numeric>


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


std::size_t getThreadCount()
{
    std::size_t threadCount = std::thread::hardware_concurrency();
    const auto p_ompNumThreads = std::getenv("OMP_NUM_THREADS");
    if (p_ompNumThreads) {
        const std::size_t ompThreadCount = std::strtoul(p_ompNumThreads, NULL, 10);
        if (ompThreadCount < threadCount) {
            threadCount = ompThreadCount;
        }
    }
    return std::max(1ul, threadCount);
}


void fill(std::istream& r_stream,
          std::pair<std::size_t,std::size_t> matrixSize,
          [[maybe_unused]] std::size_t nonzeros,
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

    const std::size_t threadCount = getThreadCount();
    std::atomic<std::size_t> entryCount = 0;
    std::vector<std::vector<unsigned>> images(threadCount,
                                              std::vector<unsigned>(pixelCount, 0u));
    std::vector<std::thread> threads;
    threads.reserve(threadCount);
    std::mutex mutex;

    for (std::size_t i_thread=0ul; i_thread<threadCount; ++i_thread) {
        threads.push_back(std::thread(
            [&entryCount, &r_stream, &images, &mutex, imageSize, matrixSize, i_thread]() -> void {
                auto& r_image = images[i_thread];
                while (true) {
                    std::optional<std::pair<std::size_t,std::size_t>> maybePosition;
                    {
                        std::scoped_lock<std::mutex> lock(mutex);
                        maybePosition = readLine(r_stream);
                    }
                    if (maybePosition.has_value()) {
                        ++entryCount;
                        const std::size_t row = maybePosition->first;
                        const std::size_t column = maybePosition->second;
                        const std::size_t imageRow = row * imageSize.first / matrixSize.second;
                        const std::size_t imageColumn = column * imageSize.second / matrixSize.first;
                        const std::size_t i_flat = std::min(imageRow * imageSize.first + imageColumn,
                                                            r_image.size() - 1);
                        ++r_image[i_flat];
                    } else {
                        break;
                    }
                } // while (true)
            } // [](){}
        )); // threads.push_back
    } // for i_thread in range(threadCount)

    for (std::thread& r_thread : threads) {
        r_thread.join();
    }
    threads.clear();

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

    for (std::size_t i_pixel=0; i_pixel<pixelCount; ++i_pixel) {
        const unsigned nnzs = std::accumulate(images.begin(),
                                              images.end(),
                                              0u,
                                              [i_pixel](unsigned lhs, const auto& r_image){
                                                  return lhs + r_image[i_pixel];
                                              });
        const unsigned char intensity = 255 - std::min(
            nnzs ? std::max(int(nnzs / maxEntriesPerPixel * 0xff), 0x80) : 0,
            0xff);
        std::size_t i_value = channels * i_pixel;
        image[i_value++] = intensity ? 255 : 0;
        image[i_value++] = intensity ? 255 : 0;
        image[i_value]   = intensity ? 255 : 0;
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
