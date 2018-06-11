# Read Magic Card
An open source computer vision library for *Magic: the Gathering* card recognition.

## I just want to run the code!
Then you're at the WRONG place! You want the C++ or the C# client. They can be found in the repositories below.
 * https://github.com/klanderfri/CppCardReaderClient
 * https://github.com/klanderfri/CsCardReaderClient

## Where do I get the result data?
The library will present the results in two ways.
 1. As a character array returned by the library method reading the cards (see the *CardReaderLibrary.h* file for more information).
 2. In a tab separated file named *CardTitles.txt* in the subfolder *MtG-cards\Image Data* in your image folder.

## What else to know?
This project is a library provided as a DLL, currently compiled in Windows. You need to know how to handle DLLs to be able to use this library in your projects.

Developers and curious users might want to head for the CONTRIBUTING.md file though.
