# Contributing to this project

## Getting it running

### Before You Start
 * It's recommended that you install Vcpkg and libraries required by this project BEFORE opening/installing/starting this project!
 * Make sure the testcases works before doing any coding.

### Application Data Folder
Follow this checklist to set up the folder the application reads and writes data to:
 * Check that the folder *C:\Users\\&lt;username&gt;\Pictures\MtG-cards* exists.
 * It contains images with one (1) MtG-card in each (see *Testing*).
 * Remove all files that are neither folders nor jpg-files.

## Environment
This project is written with the C++17 standard.

This project is created in Microsoft Visual Studio Community 2017 version 15.6.4

### Image Watch
*Image Watch* is a tool for debugging and seeing the images when coding in Visual Studio.
 * https://marketplace.visualstudio.com/items?itemName=VisualCPPTeam.ImageWatch2017

### Vcpkg
It's highly recommended to use Vcpkg to install third-party frameworks.
Vcpkg makes the installations quick and easy.
 * https://github.com/Microsoft/vcpkg
 * https://blogs.msdn.microsoft.com/vcblog/2016/09/19/vcpkg-a-tool-to-acquire-and-build-c-open-source-libraries-on-windows/
 * https://docs.microsoft.com/en-us/cpp/vcpkg

### Libraries
This project uses several third-party libraries, listed below.

The easiest way to get access to them is probably to install them with Vcpkg.
 * boost
 * leptonica
 * opencv
 * tesseract

## Coding

### Includes
Includes should be added in this order:
 * stdafx.h
 * The header file of the file.
 * Other solution header files.
 * &lt;include&gt; headers.
 * OpenCV headers.
 * Boost headers.
 * Leptonica headers.
 * Tesseract headers.

Make sure to use \ and not / when applicable.

### OS Specific Code
The code is meant to run on Windows.

If you add OS-specific code, make sure you use the WindowsMethods class or
a similar class derived from the SystemMethods class!

### About Compile Warnings
There should be NO warnings. Warnings are just as bad as errors. If you get warnings, you're doing it wrong!

### Strings
One should use *wstring* when coding strings since this code is meant to run on Windows.

### Language
British English should be used everywere where it's possible. American English should be avoided.

## Testing
The application has a test case algorithm which is activated when the application is set to do debugging (see ReadMe.md).
It will check if there are any broken test cases, indicating broken code. You'll have to download the test data for it to work properly.

### Test Data
A zip files with test data (aka photos of *Magic: the Gathering*-cards) can be downloaded from the link below.
http://tankarochkuriosa.se/documents/mtg-cards.zip (106 MB)
