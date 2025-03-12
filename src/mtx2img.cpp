// --- Internal Includes ---
#include "mtx2img/mtx2img.hpp"

// --- STL Includes ---
#include <array> // array
#include <stdexcept>
#include <type_traits>
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
#include <variant> // monostate
#include <complex> // complex

#ifndef NDEBUG
    #include <iostream> // cout, cerr
#endif


namespace mtx2img {


template <class T>
struct is_complex : std::false_type {};


template <class T>
struct is_complex<std::complex<T>> : std::true_type {};


template <class T>
constexpr bool is_complex_v = is_complex<T>::value;


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


std::vector<std::array<unsigned char, CHANNELS>> makeColormap(const std::string& rColormapName) {
    if (rColormapName == "binary") {
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
    } else if (rColormapName == "kindlmann") {
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
    } else if (rColormapName == "viridis") {
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
    } else if (rColormapName == "glasbey256") {
        // Categorical color map for 256 values.
        // doi:10.1002/col.20327
        return {
            {130,89 ,146},{24 ,105,255},{0  ,138,0  },{243,109,255},{113,0  ,121},{170,251,0  },{0  ,190,194},{255,162,53 },
            {93 ,61 ,4  },{8  ,0  ,138},{0  ,93 ,93 },{154,125,130},{162,174,255},{150,182,117},{158,40 ,255},{77 ,0  ,20 },
            {255,174,190},{206,0  ,146},{0  ,255,182},{0  ,45 ,0  },{158,117,0  },{61 ,53 ,65 },{243,235,146},{101,97 ,138},
            {138,61 ,77 },{89 ,4  ,186},{85 ,138,113},{178,190,194},{255,93 ,130},{28 ,198,0  },{146,247,255},{45 ,134,166},
            {57 ,93 ,40 },{235,206,255},{255,93 ,0  },{166,97 ,170},{134,0  ,0  },{53 ,0  ,89 },{0  ,81 ,142},{158,73 ,16 },
            {206,190,0  },{0  ,40 ,40 },{0  ,178,255},{202,166,134},{190,154,194},{45 ,32 ,12 },{117,101,69 },{130,121,223},
            {0  ,194,138},{186,231,194},{134,142,166},{202,113,89 },{130,154,0  },{45 ,0  ,255},{210,4  ,247},{255,215,190},
            {146,206,247},{186,93 ,125},{255,65 ,194},{190,134,255},{146,142,101},{166,4  ,170},{134,227,117},{73 ,0  ,61 },
            {251,239,12 },{105,85 ,93 },{89 ,49 ,45 },{105,53 ,255},{182,4  ,77 },{93 ,109,113},{65 ,69 ,53 },{101,113,0  },
            {121,0  ,73 },{28 ,49 ,81 },{121,65 ,158},{255,146,113},{255,166,243},{186,158,65 },{130,170,154},{215,121,0  },
            {73 ,61 ,113},{81 ,162,85 },{231,130,182},{210,227,251},{0  ,73 ,49 },{109,219,194},{61 ,77 ,93 },{97 ,53 ,85 },
            {0  ,113,81 },{93 ,24 ,0  },{154,93 ,81 },{85 ,142,219},{202,202,154},{53 ,24 ,32 },{57 ,61 ,0  },{0  ,154,150},
            {235,16 ,109},{138,69 ,121},{117,170,194},{202,146,154},{210,186,198},{154,206,0  },{69 ,109,170},{117,89 ,0  },
            {206,77 ,12 },{0  ,223,251},{255,61 ,65 },{255,202,73 },{45 ,49 ,146},{134,105,134},{158,130,190},{206,174,255},
            {121,69 ,45 },{198,251,130},{93 ,117,73 },{182,69 ,73 },{255,223,239},{162,0  ,113},{77 ,77 ,166},{166,170,202},
            {113,28 ,40 },{40 ,121,121},{8  ,73 ,0  },{0  ,105,134},{166,117,73 },{251,182,130},{85 ,24 ,125},{0  ,255,89 },
            {0  ,65 ,77 },{109,142,146},{170,36 ,0  },{190,210,109},{138,97 ,186},{210,65 ,190},{73 ,97 ,81 },{206,243,239},
            {97 ,194,97 },{20 ,138,77 },{0  ,255,231},{0  ,105,0  },{178,121,158},{170,178,158},{186,85 ,255},{198,121,206},
            {32 ,49 ,32 },{125,4  ,219},{194,198,247},{138,198,206},{231,235,206},{40 ,28 ,57 },{158,255,174},{130,206,154},
            {49 ,166,12 },{0  ,162,117},{219,146,85 },{61 ,20 ,4  },{255,138,154},{130,134,53 },{105,77 ,113},{182,97 ,0  },
            {125,45 ,0  },{162,178,57 },{49 ,4  ,125},{166,61 ,202},{154,32 ,45 },{4  ,223,134},{117,125,109},{138,150,210},
            {8  ,162,202},{247,109,93 },{16 ,85 ,202},{219,182,101},{146,89 ,109},{162,255,227},{89 ,85 ,40 },{113,121,170},
            {215,89 ,101},{73 ,32 ,81 },{223,77 ,146},{0  ,0  ,202},{93 ,101,210},{223,166,0  },{178,73 ,146},{182,138,117},
            {97 ,77 ,61 },{166,150,162},{85 ,28 ,53 },{49 ,65 ,65 },{117,117,134},{146,158,162},{117,154,113},{255,130,32 },
            {134,85 ,255},{154,198,182},{223,150,243},{202,223,49 },{142,93 ,40 },{53 ,190,227},{113,166,255},{89 ,138,49 },
            {255,194,235},{170,61 ,105},{73 ,97 ,125},{73 ,53 ,28 },{69 ,178,158},{28 ,36 ,49 },{247,49 ,239},{117,0  ,166},
            {231,182,170},{130,105,101},{227,162,202},{32 ,36 ,0  },{121,182,16 },{158,142,255},{210,117,138},{202,182,219},
            {174,154,223},{255,113,219},{210,247,178},{198,215,206},{255,210,138},{93 ,223,53 },{93 ,121,146},{162,142,0  },
            {174,223,239},{113,77 ,194},{125,69 ,0  },{101,146,182},{93 ,121,255},{81 ,73 ,89 },{150,158,81 },{206,105,174},
            {101,53 ,117},{219,210,227},{182,174,117},{81 ,89 ,0  },{182,89 ,57 },{85 ,4  ,235},{61 ,117,45 },{146,130,154},
            {130,36 ,105},{186,134,57 },{138,178,227},{109,178,130},{150,65 ,53 },{109,65 ,73 },{138,117,61 },{178,113,117},
            {146,28 ,73 },{223,109,49 },{0  ,227,223},{146,4  ,202},{49 ,40 ,89 },{0  ,125,210},{162,109,255},{255,255,255}
        };
    } else if (rColormapName == "glasbey64") {
        // Categorical color map for 64 values.
        // doi:10.1002/col.20327
        return {
            {73 ,0  ,61 },{24 ,105,255},{0  ,138,0  },{243,109,255},{113,0  ,121},{170,251,0  },{0  ,190,194},{255,162,53 },
            {93 ,61 ,4  },{8  ,0  ,138},{0  ,93 ,93 },{154,125,130},{162,174,255},{150,182,117},{158,40 ,255},{77 ,0  ,20 },
            {255,174,190},{206,0  ,146},{0  ,255,182},{0  ,45 ,0  },{158,117,0  },{61 ,53 ,65 },{243,235,146},{101,97 ,138},
            {138,61 ,77 },{89 ,4  ,186},{85 ,138,113},{178,190,194},{255,93 ,130},{28 ,198,0  },{146,247,255},{45 ,134,166},
            {57 ,93 ,40 },{235,206,255},{255,93 ,0  },{166,97 ,170},{134,0  ,0  },{53 ,0  ,89 },{0  ,81 ,142},{158,73 ,16 },
            {206,190,0  },{0  ,40 ,40 },{0  ,178,255},{202,166,134},{190,154,194},{45 ,32 ,12 },{117,101,69 },{130,121,223},
            {0  ,194,138},{186,231,194},{134,142,166},{202,113,89 },{130,154,0  },{45 ,0  ,255},{210,4  ,247},{255,215,190},
            {146,206,247},{186,93 ,125},{255,65 ,194},{190,134,255},{146,142,101},{166,4  ,170},{134,227,117},{255,255,255}
        };
    } else if (rColormapName == "glasbey8") {
        // Categorical color map for 8 values.
        // doi:10.1002/col.20327
        return {
            {255,162,53 },{24 ,105,255},{0  ,138,0  },{243,109,255},{113,0  ,121},{170,251,0  },{0  ,190,194},{255,255,255}
        };
    } else {
        throw std::invalid_argument(std::format(
            "Error: invalid colormap: {}\n",
            rColormapName
        ));
    }
}


class Parser
{
public:
    Parser(std::istream& rStream)
        : _pStream(&rStream),
          _inputBuffer(0x400, '\0'),
          _properties()
    {
        // Make sure that the input buffer ends with a \0 that doesn't appear in its size.
        [[maybe_unused]] const char* dummy = _inputBuffer.c_str();
        this->parseHeader();
    }

    template <class TValue>
    std::optional<std::conditional_t<
        std::is_same_v<TValue,std::monostate>,
        std::tuple<std::size_t,std::size_t>,        // <== no values requested, only row and column indices
        std::tuple<std::size_t,std::size_t,TValue>  // <== values requested
    >>
    parseLine()
    {
        if (_properties.format.value() == format::Format::Coordinate) {
            return this->parseSparseDataLine<TValue>();
        } else if (_properties.format.value() == format::Format::Array) {
            return this->parseDenseDataLine<TValue>();
        } else {
            throw InvalidFormat(std::format(
                "Error: unhandled format {}",
                (int)_properties.format.value()
            ));
        }
    }

    format::Properties getProperties() const
    {
        return _properties;
    }

private:
    void parseHeader()
    {
        std::regex formatPattern(R"(^%%MatrixMarket (\w+) (\w+) (.*)?)");
        std::regex qualifierPattern(R"(\w+)");
        std::size_t iLine = 0ul;
        std::istream& rStream = *_pStream;

        #ifndef NDEBUG
            std::cout << "mtx2img: --- HEADER BEGIN ---\n";
        #endif

        while (rStream.peek() == '%' && !rStream.bad() && !rStream.fail()) /*comment line begins with a '%'*/ {
            // Read the input stream until the buffer is filled
            // or a newline character is encountered.
            rStream.getline(_inputBuffer.data(), _inputBuffer.size());

            // If the fail bit is set, the buffer got filled before
            // a newline character was found in the stream. This means
            // that the input file is invalid.
            if (rStream.fail()) {
                throw ParsingException(std::format(
                    "Error: header line exceeds the maximum length allowed by the specification ({})\n{}...\n",
                    _inputBuffer.size(),
                    _inputBuffer.data()
                ));
            }

            #ifndef NDEBUG
                // Print the header in debug mode.
                std::cout << "mtx2img: " << _inputBuffer.data() << "\n";
            #endif

            // The first line must contain format properties.
            std::match_results<const std::string::value_type*> match;
            if (std::regex_match(_inputBuffer.data(), match, formatPattern)) {
                if (iLine == 0ul) {
                    assert(2 <= match.size());

                    // Parse object type (matrix, vector, etc.)
                    const std::string objectName = match.str(1);
                    if (objectName == "matrix") {
                        _properties.object = format::Object::Matrix;
                    } else if (objectName == "vector") {
                        _properties.object = format::Object::Vector;
                    } else {
                        throw InvalidFormat(std::format(
                            "Error: invalid input object type: {}\n",
                            objectName
                        ));
                    }

                    // Parse format type (coordinate or array)
                    const std::string formatName = match.str(2);
                    if (formatName == "coordinate") {
                        _properties.format = format::Format::Coordinate;
                    } else if (formatName == "array") {
                        _properties.format = format::Format::Array;
                    } else {
                        throw InvalidFormat(std::format(
                            "Error: invalid matrix format: {}\n",
                            formatName
                        ));
                    }

                    // Loop over optional qualifiers (value type, symmetry)
                    if (3 < match.size()) {
                        const std::string qualifiers = match.str(3);
                        for (auto itQualifier = std::sregex_iterator(qualifiers.begin(),
                                                                      qualifiers.end(),
                                                                      qualifierPattern);
                                itQualifier != std::sregex_iterator();
                                ++itQualifier) {
                            const std::string qualifier = itQualifier->str();
                            if (qualifier == "real") {
                                _properties.data = format::Data::Real;
                            } else if (qualifier == "integer") {
                                _properties.data = format::Data::Integer;
                            } else if (qualifier == "complex") {
                                _properties.data = format::Data::Complex;
                            } else if (qualifier == "pattern") {
                                _properties.data = format::Data::Pattern;
                            } else if (qualifier == "general") {
                                _properties.structure = format::Structure::General;
                            } else if (qualifier == "symmetric") {
                                _properties.structure = format::Structure::Symmetric;
                            } else if (qualifier == "skew-symmetric") {
                                _properties.structure = format::Structure::SkewSymmetric;
                            } else if (qualifier == "hermitian") {
                                _properties.structure = format::Structure::Hermitian;
                            } else {
                                throw InvalidFormat(std::format(
                                    "Error: invalid qualifier in input header: {}",
                                    qualifier
                                ));
                            }
                        }
                    }
                } // if iLine == 0

                #ifndef NDEBUG
                else {
                    // Warn if matrix properties are redefined.
                    std::cerr << "mtx2img: WARNING: input redefines matrix properties (redefined properties are ignored):\n";
                }
                #endif
            } else if (iLine == 0ul) {
                throw ParsingException(std::format(
                    "Error: the first line of the input must begin with '%%MatrixMarket' and define the matrix format, but it is\n{}\n",
                    _inputBuffer.data()
                ));
            }

            ++iLine;
        }

        long long rows = 0ul, columns = 0ul, nonzeros = 0ul;

        // Parse number of rows
        rStream >> rows;
        if (rStream.fail()) {
            throw ParsingException("Error: failed to parse the number of rows in the input matrix\n");
        } else if (rows < 0ll) {
            throw ParsingException(std::format(
                "Error: negative number of rows in input: {}\n",
                rows
            ));
        } else {
            _properties.rows = static_cast<std::size_t>(rows);
        }

        // Parse number of columns
        rStream >> columns;
        if (rStream.fail()) {
            throw ParsingException("Error: failed to parse the number of columns in the input matrix\n");
        } else if (columns < 0ll) {
            throw ParsingException(std::format(
                "Error: negative number of columns in input: {}\n",
                columns
            ));
        } else {
            _properties.columns = static_cast<std::size_t>(columns);
        }

        // Parse number of nonzeros
        switch (_properties.format.value()) {
            case format::Format::Coordinate: {
                rStream >> nonzeros;
                if (rStream.fail()) {
                    throw ParsingException("Error: failed to parse the number of nonzeros in the input matrix\n");
                } else if (nonzeros < 0ll) {
                    throw ParsingException(std::format(
                        "Error: negative number of nonzeros in input: {}\n",
                        nonzeros
                    ));
                } else {
                    _properties.nonzeros = static_cast<std::size_t>(nonzeros);
                }
                break;
            } // case format::Format::Coordinate
            case format::Format::Array: {
                _properties.nonzeros = _properties.rows.value() * _properties.columns.value();
                break;
            } // case format::Format::Array
            default:
                throw std::runtime_error(std::format(
                    "Error: unhandled format {}",
                    (int)_properties.format.value()
                ));
        } // switch _properties.format

        // Assign default qualifiers if necessary
        if (!_properties.data.has_value()) {
            _properties.data = format::Data::Real;
        }

        if (!_properties.structure.has_value()) {
            _properties.structure = format::Structure::General;
        }

        // Ignore the rest of the line
        rStream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        #ifndef NDEBUG
            std::cout << "mtx2img: --- HEADER END ---\n";

            // Print matrix properties in debug mode
            std::cout << "mtx2img: input matrix properties:\n"
                      << "mtx2img:     " << _properties.rows.value() << " rows\n"
                      << "mtx2img:     " << _properties.columns.value() << " columns\n"
                      << "mtx2img:     " << _properties.nonzeros.value() << " entries\n";
        #endif
    }

    template <class TValue>
    std::optional<std::conditional_t<
        std::is_same_v<TValue,std::monostate>,
        std::tuple<std::size_t,std::size_t>,        // <== no values requested, only row and column indices
        std::tuple<std::size_t,std::size_t,TValue>  // <== values requested
    >>
    parseSparseDataLine()
    {
        constexpr std::streamsize ignoreSize = std::numeric_limits<std::streamsize>::max();
        using Entry = std::conditional_t<
            std::is_same_v<TValue,std::monostate>,
            std::tuple<std::size_t,std::size_t>,
            std::tuple<std::size_t,std::size_t,TValue>
        >;
        Entry output;
        std::istream& rStream = *_pStream;

        // Read row index
        rStream >> std::get<0>(output);
        if (rStream.eof() || rStream.bad()) {
            rStream.clear();
            rStream.ignore(ignoreSize, '\n');
            return {};
        }

        // Read column index
        rStream >> std::get<1>(output);
        if (rStream.eof() || rStream.bad()) {
            rStream.clear();
            rStream.ignore(ignoreSize, '\n');
            return {};
        }

        // Read value if requested
        if constexpr (!std::is_same_v<TValue,std::monostate>) {
            rStream >> std::get<2>(output);
            if (rStream.eof() || rStream.bad()) {
                rStream.clear();
                rStream.ignore(ignoreSize, '\n');
                return {};
            }
        }

        rStream.ignore(ignoreSize, '\n');

        #ifndef NDEBUG
            // Check indices in debug mode
            // => matrix market indices begin with 1,
            //    so it's usually safe to subtract 1 from them.
            if (std::get<0>(output) == 0ul) {
                std::cerr << "mtx2img: WARNING: unexpected 0-based row index!\n";
            }
            if (std::get<1>(output) == 0ul) {
                std::cerr << "mtx2img: WARNING: unexpected 0-based column index!\n";
            }
        #endif

        // Convert (row,column) indices to 0-based indices
        --std::get<0>(output);
        --std::get<1>(output);

        return output;
    }

    template <class TValue>
    std::optional<std::conditional_t<
        std::is_same_v<TValue,std::monostate>,
        std::tuple<std::size_t,std::size_t>,        // <== no values requested, only row and column indices
        std::tuple<std::size_t,std::size_t,TValue>  // <== values requested
    >>
    parseDenseDataLine()
    {
        constexpr std::streamsize ignoreSize = std::numeric_limits<std::streamsize>::max();
        using Entry = std::conditional_t<
            std::is_same_v<TValue,std::monostate>,
            std::tuple<std::size_t,std::size_t>,
            std::tuple<std::size_t,std::size_t,TValue>
        >;
        Entry output;
        std::istream& rStream = *_pStream;

        // Advance row or column index
        if (_lastPosition.has_value()) {
            if (_properties.columns.value() <= ++_lastPosition->second) {
                _lastPosition = std::make_pair(++_lastPosition->first, 0ul);
            }
        } else {
            _lastPosition = std::make_pair(0ul, 0ul);
        }

        std::get<0>(output) = _lastPosition->first;
        std::get<1>(output) = _lastPosition->second;

        // Read value if requested
        if constexpr (!std::is_same_v<TValue,std::monostate>) {
            rStream >> std::get<2>(output);
            if (rStream.eof() || rStream.bad()) {
                rStream.clear();
                rStream.ignore(ignoreSize, '\n');
                return {};
            }
        }

        return output;
    }

private:
    std::istream* _pStream;

    /// The dense format does not store the row and column indices
    /// in the data lines, so they must be stored separately in the
    /// parser.
    std::optional<std::pair<
        std::size_t,
        std::size_t
    >> _lastPosition;

    std::string _inputBuffer;

    format::Properties _properties;
}; // class Parser


/// @brief Generate the upper triangle from entries in the lower triangle.
template <class TValue, class TTransform>
void fillSymmetricPart(std::span<TValue> nnzMap,
                       std::pair<std::size_t,std::size_t> imageSize,
                       TTransform&& rTransformFunctor)
{
    // Check whether the provided sizes are consistent with the image buffer.
    assert(nnzMap.size() == imageSize.first * imageSize.second);

    // Symmetric matrices must be square, and mtx2img keeps the aspect ratio
    // of the input matrix.
    assert(imageSize.first == imageSize.second);

    for (std::size_t iRow=0; iRow<imageSize.second; ++iRow) {
        for (std::size_t iColumn=0; iColumn<iRow; ++iColumn) {
            const std::size_t iFlat = iRow * imageSize.first + iColumn;
            const std::size_t iSymmetric = iColumn * imageSize.first + iRow;

            // Debug check: the upper triangle should be empty if the
            // symmetric qualifier was set.
            assert(!nnzMap[iSymmetric]);

            nnzMap[iSymmetric] = rTransformFunctor(nnzMap[iFlat]);
        } // for iColumn
    } // for iRow
}


template <Aggregation TAggregation, class TValue, class TPixel>
void registerEntry([[maybe_unused]] const TValue value,
                   TPixel& rPixel)
{
    if constexpr (std::is_same_v<TValue,std::monostate> || std::is_integral_v<TValue>) {
        // No value is available => assume we're just counting the number of entries
        static_assert(std::is_integral_v<TPixel>);
        ++rPixel;
    } else if constexpr (std::is_floating_point_v<TValue>) {
        // Values are real => register their magnitude
        static_assert(std::is_same_v<TValue,TPixel>);
        if constexpr (TAggregation == Aggregation::Sum) {
            rPixel += std::abs(value);
        } else if constexpr (TAggregation == Aggregation::Max) {
            rPixel = std::max(rPixel, std::abs(value));
        } else {
            // Error on unhandled aggregation
            static_assert(TAggregation == Aggregation::Sum);
        }
    } else if constexpr (is_complex_v<TValue>) {
        // Values are complex => register their magnitude
        static_assert(std::is_same_v<typename TValue::value_type,TPixel>);
        if constexpr (TAggregation == Aggregation::Sum) {
            rPixel += std::abs(value);
        } else if constexpr (TAggregation == Aggregation::Max) {
            rPixel = std::max(rPixel, std::abs(value));
        } else {
            // Error on unhandled aggregation
            static_assert(TAggregation == Aggregation::Sum);
        }
    } else {
        static_assert(std::is_same_v<TValue,void>, "Error: unsupported value type");
    }
}


template <Aggregation TAggregation>
void fill(Parser& rParser,
          std::span<unsigned char> image,
          std::pair<std::size_t,std::size_t> imageSize,
          const std::string& rColormapName,
          std::optional<format::Structure> maybeStructure)
{
    format::Properties properties = rParser.getProperties();

    // Nothing to do if the input size is null.
    if (properties.rows.value() == 0ul || properties.columns.value() == 0ul) {
        #ifndef NDEBUG
            std::cout << "mtx2img: nothing to do (degenerate input matrix).\n";
        #endif
        if (properties.nonzeros.value() == 0ul) {
            return;
        } else {
            throw ParsingException(std::format(
                "Error: degenerate input matrix ({}x{}) claims to contain {} nonzeros",
                properties.rows.value(),
                properties.columns.value(),
                properties.nonzeros.value()
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
    const auto colormap = makeColormap(rColormapName);

    // Check image buffer size
    const std::size_t pixelCount = imageSize.first * imageSize.second;
    assert(image.size() == pixelCount * CHANNELS);

    // A buffer for mapping regions in the matrix to each pixel.
    std::vector<std::conditional_t<
        TAggregation == Aggregation::Count,
        unsigned,
        std::conditional_t<
            TAggregation == Aggregation::Sum || TAggregation == Aggregation::Max,
            double,
            std::monostate // <== dummy invalid type
        >
    >> values(pixelCount, 0);

    // Track how many entries were read from the input stream.
    // This will be compared against the expected number of nonzeros.
    std::size_t entryCount = 0ul;

    // Parse the input file and map entries to pixels in the image.
    while (true) {
        const auto maybeEntry = rParser.parseLine<typename decltype(values)::value_type>();
        if (maybeEntry.has_value()) [[likely]] {
            ++entryCount;
            const std::size_t row = std::get<0>(*maybeEntry);
            const std::size_t column = std::get<1>(*maybeEntry);
            const auto value = std::get<2>(*maybeEntry);

            #ifndef NDEBUG
                if (properties.rows.value() <= row) {
                    std::cerr << std::format("mtx2img: row index {} is out of bound {} at entry {}\n",
                                             row, properties.rows.value(), entryCount);
                }
                if (properties.columns.value() <= column) {
                    std::cerr << std::format("mtx2img: column index {} is out of bound {} at entry {}\n",
                                             column, properties.columns.value(), entryCount);
                }
            #endif

            const std::size_t imageRow = row * imageSize.second / properties.rows.value();
            const std::size_t imageColumn = column * imageSize.first / properties.columns.value();
            const std::size_t iFlat = imageRow * imageSize.first + imageColumn;
            assert(iFlat < values.size());
            registerEntry<TAggregation>(value, values[iFlat]);
        } else {
            break;
        }
    } // while (true)

    const auto itMinMax = std::minmax_element(values.begin(), values.end());
    const auto minValue = itMinMax.first != values.end() ? *itMinMax.first : 0;
    const auto maxValue = itMinMax.second != values.end() ? *itMinMax.second : 0;

    // Check the read number of entries
    if (entryCount != properties.nonzeros.value()) {
        throw ParsingException(std::format(
            "Expecting {} entries, but read {}\n",
            properties.nonzeros.value(),
            entryCount
        ));
    }

    #ifndef NDEBUG
        std::cout << std::format("mtx2img: highest aggregate value per pixel is {}\n", maxValue);
    #endif

    // No need to pass through the image again if no
    // entries were read.
    if (minValue == maxValue) {
        return;
    }

    // If the input was provided in symmetric format, the entries
    // read so far were limited to the main diagonal and the lower
    // triangle, so the upper triangle must be filled in separately.
    // Note: currently, all options are handled in the same manner,
    //       but once value-based intensity is enabled, skewness and
    //       negative values will have to be considered.
    if (maybeStructure.has_value()) {
        std::span<typename decltype(values)::value_type> valueRange(values);
        switch (maybeStructure.value()) {
            case format::Structure::General: break; // <== nothing to do
            case format::Structure::Symmetric:
                fillSymmetricPart(valueRange,
                                  imageSize,
                                  [](auto v){return v;});
                break;
            case format::Structure::SkewSymmetric:
                fillSymmetricPart(valueRange,
                                  imageSize,
                                  [](auto v){return -v;});
                break;
            case format::Structure::Hermitian:
                fillSymmetricPart(valueRange,
                                  imageSize,
                                  [](auto v){return v;});
                break;
            default: throw std::runtime_error("Error: missing fill strategy implementation for input matrix structure.");
        }
    }

    // Apply the colormap and fill the image buffer
    const std::size_t maxColor = colormap.empty() ? 0 : colormap.size() - 1;
    for (std::size_t iPixel=0ul; iPixel<pixelCount; ++iPixel) {
        const std::size_t intensity = std::min<std::size_t>(
            maxColor,
            maxColor - (maxColor * (std::max(values[iPixel] - minValue, typename decltype(values)::value_type(0))) / (maxValue - minValue))
        );

        const auto& rColor = colormap[intensity];
        const std::size_t iImageBegin = CHANNELS * iPixel;
        for (std::size_t iComponent=0; iComponent<CHANNELS; ++iComponent) {
            image[iImageBegin + iComponent] = rColor[iComponent];
        }
    }
}


std::vector<unsigned char> convert(std::istream& rStream,
                                   std::size_t& rImageWidth,
                                   std::size_t& rImageHeight,
                                   const Aggregation aggregation,
                                   const std::string& rColormapName)
{
    std::vector<unsigned char> image;
    Parser parser(rStream);
    const format::Properties inputProperties = parser.getProperties();

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
            case format::Format::Array: break; // <== ok
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
        const std::pair<std::size_t,std::size_t> requestedImageSize {rImageWidth, rImageHeight};
    #endif

    // Preserve the aspect ratio of the input matrix (as much as possible),
    // by restricting the resolution of the output image corresponding to the
    // shorter dimension.
    if (inputProperties.columns.value() < inputProperties.rows.value()) {
        rImageWidth = std::max(inputProperties.columns.value() * rImageWidth / inputProperties.rows.value(), 1ul);
    } else {
        rImageHeight = std::max(inputProperties.rows.value() * rImageHeight / inputProperties.columns.value(), 1ul);
    }

    // Restrict output image size
    if (inputProperties.columns.value() < rImageWidth) {
        rImageWidth = inputProperties.columns.value();
        rImageHeight = std::max(inputProperties.rows.value() * inputProperties.columns.value() / rImageWidth, 1ul);
    }

    if (inputProperties.rows.value() < rImageHeight) {
        rImageHeight = inputProperties.rows.value();
        rImageWidth = std::max(inputProperties.columns.value() * inputProperties.rows.value() / rImageHeight, 1ul);
    }

    #ifndef NDEBUG
        // Print changes to the output dimension in debug mode
        if (rImageWidth != requestedImageSize.first || rImageHeight != requestedImageSize.second) {
            std::cout << std::format("mtx2img: restrict output image size from {}x{} to {}x{}\n",
                requestedImageSize.first,
                requestedImageSize.second,
                rImageWidth,
                rImageHeight
            );
        }
    #endif

    const std::pair<std::size_t,std::size_t> imageSize {rImageWidth, rImageHeight};

    // Resize image buffer to final size and initialize it to full white
    image.resize(imageSize.first * imageSize.second * CHANNELS, 0xff);

    // Parse input stream and fill the output image buffer
    switch (aggregation) {
        #define MTX2IMG_FILL(AGGREGATION)                                                       \
            fill<AGGREGATION>(parser,                       /* mtx/mm parser                */  \
                              image,                        /* buffer                       */  \
                              imageSize,                    /* buffer dimensions            */  \
                              rColormapName,                /* name of the colormap to use  */  \
                              inputProperties.structure)    /* input matrix symmetry)       */
        case Aggregation::Count:    MTX2IMG_FILL(Aggregation::Count);   break;
        case Aggregation::Sum:      MTX2IMG_FILL(Aggregation::Sum);     break;
        case Aggregation::Max:      MTX2IMG_FILL(Aggregation::Max);     break;
        #undef MTX2IMG_FILL
        default:
            throw std::runtime_error(std::format(
                "Error: missing implementation for aggregation {}\n",
                (int)aggregation
            ));
    }

    return image;
}


} // namespace mtx2img
