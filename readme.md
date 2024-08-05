## Usage

Convert [matrix market](https://math.nist.gov/MatrixMarket/formats.html#MMformat) files to images.

<p align="center">
<img src=".github/assets/cube_isoparametric_quadratic_tets.png" width=300/> <img src=".github/assets/rbs480a.png" width=300/>
</p>

`mtx2img <input-path> <output-path> [-r <output-resolution>] [-a <aggregation-method>] [-c <colormap-name>]`

Required arguments:
- `<input-path>`: path pointing to an existing MatrixMarket file (*.mtx* or *.mm*). It must use the *coordinate* format (i.e.: represent a sparse matrix). Alternatively, `-` can be passed to read the same format from *stdin* instead of a file.
- `<output-path>`: the output image will be written here. If a file already exists, it will be overwritten. If the path exists but is not a file, the program will fail without touching the output path. Alternatively, `-` can be passed to write the output image to *stdout*.

Optional arguments:
- `[-r <output-resolution>]`: highest resolution of the output image in pixels. This setting is overridden if the number of rows or columns in the input matrix is less than the provided width, but the its aspect ratio as always preserved (closest ratio representable by the requested resolution). The default value is 1080.
- `[-a <aggregation-method>]`: controls how matrix entries are aggregated to pixels.
   - `count`: compute the ratio of nonzero entries referencing the same pixel (default)
   - `sum`: accumulates the absoltute value of entries for each pixel
- `[-c <colormap-name>]`: name of the colormap to apply on pixels. If the matrix dimensions are larger than the output image dimensions, multiple matrix entries may end up getting mapped to the same pixel. The program aggregates these entries for each pixel (using the aggregation method set by the `-a` flag), and normalizes the comuted values after reading the matrix. The `-c` option controls how these aggregated values are mapped to RGB colors in the output image.
   - `binary`: any pixel with at least one nonzero mapping to it is black; the rest are white (default)
   - [`kindlmann`](https://www.kennethmoreland.com/color-advice/#extended-kindlmann) (extended)
   - [`viridis`](https://www.kennethmoreland.com/color-advice/#viridis)
   - [`glasbey256`](https://strathprints.strath.ac.uk/30312/1/colorpaper_2006.pdf)
   - [`glasbey64`](https://strathprints.strath.ac.uk/30312/1/colorpaper_2006.pdf)
   - [`glasbey8`](https://strathprints.strath.ac.uk/30312/1/colorpaper_2006.pdf)

## Installation

### Precompiled binary

Precompiled binaries are available from the "Release" section of the repository.

*Recently built binaries are also accessible as artifacts of GitHub CI runs.*

### Build from source

Requirements:
- git (or just download the source manually)
- a C++20 compiler (touchy subject I know ... make sure it supports `std::format` and `ranges`)
- CMake >= 3.15.0 [optional]

```bash
git clone https://github.com/matekelemen/mtx2img.git
```

You have 3 options to build the project. These are, in descending order of preference:
1) using CMake. For example:
```bash
cmake -H<path-to-repo> -B<path-to-build-dir> -DCMAKE_INSTALL_PREFIX=<path-to-install-dir>
cmake --build <path-to-build-dir> --target install --config Release
```
2) using the provided `makefile` (expects `g++`):
```bash
cd <path-to-repo-root>
make
```
3) using the `build.sh` shell script (disclaimer: it's meant for development and uses CMake internally):
```bash
<path_to_repo_root>/build.sh -t Release -i <path-to-install-dir>
```
