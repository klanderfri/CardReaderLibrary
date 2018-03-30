# Read Magic Card
A computer vision project for identifying Magic: the Gathering-cards.

## Before You Start
It's recommended that you install Vcpkg and libraries required by this project
BEFORE opening/installing/starting this project!

## Checklist
Follow this checklist when setting up the project to avoid non-code problems.
 * Check that the folder C:\Users\<username>\Pictures\MtG-cards exists.
 * It contains images with 1 MtG-card in each.
 * Remove all files that are neither folders nor jpg-files.
 * ~~Check that the folder C:\Program Files\Tesseract-OCR\tessdata exists.~~
 * ~~Make sure it contains the file eng.traineddata of the correct version (check Vcpkg to see which version).~~
 * Don't worry about the Tesseract trained data file. It should be handled and used by the project code now! :-D
 
## Environment
This project is written with the C++17 standard.

This project is created in Microsoft Visual Studio Community 2017 version 15.6.4

## Vcpkg
It's highly recommended to use Vcpkg to install third-party frameworks.
Vcpkg makes the installations quick and easy.
 * https://github.com/Microsoft/vcpkg
 * https://blogs.msdn.microsoft.com/vcblog/2016/09/19/vcpkg-a-tool-to-acquire-and-build-c-open-source-libraries-on-windows/
 * https://docs.microsoft.com/en-us/cpp/vcpkg


## Libraries
This project uses several third-party libraries, listed below.

The easiest way to get access to them is probably to install them with Vcpkg.
 * boost
 * leptonica
 * opencv
 * tesseract

## Includes
Includes should be added in this order:
 * stdafx.h
 * The header file of the file.
 * Ocv-header files.
 * <include> headers.
 * OpenCV headers.
 * Boost headers.
 * Leptonica headers.
 * Tesseract headers.
 Make sure to use \ and not / when applicable.

## OS Specific Code
The code is meant to run on Windows.

If you add OS-specific code, make sure you use the OcvWindows class or
a similar class derived from the OcvSystemDependencyClass class!

## Test Data
A zip files with test data (aka photos of MtG cards) can be downloaded from the link below.
http://tankarochkuriosa.se/documents/mtg-cards.zip (163 MB)

## About Compile Warnings
There should be NO warnings. Warnings are just as bad as errors. If you get warnings, you're doing it wrong!
