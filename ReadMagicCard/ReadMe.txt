
It's recommended that you install Vcpkg and libraries required by this project
BEFORE opening/installing/starting this project!


Checklist for running this program:
 * The folder C:\Users\<usernam>\Pictures\MtG-cards exists.
 * It contains images with 1 MtG-card in each.
 * Remove all files that are neither folders nor jpg-files.
 * The folder C:\Program Files\Tesseract-OCR\tessdata exists.
 * It contains the file eng.traineddata of the correct version (check Vcpkg to see which version).
 
 
 This project is written with the C++17 standard.
 This project is created in Microsoft Visual Studio Community 2017 version 15.6.4


It's highly recommended to use Vcpkg to install third-party frameworks.
Vcpkg makes the installations quick and easy.
https://github.com/Microsoft/vcpkg
https://blogs.msdn.microsoft.com/vcblog/2016/09/19/vcpkg-a-tool-to-acquire-and-build-c-open-source-libraries-on-windows/
https://docs.microsoft.com/en-us/cpp/vcpkg


Libraries installed by Vcpkg and used in this project:
 * boost
 * leptonica
 * opencv
 * tesseract


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


The code is meant to run on Windows.
If you add OS-specific code, make sure you use the OcvWindows class or
a similar class derived from the OcvSystemDependencyClass class!
