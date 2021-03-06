#pragma once


#include "Common.hpp"


class Image3D
{
	protected:
		uint64_t width;
		uint64_t height;
		uint64_t depth;
		uint64_t pixelSize;
		void* data;
		
	public:
		Image3D();
		Image3D(uint64_t W, uint64_t H, uint64_t D, uint64_t P);
		~Image3D();
		void Allocate(uint64_t W, uint64_t H, uint64_t D, uint64_t P);
		void Deallocate(); 
		void* Data();
		uint64_t Width();
		uint64_t Height();
		uint64_t Depth();
		uint64_t ByteSize();
		void Copy(Image3D& inImg);
		void Smooth2D();
		void Median2D();
		void Smooth();
		void Median();
		void CentralDifference(Image3D& inImg);
		void Sobel(Image3D& inImg);
		void Sobel2(Image3D& inImg);
		void Normalize();
		void Histogram(std::vector<float>* histogram);
		void BrightnessContrastThreshold(double brightness, double contrast, double threshold);
};