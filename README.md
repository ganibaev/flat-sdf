# Flat SDF

<img src="https://user-images.githubusercontent.com/55918604/226019933-e0a0e58a-e4d7-4170-bbf7-220fba907e12.png" width="300">

This is a repository for my software 2D SDF renderer written in C++ from scratch.

The renderer generates (or loads from file) all Signed Distance Fields as 256x256 textures, and then uses multithreading to sample the distance fields for every pixel.

It supports magnification and minification of all shapes, color gradients, colored strokes around shapes and antialiasing via edge blurring.

## Running the code

The program saves the output files in `.bmp` format with rendered scenes to the root folder of the project. To build and run the program via CMake (version 3.5 or higher), you can use the standard commands executed from the root folder of the project:

```
mkdir build
cd build
cmake ..
make
./flat-sdf
```

The program also outputs the time spent on rendering each scene to the console via `stdout`.

## Acknowledgements

This project was made as part of the Forward and Inverse Rendering course at MSU, and it uses the course's logo (blue flame) in example scenes 1 and 2.
