#pragma once
#include "OcvSystemDependencyClass.h"
//Base class for classes that handles card or part of cards.
class OcvCardHandler
{
public:
	//Type used by OpenCV methods to represent hierarchies when finding contours.
	typedef std::vector<cv::Vec4i> Hierarchy;

	OcvCardHandler(std::wstring imageFileName, cv::Mat imageData, OcvSystemDependencyClass* systemMethods, bool debuggingOn);
	~OcvCardHandler();

protected:

	//The height of the shrinked card image we work with to maximize performance.
	const int WORKING_CARD_HEIGHT;

	//The name of the file with the card the reader should handle.
	std::wstring imageFileName;
	//The image data of the image containing the card.
	cv::Mat originalImageData;
	//Telling if the program should do debugging such as exporting the images to files.
	bool debuggingOn;
	//Pointer to object handling the system dependent methods.
	OcvSystemDependencyClass* systemMethods;
};
