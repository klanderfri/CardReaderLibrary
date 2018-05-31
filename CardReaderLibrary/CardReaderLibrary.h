// CardReaderLibrary.h - Contains declarations of card reader methods.
#pragma once

#ifdef LIBRARY_EXPORTS
#    define LIBRARY_API __declspec(dllexport)
#else
#    define LIBRARY_API __declspec(dllimport)
#endif

namespace CRLibrary {

	// -- Method reading the card image files. --
	//
	// - Input arguments format -
	//
	// Header:
	// bool;bool;bool;bool;string;
	// <run silent>;<run parallelized>;<store extracted card images>;<run debugging>;<full path to output folder>;
	//
	// The header will be followed by the body, specifying which images are to be read.
	// string;string;string;
	// <full file path 1>;<full file path 2>;<full file path 3>;
	//
	// Example:
	// "0;1;1;0;C:\\MtG-cards\\Output;C:\\Users\\BjornLa\\Pictures\\MtG-cards\\Arterial Flow.jpg;C:\\Users\\BjornLa\\Pictures\\MtG-cards\\Buccaneer's Bravado.jpg;"
	//
	// - Result output format -
	//
	// The result will be given as another string in which each read result will be presented:
	// string;string;int;int;bool
	// <original full file path>;<extracted card name>;<card type>;<ocr read confidence>;<successful read>
	//
	// Example:
	// "C:\\Users\\BjornLa\\Pictures\\MtG-cards\\Arterial Flow.jpg;Arterial Flow;1;82;1;C:\\MTG-card\\Zombie - Token.jpg;ZOMBIE;8;79;1;"
	//
	// NOTE!
	// The lenght of the result depends mostly on the length of the file paths.
	// Use this formula to calculate the approximal lenght (hopefully with some margin):
	// ((average path length) + 40) * (number of files)
	//
	// If the entire read failed then the output will contain the error messages:
	// ERROR: <description of error>
	// If only a specific card failed to read then the error message will be placed in the card name slot.
	// 
	// Example:
	// "ERROR: To many files to read!;ERROR: File 'D:\\MTG-card\\Zombie - Token.jpg' does not exist!;"
	// "C:\Users\BjornLa\Pictures\MtG-cards\Problematic Testcases\Old design\Llanowar Elves - Rotated.jpg;ERROR: Could not OCR-read the title!;1;0;0;"
	//
	// -- IMPORTANT! --
	// You must delete the pointer to the result data when you are done with the result!
	extern "C" LIBRARY_API void __cdecl ReadCardTitles(char const* input, char* result, int maxLength);

	// Method comparing two card names.
	//
	// - Result output format -
	// true: The names are in the correct order.
	// false: The names are NOT sorted correctly and need to switch places.
	extern "C" LIBRARY_API bool CompareCardNames(char const* cardName1, char const* cardName2);

	// Method giving how many card images that could be handled in a single run.
	//
	// - Result output format -
	// int: The maximum amount of cards one single call to the ReadCardTitles method could handle.
	extern "C" LIBRARY_API int GetMaxCardAmount();

	// Method giving an example of how the card reading method might return.
	//
	// - Result output format -
	// string: Same as the ReadCardTitles method.
	extern "C" LIBRARY_API void __cdecl GetResultExample(char* result, int maxLength);
}