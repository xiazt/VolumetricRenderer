#include "Image3DFromTIFFFile.hpp"

#include <tiffio.h>

bool Image3DFromTIFFFileSequence(Image3D* image, std::vector<std::string> fileNames)
{
	//Check for all same size in stack
	uint32_t width = 0;
	uint32_t height = 0; 
	bool firstRun = true; 
	for(int i = 0; i < fileNames.size(); i++)
	{
		TIFF* tif = TIFFOpen(fileNames[i].c_str(), "r");
		
		if (tif) 
		{
			uint32_t w, h;
			TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
			TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
			if(firstRun)
			{
				width = w;
				height = h;
				firstRun = false; 
			}
			else
			{
				if(w != width || h != height)	
				{
					std::cerr << "Image3DFromTIFFFileSequence: Images not the same size" << fileNames[i] << std::endl;
					return false; 
				}
			}
			TIFFClose(tif);
		}
	}
	
	//Check if images have some wifdth and height
	if(width <= 0 || height <= 0)
	{
		std::cerr << "Image3DFromTIFFFileSequence: Images have zero size" << std::endl;
		return false;
	}
	
	//Allocate Image
	image->Allocate(width, height, fileNames.size(), 4);
	
	//Load Data
	for(int i = 0; i < fileNames.size(); i++)
	{
		TIFF* tif = TIFFOpen(fileNames[i].c_str(), "r");
		
		if (tif) 
		{
			unsigned char* imageData = (unsigned char*)image->Data() + width * height * 4 * i;
			TIFFReadRGBAImage(tif, width, height, (uint32_t*)imageData, 0);
			TIFFClose(tif);
		}
	}
	
	return true; 
}


bool Image3DFromTIFFFile(Image3D* image, std::string fileName)
{
	 
	TIFF* tif = TIFFOpen(fileName.c_str(), "r");
		
	if (!tif) 
	{
		std::cout << "Image3DFromTIFFFile:" << "failed to open file" << std::endl; 
		return false; 
	}
	
	uint32_t width = 0;
	uint32_t height = 0; 
	bool firstRun = true;
	
	int depth = 0;
	
	while(TIFFReadDirectory(tif))
	{
	
		uint32_t w, h;
		TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
		TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
		if(firstRun)
		{
			width = w;
			height = h;
			firstRun = false; 
		}
		else
		{
			if(w != width || h != height)	
			{
				std::cerr << "Image3DFromTIFFFile: Images not the same size" << fileName << std::endl;
				return false; 
			}
		}
		depth++; 
	}
	
	TIFFClose(tif);
	
	
	//Check if images have some wifdth and height
	if(width <= 0 || height <= 0)
	{
		std::cerr << "Image3DFromTIFFFile: Images have zero size" << std::endl;
		return false;
	}
	
	//Allocate Image
	image->Allocate(width, height, depth, 4);
	
	//Load Data
	tif = TIFFOpen(fileName.c_str(), "r");
	
	if (!tif) 
	{
		std::cout << "Image3DFromTIFFFile:" << "failed to open file second time around" << std::endl; 
		return false; 
	}

	int i = 0;
	while(TIFFReadDirectory(tif))
	{
		unsigned char* imageData = (unsigned char*)image->Data() + width * height * 4 * i;
		
		TIFFReadRGBAImage(tif, width, height, (uint32_t*)imageData, 0);
		for(int k = 0; k < width * height; k++)
			std::cout << (int)(imageData[k * 4 + 0]) << " " << (int)(imageData[k * 4 + 1]) << " " << (int)(imageData[k * 4 + 2]) << " " << (int)(imageData[k * 4 + 3]) << std::endl; 
		
		i++;
	}
	
	TIFFClose(tif);

	
	return true; 
	
	
}
