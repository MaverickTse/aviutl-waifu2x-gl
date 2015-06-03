# aviutl-waifu2x-gl
AviUtl port for the upscaling algorithm, waifu2x. OpenGL version.

## Binary Download
[HERE](https://github.com/MaverickTse/aviutl-waifu2x-gl/releases/download/v0002/aviutl-wainfu2x-gl-20150603.7z)
or visit the Release Page

## Installation
Put everything in your AviUtl installation folder and keep the archive's folder structure. __DO NOT PUT INTO the ./Plugin FOLDER !!!__

## System requirement
* Win7 or later
* CPU supporting at least SSE4.2 (Since I compiled the release using Intel Compiler with SSE4.2 optimization)
* at least 4GB RAM
* 64bit OS recommended
* AviUtl 0.99m or later
* Intel HD3000 or better video card/chipset

## Speed
On Core-i7-2600k,8 threads, EVGA GTX570 SC:
* 640x360 → 1280x720 upscale only = 7321~9273ms (~0.12fps)
* 320x320 → 640x640 weak denoise+upscale = 4372~5491ms (~0.2fps)

## Known Bug
* The GLSL scripts might not work on all machines!


## Implemented behaviour
* If the upscaled image is larger than the max resolution in your AviUtl setting, it clips off the right and bottom.
* Suggest to leave the _Block_ slider alone. Increasing this value too much will slow down operation.

## For Developers
* To build this yourself, you first need to get OpenCV3, GLEL and GLFW. Personally I have trimmed a lot of stuff from OpenCV and packed everything as a single DLL using the _world_ module.
* All processing are done in YUV. No RGB<>YUV takes place.
* The Y-plane is first doubled using NearestNeighbour, then feed into the machine-learned pattern to do fine tuning.
* UV-planes are straight Bicubic upscale using OpenCV functions.
* With the main.cpp, it is now easy to port other waifu2x implementations with similar file structure... in theory
 

## Original Code Bases Referenced
[Vanilla](https://github.com/nagadomi/waifu2x)

[OpenGL port](https://github.com/ueshita/waifu2x-converter-glsl)
