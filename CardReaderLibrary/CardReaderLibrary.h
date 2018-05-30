// CardReaderLibrary.h - Contains declarations of card reader methods.
#pragma once

#ifdef CARDREADERLIBRARY_EXPORTS  
#define CARDREADERLIBRARY_EXPORTS __declspec(dllexport)   
#else  
#define CARDREADERLIBRARY_EXPORTS __declspec(dllimport)   
#endif

namespace CardReaderLibrary {

	class CRLibrary {
	public:
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
		// If the entire read failed then the output will contain the error messages:
		// ERROR: <description of error>
		// 
		// Example:
		// "ERROR: To many files to read!;ERROR: File 'D:\\MTG-card\\Zombie - Token.jpg' does not exist!;
		//
		// -- IMPORTANT! --
		// You must delete the pointer to the result data when you are done with the result!
		static CARDREADERLIBRARY_EXPORTS char const* ReadCardTitles(char const* input);
		
		// Method comparing two card names.
		//
		// - Result output format -
		// true: The names are in the correct order.
		// false: The names are NOT sorted correctly and need to switch places.
		static CARDREADERLIBRARY_EXPORTS bool CompareCardNames(char const* cardName1, char const* cardName2);

		// Method giving how many card images that could be handled in a single run.
		//
		// - Result output format -
		// int: The maximum amount of cards one single call to the ReadCardTitles method could handle.
		static CARDREADERLIBRARY_EXPORTS int GetMaxCardAmount();
	};
}
