# NeuroKit

## Prerequisites
* The application is developed and tested under `Windows 10`.
* A recent Graphics Card (GPU). I used an `NVIDIA GeForce GTX TITAN Black` GPU. 
* At least `32 GB of RAM`
* A internet connection (to use the D3 library)

## Getting Started
To get started you first need to **install all required dependencies**. In brackets I noted the versions that were used during development and testing.
* [cmake](https://cmake.org/)
* [QT](https://www.qt.io/download-open-source?hsCtaTracking=9f6a2170-a938-42df-a8e2-a9f0b1d6cdce%7C6cb0de4f-9bb5-4778-ab02-bfb62735f3e5) open source (5.15.0)
* [CGGAL](https://www.cgal.org/download.html) (5.0.1) library, CGAL also requires [boost](https://www.boost.org/) (1.71.0)
* [cereal](https://uscilab.github.io/cereal/) serialization library (1.3.0)

## Building the project
1. Set these **environmental variables**. Check [this](https://www.youtube.com/watch?v=bEroNNzqlF4) tutorial on how to set environmental variables. 
* `Qt5_DIR` to `C:/Qt/5.15.0/msvc2017_64/lib/cmake/Qt5`
* `QT_QPA_PLATFORM_PLUGIN_PATH` to `C:\Qt\5.15.0\msvc2017_64\plugins\platforms`
* Set the `Path` environmental variable to the directory of the Qt .dll files. In my case this is `C:\Qt\5.15.0\msvc2017_64\bin`
* `BOOST_INCLUDEDIR` to `C:\dev\boost_1_71_0`
* `BOOST_LIBRARYDIR` to `C:\dev\boost_1_71_0\lib64-msvc-14.2`
* `CGAL_DIR` to `C:\dev\CGAL-5.0.1`
* `CEREAL_DIR` to `C:\dev\cereal-1.3.0\include`


Note that you do not have to restart your machine after setting the environment variables. **Restarting the programs (Visual Studio, the command line, CMake, etc.) is enough the apply the changes**.  

2. Create a `build` directory on the same level as the `src` directory
3. Run CMake and use the src dir as the source dir and the newly created build dir

## Datasets
Soon you can find data sets here TODO

## FAQs
Check out the FAQs for further development and more [here](https://github.com/jakobtroidl/NeuroKit/wiki/FAQs). 



