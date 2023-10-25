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
    {"-w", "1080"},
    {"-h", "1080"},
    {"-c", "binary"}
};


struct Arguments
{
    std::filesystem::path inputPath;
    std::filesystem::path outputPath;
    std::size_t width;
    std::size_t height;
    std::string colormap;
}; // struct Arguments


void printHelp()
{
    std::cout
        << "Help: convert large sparse matrices from matrix market format to images.\n"
        << "Usage: mtx2img <path-to-source> <path-to-output> [OPTION ARGUMENT] ...\n"
        << "Options:\n"
        << "    -w <width>     : width of the output image in pixels (default: " << defaultArguments.at("-w") << ").\n"
        << "    -h <height>    : height of the output image in pixels (default: " << defaultArguments.at("-h") << ").\n"
        << "    -c <colormap>  : colormap to use for per pixel nonzero density. Options: [binary, kindlmann, viridis] (default: "  << defaultArguments.at("-c") << ").\n"
        ;
}


std::optional<Arguments> parseArguments(int argc, char const* const* argv)
{
    Arguments arguments;
    auto argMap = defaultArguments;

    // Parse optional argMap
    auto it_argument = argMap.end();
    for (int i_arg=3; i_arg<argc; ++i_arg) {
        std::string arg = argv[i_arg];
        if (!arg.empty() && arg.front() == '-') {
            // Parse a key
            if (it_argument == argMap.end()) {
                if ((it_argument = argMap.find(arg)) == argMap.end()) {
                    // The provided key does not exist in the argument map
                    // Special case: "--help"
                    if (arg == "--help") {
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
    } // while (i_arg < argc)

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
    }

    if ((inputStatus.permissions() & std::filesystem::perms::owner_read) == std::filesystem::perms::none) {
        throw std::invalid_argument(std::format(
            "Error: missing read access to input file {}\n",
            arguments.inputPath.string()
        ));
    }

    // Validate output path
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
    }

    if ((outputStatus.permissions() & std::filesystem::perms::owner_write) == std::filesystem::perms::none) {
        throw std::invalid_argument(std::format(
            "Error: missing write access to output file {}\n",
            arguments.outputPath.string()
        ));
    }

    // Validate colormap
    arguments.colormap = argMap["-c"];
    {
        if (!std::set<std::string>({"binary", "kindlmann", "viridis"}).contains(arguments.colormap)) {
            throw std::invalid_argument(std::format(
                "Error: invalid colormap: {}\n",
                arguments.colormap
            ));
        }
    }

    // Convert and validate width
    char* it_end = nullptr;
    std::string& r_widthString = argMap["-w"];
    const long long width = std::strtoll(r_widthString.data(), &it_end, 0);
    if (it_end < r_widthString.data() ||
        static_cast<std::size_t>(std::distance(r_widthString.data(), it_end)) != r_widthString.size()) {
        throw std::invalid_argument(std::format(
            "Error: invalid output image width: {}\n",
            r_widthString
        ));
    } else if (width < 0) {
        throw std::invalid_argument(std::format(
            "Error: negative output image width: {}\n",
            r_widthString
        ));
    } else {
        arguments.width = static_cast<std::size_t>(width);
    }

    // Convert and validate height
    it_end = nullptr;
    std::string& r_heightString = argMap["-h"];
    const long long height = std::strtoll(r_heightString.data(), &it_end, 0);
    if (it_end < r_heightString.data() ||
        static_cast<std::size_t>(std::distance(r_heightString.data(), it_end)) != r_heightString.size()) {
        throw std::invalid_argument(std::format(
            "Error: invalid output image height: {}\n",
            r_heightString
        ));
    } else if (height < 0) {
        throw std::invalid_argument(std::format(
            "Error: negative output image height: {}\n",
            r_heightString
        ));
    } else {
        arguments.height = static_cast<std::size_t>(height);
    }

    return arguments;
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
    } catch (std::invalid_argument& r_exception) {
        std::cerr << r_exception.what();
        printHelp();
        return 1;
    }

    std::ifstream inputFile(arguments.inputPath);
    if (!inputFile.good()) {
        std::cerr << "Error: failed to open input file: " << arguments.inputPath << '\n';
        return 2;
    }

    std::vector<unsigned char> image;
    #ifdef NDEBUG
    try {
    #endif

    // Parse the input file and fill an output image buffer
    // Note: the image gets resized if the matrix dimensions
    //       are smaller than the requested image dimensions.
    image = mtx2img::convert(inputFile,
                             arguments.width,
                             arguments.height,
                             arguments.colormap);

    #ifdef NDEBUG
    } catch (mtx2img::ParsingException& r_exception) {
        std::cerr << r_exception.what();
        return 3;
    } catch (mtx2img::InvalidFormat& r_exception) {
        std::cerr << r_exception.what();
        return 4;
    } catch (mtx2img::UnsupportedFormat& r_exception) {
        std::cerr << r_exception.what();
        return 5;
    } catch (std::invalid_argument& r_exception) {
        std::cerr << r_exception.what();
        return 6;
    }
    #endif

    std::string outputPath = arguments.outputPath.string();
    if (stbi_write_png(
        outputPath.c_str(),                                                     // <== image path
        arguments.width,                                                        // <== image width
        arguments.height,                                                       // <== image height
        image.size() / arguments.width / arguments.height,                      // <== number of color channels
        static_cast<const void*>(image.data()),                                 // <== pointer to image data
        image.size() / arguments.height * sizeof(decltype(image)::value_type)   // <== bytes per row
        ) == 0) {
            std::cerr << "Error: failed to write output image.\n";
            return 1;
    }

    return 0;
}
