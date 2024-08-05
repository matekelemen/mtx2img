// --- External Includes ---
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// --- Internal Includes ---
#include "mtx2img/mtx2img.hpp"

// --- STL Includes ---
#include <fstream> // ifstream
#include <iostream>  // cout, cerr
#include <map> // map
#include <stdexcept> // invalid_argument
#include <string> // string
#include <filesystem> // filesystem::path, filesystem::perm, filesystem::status, filesystem::file_type
#include <format> // format
#include <optional> // optional
#include <set> // set


/** Default arguments:
 *  - 1080x1080 pixel output image
 *  - binary colormap (any pixel with a nonzero is black, rest are white)
 */
const std::map<std::string,std::string> defaultArguments {
    {"-r", "1080"},
    {"-a", "count"},
    {"-c", "binary"}
};


struct Arguments
{
    std::filesystem::path inputPath;
    std::filesystem::path outputPath;
    std::size_t resolution;
    mtx2img::Aggregation aggregation;
    std::string colormap;
}; // struct Arguments


void printHelp()
{
    std::cout
        << "Help: convert large sparse matrices from MatrixMarket format to images.\n"
        << "Usage: mtx2img <path-to-source> <path-to-output> [OPTION ARGUMENT] ...\n"
        << "Options:\n"
        << "    -r <resolution>  : highest resolution of the output image in pixels (default: " << defaultArguments.at("-r") << ").\n"
        << "    -a <aggregation> : controls how sparse entries are aggregated to pixels. Options: [count, sum]\n"
        << "                       \"count\" ignores values and counts the number of entries referencing each pixel.\n"
        << "                       \"sum\" reads values and sums them up for each pixel.\n"
        << "    -c <colormap>    : colormap to use for aggregated pixel values.\n"
        << "                       Options: [binary, kindlmann, viridis, glasbey256, glasbey64, glasbey8] (default: "  << defaultArguments.at("-c") << ").\n"
        << "\n"
        << "The input path must point to an existing MatrixMarket file (or pass '-' to read the same format from stdin).\n"
        << "The parent directory of the output path must exist, and the output path is assumed to either not exist, or\n"
        << "point to an existing file (in which case it will be overwritten).\n"
        ;
}


std::optional<Arguments> parseArguments(int argc, char const* const* argv)
{
    Arguments arguments;
    auto argMap = defaultArguments;

    // Parse optional argMap
    auto it_argument = argMap.end();
    for (int iArg=3; iArg<argc; ++iArg) {
        std::string arg = argv[iArg];
        if (!arg.empty() && arg.front() == '-') {
            // Parse a key
            if (it_argument == argMap.end()) {
                if ((it_argument = argMap.find(arg)) == argMap.end()) {
                    // The provided key does not exist in the argument map
                    // Special case: "--help" or "-h"
                    if (arg == "--help" || arg == "-h") {
                        return {};
                    }

                    // Otherwise it's an error
                    throw std::invalid_argument(std::format(
                        "Error: unrecognized option: {}\n",
                        arg
                    ));
                }
            } else {
                // No value was provided for the last key
                throw std::invalid_argument(std::format(
                    "Error: missing argument for option {}\n",
                    it_argument->first
                ));
            } // else (it_argument == argMap.end())
        } else {
            // Parse a value
            if (it_argument != argMap.end()) {
                it_argument->second = arg;

                // Reset the arg iterator to indicate that a value was provided
                // for the current key.
                it_argument = argMap.end();
            } else {
                // No key was provided for this value
                throw std::invalid_argument(std::format(
                    "Error: missing option for argument {}\n",
                    arg
                ));
            } // else (it_argument != argMap.end())
        } // else (!arg.empty() && arg.front() == '-')
    } // while (iArg < argc)

    // If the arg iterator was not reset, a value
    // was not provided for the last key.
    if (it_argument != argMap.end()) {
        throw std::invalid_argument(std::format(
            "Error: missing argument for option {}\n",
            it_argument->first
        ));
    }

    // Parse required arguments
    // Note: valid special case is when either of the first two args is --help
    if (argc < 2) {
        throw std::invalid_argument("Error: missing argument for input file.\n");
    } else if (argc < 3) {
        if (std::string(argv[1]) == "--help") return {};
        throw std::invalid_argument("Error: missing argument for output file.\n");
    }
    using PathString = std::filesystem::path::string_type;
    arguments.inputPath = PathString(argv[1]);
    arguments.outputPath = PathString(argv[2]);

    if (arguments.inputPath == "--help" || arguments.outputPath == "--help") {
        return {};
    }

    // Validate input path
    if (arguments.inputPath != "-") {
        const auto inputStatus = std::filesystem::status(arguments.inputPath);
        switch (inputStatus.type()) {
            case std::filesystem::file_type::regular: break; // <== ok
            case std::filesystem::file_type::none: throw std::invalid_argument(std::format(
                "Error: input file does not exist: {}\n",
                arguments.inputPath.string()
            ));
            case std::filesystem::file_type::directory: throw std::invalid_argument(std::format(
                "Error: provided input path is a directory: {}\n",
                arguments.inputPath.string()
            ));
            default: throw std::invalid_argument(std::format(
                "Error: input is not a file: {}\n",
                arguments.inputPath.string()
            ));
        } // switch inputStatus

        if ((inputStatus.permissions() & std::filesystem::perms::owner_read) == std::filesystem::perms::none) {
            throw std::invalid_argument(std::format(
                "Error: missing read access to input file {}\n",
                arguments.inputPath.string()
            ));
        } // if !readPermission
    } // if arguments.inputPath != "-"

    // Validate output path
    if (arguments.outputPath != "-") {
        const auto outputStatus = std::filesystem::status(arguments.outputPath);
        switch (outputStatus.type()) {
            case std::filesystem::file_type::not_found: break; // <== ok
            case std::filesystem::file_type::regular: break; // <== ok, overwrite it
            case std::filesystem::file_type::directory: throw std::invalid_argument(std::format(
                "Error: provided output path is a directory: {}\n",
                arguments.outputPath.string()
            ));
            default: throw std::invalid_argument(std::format(
                "Error: output already exists but is not a file: {}\n",
                arguments.outputPath.string()
            ));
        } // switch outputStatus

        if ((outputStatus.permissions() & std::filesystem::perms::owner_write) == std::filesystem::perms::none) {
            throw std::invalid_argument(std::format(
                "Error: missing write access to output file {}\n",
                arguments.outputPath.string()
            ));
        } // if !writePermission
    } // if arguments.outputPath != "-"

    // Validate aggregation method
    if (argMap["-a"] == "count") {
        arguments.aggregation = mtx2img::Aggregation::Count;
    } else if (argMap["-a"] == "sum") {
        arguments.aggregation = mtx2img::Aggregation::Sum;
    } else {
        throw std::invalid_argument(std::format(
            "Error: invalid aggregation method: {}\n",
            argMap["a"]
        ));
    }

    // Validate colormap
    arguments.colormap = argMap["-c"];
    {
        if (!std::set<std::string>({"binary", "kindlmann", "viridis", "glasbey256", "glasbey64", "glasbey8"}).contains(arguments.colormap)) {
            throw std::invalid_argument(std::format(
                "Error: invalid colormap: {}\n",
                arguments.colormap
            ));
        }
    }

    // Convert and validate resolution
    char* itEnd = nullptr;
    const std::string& r_resolutionString = argMap["-r"];
    const long long resolution = std::strtoll(r_resolutionString.data(), &itEnd, 0);
    if (itEnd < r_resolutionString.data() ||
        static_cast<std::size_t>(std::distance(r_resolutionString.data(), static_cast<const char*>(itEnd))) != r_resolutionString.size()) {
        throw std::invalid_argument(std::format(
            "Error: invalid output image resolution: {}\n",
            r_resolutionString
        ));
    } else if (resolution < 0) {
        throw std::invalid_argument(std::format(
            "Error: negative output image resolution: {}\n",
            r_resolutionString
        ));
    } else {
        arguments.resolution = static_cast<std::size_t>(resolution);
    }

    return arguments;
}


// Write function to pass to stbi_write_png_to_func
void writeImageData(void* pContext,     // <== pointer to an std::ostream
                    void* pData,        // <== pointer to the beginning of an unsigned char array
                    int extent)         // <== number of bytes to write
{
    std::ostream& rStream = *reinterpret_cast<std::ostream*>(pContext);
    rStream.write(
        reinterpret_cast<const char*>(pData),
        static_cast<std::streamsize>(extent)
    );
}


int main(int argc, char const* const* argv)
{
    // Parse arguments
    Arguments arguments;
    try {
        auto parsed = parseArguments(argc, argv);
        if (parsed.has_value()) {
            arguments = std::move(parsed.value());
        } else {
            printHelp();
            return 0;
        }
    } catch (std::invalid_argument& rException) {
        std::cerr << rException.what();
        printHelp();
        return 1;
    }

    // Set up input stream
    std::istream* pInputStream = nullptr;
    std::optional<std::ifstream> maybeInputFile;

    if (arguments.inputPath == "-") {
        // Special case: read from the pipe.
        if (!std::cin.eof()) {
            pInputStream = &std::cin;
        } else {
            std::cerr << "Error: requested to read input from the pipe, but it is closed.\n";
            return 2;
        }
    } else {
        // Otherwise read from a file.
        maybeInputFile.emplace(arguments.inputPath);
        pInputStream = &maybeInputFile.value();
        if (!maybeInputFile.value().good()) {
            std::cerr << "Error: failed to open input file: " << arguments.inputPath << '\n';
            return 3;
        }
    }

    // Set up output stream
    const std::string outputPath = arguments.outputPath.string();
    std::ostream* pOutputStream = nullptr;
    std::optional<std::ofstream> maybeOutputFile;

    if (arguments.outputPath == "-") {
        // Special case: write to stdout.
        pOutputStream = &std::cout;
    } else {
        // Otherwise write to a file.
        maybeOutputFile.emplace(arguments.outputPath);
        pOutputStream = &maybeOutputFile.value();
    }

    std::vector<unsigned char> image;
    std::pair<
        std::size_t,    // <== width
        std::size_t     // <== height
    > imageSize {arguments.resolution, arguments.resolution};

    #ifdef NDEBUG
    try {
    #endif

    // Parse the input file and fill an output image buffer
    // Note: the image gets resized if the matrix dimensions
    //       are smaller than the requested image dimensions.
    image = mtx2img::convert(*pInputStream,
                             imageSize.first,
                             imageSize.second,
                             arguments.aggregation,
                             arguments.colormap);

    #ifdef NDEBUG
    } catch (mtx2img::ParsingException& rException) {
        std::cerr << rException.what();
        return 4;
    } catch (mtx2img::InvalidFormat& rException) {
        std::cerr << rException.what();
        return 5;
    } catch (mtx2img::UnsupportedFormat& rException) {
        std::cerr << rException.what();
        return 6;
    } catch (std::invalid_argument& rException) {
        std::cerr << rException.what();
        return 7;
    }
    #endif

    if (stbi_write_png_to_func(
        writeImageData,                                                         // <== write functor
        reinterpret_cast<void*>(pOutputStream),                                 // <== write context (output stream)
        imageSize.first,                                                        // <== image width
        imageSize.second,                                                       // <== image height
        image.size() / imageSize.first / imageSize.second,                      // <== number of color channels
        static_cast<const void*>(image.data()),                                 // <== pointer to image data
        image.size() / imageSize.second * sizeof(decltype(image)::value_type)   // <== bytes per row
        ) == 0) {
            std::cerr << "Error: failed to write output image.\n";
            return 1;
    }

    return 0;
}
