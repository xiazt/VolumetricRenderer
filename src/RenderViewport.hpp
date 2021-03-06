#pragma once


#include "Common.hpp"


#include "Image3D.hpp"
#include "VolumeData.hpp"
#include "Renderer/CameraObject.hpp"
#include "Renderer/TextureVolumeObject.hpp"
#include "Renderer/RayVolumeObject.hpp"
#include "Renderer/PhotonVolumeObject.hpp"
#include "Renderer/TextureSliceObject.hpp"
#include "Renderer/Texture3D.hpp"
#include "Renderer/TextureCube.hpp"
#include "Renderer/Texture1D.hpp"
#include "Renderer/CameraControl3D.hpp"
#include "Renderer/CameraControl2D.hpp"
#include "Renderer/AxisObject.hpp"


class RenderViewport: public QOpenGLWidget
{
	Q_OBJECT
	protected:
		int windowWidth;
		int windowHeight; 
	
		void initializeGL();
		void paintGL();
		void resizeGL(int w, int h);
		
		void mousePressEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
		void wheelEvent(QWheelEvent *event);
		void keyPressEvent(QKeyEvent *event);
		void keyReleaseEvent(QKeyEvent *event);
		
	public:
		enum RENDER_TYPE{IMAGE2D_RENDERER, SLICE_RENDER, RAY_RENDER, PHOTON_RENDER}; 
		RENDER_TYPE renderType; 
		CameraObject* cameraObject; 
		CameraControl3D* cameraControl3D; 
		CameraControl2D* cameraControl2D; 
		TextureVolumeObject* textureVolumeObject; 
		RayVolumeObject* rayVolumeObject; 
		PhotonVolumeObject* photonVolumeObject;
		TextureSliceObject* textureSliceObject;
		Texture3D* textureVolume; 
		Texture3D* textureGradient; 
		TextureCube* textureEnvMap; 
		Texture1D* textureLUT; 
		AxisObject* axisObject;
		std::vector<float> textureVolumeHistogram;
		VolumeData* volumeData; 
		
		RenderViewport();
		
	public slots:
		void EnableDisableAxis(bool en);
		void LoadLUT(float* buffer, int sizeLUT);
		void LoadDefaultEnvMap();
		void ChooseRenderer(RENDER_TYPE rt);
		void SetGradientThreshold(float threshold);
		void SetBackFaceCulling(bool cull); 
		void SetBrightness(double b); 
		void SetContrast(double c); 
		void SetThreshold(double t); 
		void Refresh();
};