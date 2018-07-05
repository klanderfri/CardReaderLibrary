#pragma once
#include "ImageHandlerBase.h"
//Class cleaning an OCR-image from noises that might disturb and interfere the decoding.
class OcrImageNoiseCleaner :
	public ImageHandlerBase
{
public:
	OcrImageNoiseCleaner(Session* session, std::wstring imageFilePath, cv::Mat originalImageData);
	~OcrImageNoiseCleaner();

	////Cleans the text images from noise and clutter.
	void CleanImage(cv::Mat& dirtyImage);

private:

	//Handles a single figure (noise or letter).
	void handleFigure(FigureArea figure, cv::Mat& dirtyImage);
	//Checks if a figure is noise.
	bool isNoise(FigureArea figure, cv::Size imageArea);
	//Checks if a figure is too long and thin to be a letter.
	bool isOblong(FigureArea figure);
	//Checks if a figure is so small it can be considered noise.
	bool isSmallEnoughToBeNoise(FigureArea figure);
	//Checks if a figure is in the middle of the text image.
	bool isInCenter(FigureArea figure, cv::Size imageArea);
};

