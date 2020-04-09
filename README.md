# NeCo - Neuro Comparer

## Prerequisits
* The application is developed and tested under `Windows 10`.
* A recent Graphics Card (GPU). I used an `NVIDIA GeForce GTX TITAN Black` GPU. 
* At least `XXX GB of RAM`

## Getting Started
To get started you first need to **install all required dependencies**.
* [cmake](https://cmake.org/)
* open source version of [QT](https://www.qt.io/download-open-source?hsCtaTracking=9f6a2170-a938-42df-a8e2-a9f0b1d6cdce%7C6cb0de4f-9bb5-4778-ab02-bfb62735f3e5)
* [CGGAL](https://www.cgal.org/download.html) library, CGAL also requires [boost](https://www.boost.org/)
* [cereal](https://uscilab.github.io/cereal/) serialization library

## Building the project
1. Set these **environmental variables**. Check [this](https://www.youtube.com/watch?v=bEroNNzqlF4) tutorial on how to set environmental variables. 
* `Qt5_DIR` to `C:/Qt/5.14.1/msvc2017_64/lib/cmake/Qt5`
* `QT_QPA_PLATFORM_PLUGIN_PATH` to `C:\Qt\5.14.1\msvc2017_64\plugins\platforms`
* Set the `Path` environmental variable to directory of the QT .dll files like `C:\Qt\5.14.2\msvc2017_64\bin`
* `BOOST_INCLUDEDIR` to `C:\dev\boost_1_71_0`
* `BOOST_LIBRARYDIR` to `C:\dev\boost_1_71_0\lib64-msvc-14.2`
* `CGAL_DIR` to `C:\dev\CGAL-5.0.1`
* `CEREAL_DIR` to `C:\dev\cereal-1.3.0\include`

Note that you do not have to restart youre machine after setting the environmental variables. **Restarting the programs (Visual Studio, the command line, cmake etc.) is enough the apply the changes**.  

## Data Preprocessing
For data preprocessing and input data specification please refer to this [repo](https://github.com/jakobtroidl/neco_convert). 



