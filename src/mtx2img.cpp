// --- Internal Includes ---
#include "mtx2img/mtx2img.hpp"

// --- STL Includes ---
#include <array> // array
#include <stdexcept>
#include <vector> // vector
#include <limits> // numeric_limits
#include <fstream> // ifstream
#include <utility> // r
#include <optional> // optional
#include <span> // span
#include <ranges> // ranges::views::iota
#include <format> // format
#include <cassert> // assert
#include <regex> // regex, regex_match

#ifndef NDEBUG
    #include <iostream> // cout, cerr
#endif


namespace mtx2img {


namespace format {


struct Properties
{
    std::optional<Object> object;
    std::optional<Format> format;
    std::optional<Data> data;
    std::optional<Structure> structure;
    std::optional<std::size_t> rows;
    std::optional<std::size_t> columns;
    std::optional<std::size_t> nonzeros;
}; // struct Properties


} // namespace format


constexpr std::size_t CHANNELS = 3ul;


std::vector<std::array<unsigned char, CHANNELS>> makeColormap(const std::string& r_colormapName) {
    if (r_colormapName == "binary") {
        return {
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},
            {  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{  0,  0,  0},{255,255,255}
        };
    } else if (r_colormapName == "kindlmann") {
        // Extended Kindlmann colormap for 256 values
        // Source: https://www.kennethmoreland.com/color-advice/#extended-kindlmann
        return {
            {  0,  0,  0},{  5,  0,  4},{  9,  0,  9},{ 13,  1, 13},{ 16,  1, 17},{ 19,  1, 21},{ 22,  1, 24},{ 24,  1, 27},
            { 26,  1, 30},{ 27,  2, 34},{ 28,  2, 38},{ 29,  2, 42},{ 29,  2, 46},{ 30,  2, 50},{ 30,  3, 53},{ 30,  3, 57},
            { 30,  3, 61},{ 29,  3, 65},{ 29,  3, 68},{ 28,  3, 72},{ 27,  4, 75},{ 27,  4, 79},{ 26,  4, 82},{ 25,  4, 85},
            { 24,  4, 88},{ 23,  4, 92},{ 22,  5, 95},{ 21,  5, 98},{ 20,  5,101},{ 19,  5,103},{ 18,  5,106},{ 18,  5,109},
            { 17,  5,111},{ 14,  5,115},{  8,  6,119},{  6,  8,120},{  6, 11,120},{  6, 15,119},{  6, 18,118},{  6, 22,116},
            {  5, 25,114},{  5, 28,112},{  5, 31,109},{  5, 33,107},{  5, 36,104},{  5, 38,101},{  5, 40, 99},{  5, 42, 96},
            {  5, 44, 94},{  4, 46, 91},{  4, 48, 89},{  4, 49, 87},{  4, 51, 85},{  4, 52, 83},{  4, 54, 81},{  4, 55, 79},
            {  4, 57, 77},{  4, 58, 76},{  4, 59, 74},{  3, 61, 73},{  3, 62, 71},{  3, 63, 70},{  3, 65, 69},{  3, 66, 67},
            {  3, 67, 66},{  3, 68, 65},{  3, 69, 64},{  3, 71, 63},{  3, 72, 61},{  4, 73, 60},{  4, 74, 58},{  4, 75, 56},
            {  4, 77, 55},{  4, 78, 53},{  4, 79, 51},{  4, 80, 49},{  4, 81, 47},{  4, 82, 45},{  4, 84, 43},{  4, 85, 41},
            {  4, 86, 39},{  4, 87, 36},{  4, 88, 34},{  4, 89, 31},{  4, 91, 29},{  4, 92, 26},{  5, 93, 24},{  5, 94, 21},
            {  5, 95, 18},{  5, 96, 15},{  5, 97, 13},{  5, 98, 10},{  5,100,  8},{  5,101,  6},{  6,102,  5},{  8,103,  5},
            { 10,104,  5},{ 11,105,  5},{ 13,106,  5},{ 15,107,  5},{ 17,108,  5},{ 20,109,  5},{ 22,110,  5},{ 26,111,  5},
            { 29,112,  5},{ 32,112,  5},{ 36,113,  5},{ 40,114,  5},{ 43,115,  6},{ 47,116,  6},{ 51,116,  6},{ 55,117,  6},
            { 59,118,  6},{ 63,118,  6},{ 67,119,  6},{ 71,119,  6},{ 76,120,  6},{ 80,120,  6},{ 84,121,  6},{ 88,121,  6},
            { 92,122,  6},{ 97,122,  6},{101,122,  6},{105,123,  6},{109,123,  6},{113,123,  6},{118,123,  6},{122,123,  6},
            {126,123,  6},{130,123,  6},{135,123,  7},{139,123,  7},{144,123,  7},{149,122,  7},{154,122,  7},{160,121,  8},
            {165,120,  8},{171,119,  8},{177,118,  8},{183,117,  9},{189,115,  9},{196,113,  9},{202,111, 10},{209,108, 10},
            {216,105, 10},{222,102, 11},{229, 98, 11},{236, 94, 11},{243, 90, 12},{244, 91, 27},{245, 92, 37},{245, 94, 46},
            {245, 96, 52},{246, 98, 58},{246, 99, 63},{246,101, 67},{246,103, 71},{246,105, 74},{246,107, 77},{247,109, 79},
            {247,111, 83},{247,112, 87},{247,114, 91},{247,115, 96},{248,117,101},{248,118,106},{248,120,112},{248,121,118},
            {249,122,123},{249,123,129},{249,125,135},{249,126,141},{249,127,147},{249,129,153},{249,130,158},{249,131,164},
            {249,132,169},{249,133,175},{249,134,180},{249,135,186},{249,137,191},{249,138,196},{249,139,201},{250,140,206},
            {250,141,211},{250,142,216},{250,143,221},{250,144,225},{250,145,230},{250,146,235},{250,147,239},{250,148,243},
            {250,149,248},{248,152,250},{245,155,250},{242,159,250},{239,162,251},{236,165,251},{234,168,251},{232,171,251},
            {230,173,251},{229,176,251},{227,178,251},{226,180,251},{225,182,251},{224,184,252},{224,186,252},{223,188,252},
            {223,190,252},{222,191,252},{222,193,252},{222,195,252},{222,196,252},{223,198,252},{223,199,252},{223,201,252},
            {224,202,252},{224,204,253},{225,205,253},{226,207,253},{226,208,253},{227,209,253},{228,211,253},{229,212,253},
            {229,213,253},{230,215,253},{231,216,253},{231,218,253},{231,219,253},{232,221,253},{232,222,253},{232,224,253},
            {232,225,254},{233,227,254},{233,228,254},{233,230,254},{233,231,254},{233,233,254},{233,234,254},{234,236,254},
            {234,237,254},{234,239,254},{235,240,254},{235,242,254},{236,243,254},{237,245,254},{237,246,254},{238,247,254},
            {239,249,254},{240,250,254},{242,251,254},{243,252,254},{245,253,255},{248,254,255},{251,255,255},{255,255,255}
        };
    } else if (r_colormapName == "viridis") {
        // Viridis colormap for 256 values
        // Source: https://www.kennethmoreland.com/color-advice/#viridis
        return {
            { 68,  1, 84},{ 68,  2, 86},{ 69,  4, 87},{ 69,  5, 89},{ 70,  7, 90},{ 70,  8, 92},{ 70, 10, 93},{ 70, 11, 94},
            { 71, 13, 96},{ 71, 14, 97},{ 71, 16, 99},{ 71, 17,100},{ 71, 19,101},{ 72, 20,103},{ 72, 22,104},{ 72, 23,105},
            { 72, 24,106},{ 72, 26,108},{ 72, 27,109},{ 72, 28,110},{ 72, 29,111},{ 72, 31,112},{ 72, 32,113},{ 72, 33,115},
            { 72, 35,116},{ 72, 36,117},{ 72, 37,118},{ 72, 38,119},{ 72, 40,120},{ 72, 41,121},{ 71, 42,122},{ 71, 44,122},
            { 71, 45,123},{ 71, 46,124},{ 71, 47,125},{ 70, 48,126},{ 70, 50,126},{ 70, 51,127},{ 70, 52,128},{ 69, 53,129},
            { 69, 55,129},{ 69, 56,130},{ 68, 57,131},{ 68, 58,131},{ 68, 59,132},{ 67, 61,132},{ 67, 62,133},{ 66, 63,133},
            { 66, 64,134},{ 66, 65,134},{ 65, 66,135},{ 65, 68,135},{ 64, 69,136},{ 64, 70,136},{ 63, 71,136},{ 63, 72,137},
            { 62, 73,137},{ 62, 74,137},{ 62, 76,138},{ 61, 77,138},{ 61, 78,138},{ 60, 79,138},{ 60, 80,139},{ 59, 81,139},
            { 59, 82,139},{ 58, 83,139},{ 58, 84,140},{ 57, 85,140},{ 57, 86,140},{ 56, 88,140},{ 56, 89,140},{ 55, 90,140},
            { 55, 91,141},{ 54, 92,141},{ 54, 93,141},{ 53, 94,141},{ 53, 95,141},{ 52, 96,141},{ 52, 97,141},{ 51, 98,141},
            { 51, 99,141},{ 50,100,142},{ 50,101,142},{ 49,102,142},{ 49,103,142},{ 49,104,142},{ 48,105,142},{ 48,106,142},
            { 47,107,142},{ 47,108,142},{ 46,109,142},{ 46,110,142},{ 46,111,142},{ 45,112,142},{ 45,113,142},{ 44,113,142},
            { 44,114,142},{ 44,115,142},{ 43,116,142},{ 43,117,142},{ 42,118,142},{ 42,119,142},{ 42,120,142},{ 41,121,142},
            { 41,122,142},{ 41,123,142},{ 40,124,142},{ 40,125,142},{ 39,126,142},{ 39,127,142},{ 39,128,142},{ 38,129,142},
            { 38,130,142},{ 38,130,142},{ 37,131,142},{ 37,132,142},{ 37,133,142},{ 36,134,142},{ 36,135,142},{ 35,136,142},
            { 35,137,142},{ 35,138,141},{ 34,139,141},{ 34,140,141},{ 34,141,141},{ 33,142,141},{ 33,143,141},{ 33,144,141},
            { 33,145,140},{ 32,146,140},{ 32,146,140},{ 32,147,140},{ 31,148,140},{ 31,149,139},{ 31,150,139},{ 31,151,139},
            { 31,152,139},{ 31,153,138},{ 31,154,138},{ 30,155,138},{ 30,156,137},{ 30,157,137},{ 31,158,137},{ 31,159,136},
            { 31,160,136},{ 31,161,136},{ 31,161,135},{ 31,162,135},{ 32,163,134},{ 32,164,134},{ 33,165,133},{ 33,166,133},
            { 34,167,133},{ 34,168,132},{ 35,169,131},{ 36,170,131},{ 37,171,130},{ 37,172,130},{ 38,173,129},{ 39,173,129},
            { 40,174,128},{ 41,175,127},{ 42,176,127},{ 44,177,126},{ 45,178,125},{ 46,179,124},{ 47,180,124},{ 49,181,123},
            { 50,182,122},{ 52,182,121},{ 53,183,121},{ 55,184,120},{ 56,185,119},{ 58,186,118},{ 59,187,117},{ 61,188,116},
            { 63,188,115},{ 64,189,114},{ 66,190,113},{ 68,191,112},{ 70,192,111},{ 72,193,110},{ 74,193,109},{ 76,194,108},
            { 78,195,107},{ 80,196,106},{ 82,197,105},{ 84,197,104},{ 86,198,103},{ 88,199,101},{ 90,200,100},{ 92,200, 99},
            { 94,201, 98},{ 96,202, 96},{ 99,203, 95},{101,203, 94},{103,204, 92},{105,205, 91},{108,205, 90},{110,206, 88},
            {112,207, 87},{115,208, 86},{117,208, 84},{119,209, 83},{122,209, 81},{124,210, 80},{127,211, 78},{129,211, 77},
            {132,212, 75},{134,213, 73},{137,213, 72},{139,214, 70},{142,214, 69},{144,215, 67},{147,215, 65},{149,216, 64},
            {152,216, 62},{155,217, 60},{157,217, 59},{160,218, 57},{162,218, 55},{165,219, 54},{168,219, 52},{170,220, 50},
            {173,220, 48},{176,221, 47},{178,221, 45},{181,222, 43},{184,222, 41},{186,222, 40},{189,223, 38},{192,223, 37},
            {194,223, 35},{197,224, 33},{200,224, 32},{202,225, 31},{205,225, 29},{208,225, 28},{210,226, 27},{213,226, 26},
            {216,226, 25},{218,227, 25},{221,227, 24},{223,227, 24},{226,228, 24},{229,228, 25},{231,228, 25},{234,229, 26},
            {236,229, 27},{239,229, 28},{241,229, 29},{244,230, 30},{246,230, 32},{248,230, 33},{251,231, 35},{253,231, 37}
        };
    } else {
        throw std::invalid_argument(std::format(
            "Error: invalid colormap: {}\n",
            r_colormapName
        ));
    }
}


std::optional<std::pair<std::size_t,std::size_t>> readDataLine(std::istream& r_stream)
{
    constexpr std::streamsize ignoreSize = std::numeric_limits<std::streamsize>::max();
    std::size_t row, column;

    r_stream >> row;
    if (r_stream.eof() || r_stream.bad()) {
        r_stream.clear();
        r_stream.ignore(ignoreSize, '\n');
        return {};
    }

    r_stream >> column;
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
        if (row == 0ul) {
            std::cerr << "mtx2img: WARNING: unexpected 0-based row index!\n";
        }
        if (column == 0ul) {
            std::cerr << "mtx2img: WARNING: unexpected 0-based column index!\n";
        }
    #endif

    // Convert (row,column) indices to 0-based indices
    return std::make_pair(
        std::max(row, 1ul) - 1ul,
        std::max(column, 1ul) - 1ul
    );
}


format::Properties parseHeader(std::istream& r_stream,
                               std::span<std::string::value_type> inputBuffer)
{
    format::Properties inputProperties;
    std::regex formatPattern(R"(^%%MatrixMarket (\w+) (\w+) (.*)?)");
    std::regex qualifierPattern(R"(\w+)");
    std::size_t i_line = 0ul;

    #ifndef NDEBUG
        std::cout << "mtx2img: --- HEADER BEGIN ---\n";
    #endif

    while (r_stream.peek() == '%' && !r_stream.bad() && !r_stream.fail()) /*comment line begins with a '%'*/ {
        // Read the input stream until the buffer is filled
        // or a newline character is encountered.
        r_stream.getline(inputBuffer.data(), inputBuffer.size());

        // If the fail bit is set, the buffer got filled before
        // a newline character was found in the stream. This means
        // that the input file is invalid.
        if (r_stream.fail()) {
            throw ParsingException(std::format(
                "Error: header line exceeds the maximum length allowed by the specification ({})\n{}...\n",
                inputBuffer.size(),
                inputBuffer.data()
            ));
        }

        #ifndef NDEBUG
            // Print the header in debug mode.
            std::cout << "mtx2img: " << inputBuffer.data() << "\n";
        #endif

        // The first line must contain format properties.
        std::match_results<const std::string::value_type*> match;
        if (std::regex_match(inputBuffer.data(), match, formatPattern)) {
            if (i_line == 0ul) {
                assert(2 <= match.size());

                // Parse object type (matrix, vector, etc.)
                const std::string objectName = match.str(1);
                if (objectName == "matrix") {
                    inputProperties.object = format::Object::Matrix;
                } else if (objectName == "vector") {
                    inputProperties.object = format::Object::Vector;
                } else {
                    throw InvalidFormat(std::format(
                        "Error: invalid input object type: {}\n",
                        objectName
                    ));
                }

                // Parse format type (coordinate or array)
                const std::string formatName = match.str(2);
                if (formatName == "coordinate") {
                    inputProperties.format = format::Format::Coordinate;
                } else if (formatName == "array") {
                    inputProperties.format = format::Format::Array;
                } else {
                    throw InvalidFormat(std::format(
                        "Error: invalid matrix format: {}\n",
                        formatName
                    ));
                }

                // Loop over optional qualifiers (value type, symmetry)
                if (3 < match.size()) {
                    const std::string qualifiers = match.str(3);
                    for (auto it_qualifier = std::sregex_iterator(qualifiers.begin(),
                                                                qualifiers.end(),
                                                                qualifierPattern);
                            it_qualifier != std::sregex_iterator();
                            ++it_qualifier) {
                        const std::string qualifier = it_qualifier->str();
                        if (qualifier == "real") {
                            inputProperties.data = format::Data::Real;
                        } else if (qualifier == "integer") {
                            inputProperties.data = format::Data::Integer;
                        } else if (qualifier == "complex") {
                            inputProperties.data = format::Data::Complex;
                        } else if (qualifier == "pattern") {
                            inputProperties.data = format::Data::Pattern;
                        } else if (qualifier == "general") {
                            inputProperties.structure = format::Structure::General;
                        } else if (qualifier == "symmetric") {
                            inputProperties.structure = format::Structure::Symmetric;
                        } else if (qualifier == "skew-symmetric") {
                            inputProperties.structure = format::Structure::SkewSymmetric;
                        } else if (qualifier == "hermitian") {
                            inputProperties.structure = format::Structure::Hermitian;
                        } else {
                            throw InvalidFormat(std::format(
                                "Error: invalid qualifier in input header: {}",
                                qualifier
                            ));
                        }
                    }
                }
            } // if i_line == 0

            #ifndef NDEBUG
            else {
                // Warn if matrix properties are redefined.
                std::cerr << "mtx2img: WARNING: input redefines matrix properties (redefined properties are ignored):\n";
            }
            #endif
        } else if (i_line == 0ul) {
            throw ParsingException(std::format(
                "Error: the first line of the input must begin with '%%MatrixMarket' and define the matrix format, but it is\n{}\n",
                inputBuffer.data()
            ));
        }

        ++i_line;
    }

    long long rows = 0ul, columns = 0ul, nonzeros = 0ul;

    // Parse number of rows
    r_stream >> rows;
    if (r_stream.fail()) {
        throw ParsingException("Error: failed to parse the number of rows in the input matrix\n");
    } else if (rows < 0ll) {
        throw ParsingException(std::format(
            "Error: negative number of rows in input: {}\n",
            rows
        ));
    } else {
        inputProperties.rows = static_cast<std::size_t>(rows);
    }

    // Parse number of columns
    r_stream >> columns;
    if (r_stream.fail()) {
        throw ParsingException("Error: failed to parse the number of columns in the input matrix\n");
    } else if (columns < 0ll) {
        throw ParsingException(std::format(
            "Error: negative number of columns in input: {}\n",
            columns
        ));
    } else {
        inputProperties.columns = static_cast<std::size_t>(columns);
    }

    // Parse number of nonzeros
    r_stream >> nonzeros;
    if (r_stream.fail()) {
        throw ParsingException("Error: failed to parse the number of nonzeros in the input matrix\n");
    } else if (nonzeros < 0ll) {
        throw ParsingException(std::format(
            "Error: negative number of nonzeros in input: {}\n",
            nonzeros
        ));
    } else {
        inputProperties.nonzeros = static_cast<std::size_t>(nonzeros);
    }

    // Assign default qualifiers if necessary
    if (!inputProperties.data.has_value()) {
        inputProperties.data = format::Data::Real;
    }

    if (!inputProperties.structure.has_value()) {
        inputProperties.structure = format::Structure::General;
    }

    // Ignore the rest of the line
    r_stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    #ifndef NDEBUG
        std::cout << "mtx2img: --- HEADER END ---\n";

        // Print matrix properties in debug mode
        std::cout << "mtx2img: input matrix properties:\n"
                  << "mtx2img:     " << rows << " rows\n"
                  << "mtx2img:     " << columns << " columns\n"
                  << "mtx2img:     " << nonzeros << " entries\n";
    #endif

    return inputProperties;
}


/// Generate the upper triangle from entries in the lower triangle.
template <class TTransform>
void fillSymmetricPart(std::span<unsigned> nnzMap,
                       std::pair<std::size_t,std::size_t> imageSize,
                       TTransform&& r_transformFunctor)
{
    // Check whether the provided sizes are consistent with the image buffer.
    assert(nnzMap.size() == imageSize.first * imageSize.second);

    // Symmetric matrices must be square, and mtx2img keeps the aspect ratio
    // of the input matrix.
    assert(imageSize.first == imageSize.second);

    for (std::size_t i_row=0; i_row<imageSize.second; ++i_row) {
        for (std::size_t i_column=0; i_column<i_row; ++i_column) {
            const std::size_t i_flat = i_row * imageSize.first + i_column;
            const std::size_t i_symmetric = i_column * imageSize.first + i_row;

            // Debug check: the upper triangle should be empty if the
            // symmetric qualifier was set.
            assert(!nnzMap[i_symmetric]);

            nnzMap[i_symmetric] = r_transformFunctor(nnzMap[i_flat]);
        } // for i_column
    } // for i_row
}


void fill(std::istream& r_stream,
          std::pair<std::size_t,std::size_t> matrixSize,
          std::size_t nonzeros,
          std::span<unsigned char> image,
          std::pair<std::size_t,std::size_t> imageSize,
          const std::string& r_colormapName,
          std::optional<format::Structure> maybeStructure)
{
    // Nothing to do if the input size is null.
    if (matrixSize.first == 0ul || matrixSize.second == 0ul) {
        #ifndef NDEBUG
            std::cout << "mtx2img: nothing to do (degenerate input matrix).\n";
        #endif
        if (nonzeros == 0ul) {
            return;
        } else {
            throw ParsingException(std::format(
                "Error: degenerate input matrix ({}x{}) claims to have {} nonzeros",
                matrixSize.first,
                matrixSize.second,
                nonzeros
            ));
        }
    }

    // Nothing to do if the output size is null
    if (imageSize.first == 0ul || imageSize.second == 0ul) {
        #ifndef NDEBUG
            std::cout << "mtx2img: nothing to do (degenerate output image).\n";
        #endif
        return;
    }

    // Choose colormap
    const auto colormap = makeColormap(r_colormapName);

    // Check image buffer size
    const std::size_t pixelCount = imageSize.first * imageSize.second;
    assert(image.size() == pixelCount * CHANNELS);

    // A buffer for keeping track of how many entries in the matrix
    // map to a given pixel.
    std::vector<unsigned> nnzMap(pixelCount, 0u);

    // Track how many entries were read from the input stream.
    // This will be compared against the expected number of nonzeros.
    std::size_t entryCount = 0ul;

    // Most number of matrix entries mapping to a single pixel.
    decltype(nnzMap)::value_type maxNnzCount = 0u;

    // Parse the input file and map entries to pixels in the image.
    while (true) {
        const auto maybePosition = readDataLine(r_stream);
        if (maybePosition.has_value()) [[likely]] {
            ++entryCount;
            const std::size_t row = maybePosition->first;
            const std::size_t column = maybePosition->second;

            #ifndef NDEBUG
                if (matrixSize.first <= row) {
                    std::cerr << std::format("mtx2img: row index {} is out of bounds {} at entry {}\n",
                                             row, matrixSize.first, entryCount);
                }
                if (matrixSize.second <= column) {
                    std::cerr << std::format("mtx2img: column index {} is out of bounds {} at entry {}\n",
                                             column, matrixSize.second, entryCount);
                }
            #endif

            const std::size_t imageRow = row * imageSize.second / matrixSize.first;
            const std::size_t imageColumn = column * imageSize.first / matrixSize.second;
            const std::size_t i_flat = imageRow * imageSize.first + imageColumn;
            assert(i_flat < nnzMap.size());
            maxNnzCount = std::max(maxNnzCount, ++nnzMap[i_flat]);
        } else {
            break;
        }
    } // while (true)

    // Check the read number of entries
    if (entryCount != nonzeros) {
        throw ParsingException(std::format(
            "Expecting {} entries, but read {}\n",
            nonzeros,
            entryCount
        ));
    }

    #ifndef NDEBUG
        std::cout << std::format("mtx2img: highest nonzero density is {}\n", maxNnzCount);
    #endif

    // No need to pass through the image again if no
    // entries were read.
    if (maxNnzCount == 0) {
        return;
    }

    // If the input was provided in symmetric format, the entries
    // read so far were limited to the main diagonal and the lower
    // triangle, so the upper triangle must be filled in separately.
    // Note: currently, all options are handled in the same manner,
    //       but once value-based intensity is enabled, skewness and
    //       negative values will have to be considered.
    if (maybeStructure.has_value()) {
        switch (maybeStructure.value()) {
            case format::Structure::General: break; // <== nothing to do
            case format::Structure::Symmetric:
                fillSymmetricPart(nnzMap,
                                  imageSize,
                                  [](unsigned v){return v;});
                break;
            case format::Structure::SkewSymmetric:
                fillSymmetricPart(nnzMap,
                                  imageSize,
                                  [](unsigned v){return v;});
                break;
            case format::Structure::Hermitian:
                fillSymmetricPart(nnzMap,
                                  imageSize,
                                  [](unsigned v){return v;});
                break;
            default: throw std::runtime_error("Missing fill strategy implementation for input matrix structure.");
        }
    }

    // Apply the colormap and fill the image buffer
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
                                   const std::string& r_colormapName,
                                   std::span<std::string::value_type> inputBuffer)
{
    std::vector<unsigned char> image;
    const format::Properties inputProperties = parseHeader(r_stream, inputBuffer);

    // Validate object type
    if (inputProperties.object.has_value()) {
        switch (inputProperties.object.value()) {
            case format::Object::Matrix: break; // <== ok
            case format::Object::Vector: throw UnsupportedFormat("Error: vector input is not supported yet\n"); // <== @todo
            default: throw UnsupportedFormat("Error: unsupported input object type\n");
        }
    } else {
        throw ParsingException("Error: missing input object type specifier\n");
    }

    // Validate format type
    if (inputProperties.format.has_value()) {
        switch (inputProperties.format.value()) {
            case format::Format::Coordinate: break; // <== ok
            case format::Format::Array: throw UnsupportedFormat("Error: dense input format is not supported yet\n"); // <== @todo
            default: throw UnsupportedFormat("Error: unsupported input format type\n");
        }
    } else {
        throw ParsingException("Error: missing input format type specifier\n");
    }

    // Validate data type (optional qualifier - no error if missing)
    if (inputProperties.data.has_value()) {
        switch (inputProperties.data.value()) {
            case format::Data::Real: break;     // <== ok
            case format::Data::Integer: break;  // <== ok
            case format::Data::Complex: break;  // <== ok
            case format::Data::Pattern: break;  // <== ok
            default: throw UnsupportedFormat("Error: unsupported input value type.\n");
        }
    }

    // Validate object structure (optional qualifier - no error if missing)
    if (inputProperties.structure.has_value()) {
        switch (inputProperties.structure.value()) {
            case format::Structure::General: break;         // <== ok
            case format::Structure::Symmetric: break;       // <== ok
            case format::Structure::SkewSymmetric: break;   // <== ok
            case format::Structure::Hermitian: break;       // <== ok
            default: throw UnsupportedFormat("Error: unsupported input object format.\n");
        }
    }

    #ifndef NDEBUG
        // Print changes to the output dimension in debug mode
        const bool resize = inputProperties.columns.value() < static_cast<std::size_t>(r_imageWidth)
                         || inputProperties.rows.value() < static_cast<std::size_t>(r_imageHeight);
        const std::pair<std::size_t,std::size_t> requestedImageSize {r_imageWidth, r_imageHeight};
        if (resize) {
            std::cout << "mtx2img: restricting image size ";
        }
    #endif

    // Restrict output image size
    if (inputProperties.columns.value() < r_imageWidth) {
        r_imageWidth = inputProperties.columns.value();
        r_imageHeight = inputProperties.rows.value() * inputProperties.columns.value() / r_imageWidth;
    }

    if (inputProperties.rows.value() < r_imageHeight) {
        r_imageHeight = inputProperties.rows.value();
        r_imageWidth = inputProperties.columns.value() * inputProperties.rows.value() / r_imageHeight;
    }

    #ifndef NDEBUG
        // Print changes to the output dimension in debug mode
        if (resize) {
            std::cout << std::format("from {}x{} to {}x{}\n",
                requestedImageSize.first,
                requestedImageSize.second,
                r_imageWidth,
                r_imageHeight
            );
        }
    #endif

    const std::pair<std::size_t,std::size_t> imageSize {r_imageWidth, r_imageHeight};

    // Resize image buffer to final size and initialize it to full white
    image.resize(imageSize.first * imageSize.second * CHANNELS, 0xff);

    // Parse input stream and fill output image buffer
    fill(
        r_stream,                           // <== input stream
        {
            inputProperties.rows.value(),   // <== number of rows in input matrix
            inputProperties.columns.value() // <== number of columns in input matrix
        },
        inputProperties.nonzeros.value(),   // <== number of nonzero entries in matrix
        image,                              // <== buffer
        imageSize,                          // <== buffer dimensions
        r_colormapName,                     // <== name of the colormap to use
        inputProperties.structure           // <== input matrix symmetry
    );

    return image;
}


std::vector<unsigned char> convert(std::istream& r_stream,
                                   std::size_t& r_imageWidth,
                                   std::size_t& r_imageHeight,
                                   const std::string& r_colormapName)
{
    // The matrix market format is limited to 1024 characters per line
    std::string inputBuffer(0x400, '\0');

    // Make sure that the input buffer ends with a \0 that doesn't show in its size.
    [[maybe_unused]] const char* dummy = inputBuffer.c_str();

    // Perform the conversion using the provided input buffer
    return convert(
        r_stream,
        r_imageWidth,
        r_imageHeight,
        r_colormapName,
        inputBuffer
    );
}


} // namespace mtx2img
