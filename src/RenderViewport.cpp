#include "RenderViewport.hpp"


#include "Util.hpp"

#include "TestGenerateVolume.hpp"

#include "IO/Image3DFromDicomFile.hpp"
#include "IO/Image3DFromTIFFFile.hpp"


RenderViewport::RenderViewport()
{
	setFocusPolicy(Qt::ClickFocus);
	renderType = SLICE_RENDER; 
}

void RenderViewport::initializeGL()
{
	if(!isValid())
	{
		std::cout << "RenderViewport: QSurfaceFormat: surface failed" << std::endl; 
		return; 
	}
	
	OPENGL_FUNC_MACRO
	if(!ogl)
	{
		std::cout << "RenderViewport: ogl functions could not be loaded" << std::endl; 
		return; 
	}
	
    ogl->glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	cameraObject = new CameraObject;
	cameraControl = new CameraControl(cameraObject);
	
	connect(cameraControl, &CameraControl::CameraUpdated, [this](){update(); });
	
	TextureVolumeObject::InitSystem(); 
	textureVolumeObject = new TextureVolumeObject;
	textureVolumeObject->Init();
	
	RayVolumeObject::InitSystem();
	rayVolumeObject = new RayVolumeObject;
	rayVolumeObject->Init();
	
	textureVolumeObject->SetVisible(true); 
	rayVolumeObject->SetVisible(false); 
	
	textureVolume = new Texture3D; 
	
	textureGradient = new Texture3D;
	
	textureLUT = new Texture1D;
	
	//Load Default LUT texture
	int sizeLUT = 256; 
	std::vector<float> bufferLUT(sizeLUT * 4);
	for(int i = 0; i < sizeLUT; i++)
	{
		bufferLUT[i * 4 + 0] = (double)i / (double)sizeLUT;  
		bufferLUT[i * 4 + 1] = (double)i / (double)sizeLUT; 
		bufferLUT[i * 4 + 2] = (double)i / (double)sizeLUT; 
		bufferLUT[i * 4 + 3] = (double)i / (double)sizeLUT; 
	}
	LoadLUT(&bufferLUT[0], sizeLUT);

	textureVolumeObject->SetVolumeTexture(textureVolume); 
	textureVolumeObject->SetGradientTexture(textureGradient); 
	textureVolumeObject->SetLUTTexture(textureLUT); 
	
	rayVolumeObject->SetVolumeTexture(textureVolume); 
	rayVolumeObject->SetGradientTexture(textureGradient); 
	rayVolumeObject->SetLUTTexture(textureLUT); 
	
	axisObject = new AxisObject();
	AxisObject::InitSystem(); 
	axisObject->Init();
	axisObject->SetVisible(false);
}

void RenderViewport::resizeGL(int w, int h)
{
	windowWidth = w;
	windowHeight = h;
}

void RenderViewport::paintGL()
{
	OPENGL_FUNC_MACRO
	if(!ogl)
	{
		std::cout << "RenderViewport: ogl functions could not be loaded" << std::endl; 
		return; 
	}
	

    ogl->glClear(GL_COLOR_BUFFER_BIT);
	ogl->glClear(GL_DEPTH_BUFFER_BIT);
	
	axisObject->Render(cameraObject->GetViewMatrix(), cameraObject->GetProjectionMatrix(windowWidth, windowHeight));
	textureVolumeObject->Render(cameraObject->GetViewMatrix(), cameraObject->GetProjectionMatrix(windowWidth, windowHeight));
	rayVolumeObject->Render(cameraObject->GetViewMatrix(), cameraObject->GetProjectionMatrix(windowWidth, windowHeight));
	
	PrintGLErrors();
}

void RenderViewport::mousePressEvent(QMouseEvent *event)
{
	cameraControl->mousePressEvent(event);
}

void RenderViewport::mouseReleaseEvent(QMouseEvent *event)
{
	cameraControl->mouseReleaseEvent(event);
}

void RenderViewport::mouseMoveEvent(QMouseEvent *event)
{
	cameraControl->mouseMoveEvent(event); 
}

void RenderViewport::wheelEvent(QWheelEvent *event)
{
	cameraControl->wheelEvent(event); 
}

void RenderViewport::keyPressEvent(QKeyEvent *event)
{
	cameraControl->keyPressEvent(event);
}

void RenderViewport::keyReleaseEvent(QKeyEvent *event)
{
	cameraControl->keyReleaseEvent(event); 
}

void RenderViewport::EnableDisableAxis(bool en)
{
	axisObject->SetVisible(en);
}

void RenderViewport::ImportDicomFile(QString fileName)
{
	
}

void RenderViewport::ImportDicomFileSequence(QStringList fileNames)
{
	std::vector<std::string> files;
	for(int i = 0; i < fileNames.size(); i++)
		files.push_back(fileNames.at(i).toStdString());
	Image3D image3D;
	bool loadGood = Image3DFromDicomFileSequence(&image3D, files);
	if(!loadGood)
		return; 
	textureVolume->Allocate(image3D.Width(), image3D.Height(), image3D.Depth());
	textureVolume->LoadData(image3D.Data());
	
	update();
}

void RenderViewport::ImportTIFFFileSequence(QStringList fileNames)
{
	std::vector<std::string> files;
	for(int i = 0; i < fileNames.size(); i++)
		files.push_back(fileNames.at(i).toStdString());
	
	Image3D image3D;
	bool loadGood = Image3DFromTIFFFileSequence(&image3D, files);
	if(!loadGood)
		return;
	
	textureVolume->Allocate(image3D.Width(), image3D.Height(), image3D.Depth(), false);
	textureVolume->LoadData(image3D.Data());
	
	
	Image3D gradient3D;
	gradient3D.Allocate(image3D.Width(), image3D.Height(), image3D.Depth(), 3);
	//image3D.Smooth(); 
	image3D.Median();
	
	gradient3D.Gradient(image3D);
	
	textureGradient->Allocate(gradient3D.Width(), gradient3D.Height(), gradient3D.Depth(), false, 3, 0.5);
	textureGradient->LoadData(gradient3D.Data());
	
	update();
}

void RenderViewport::LoadLUT(float* buffer, int sizeLUT)
{
	if(textureLUT->Width() != sizeLUT)
		textureLUT->Allocate(sizeLUT);
	textureLUT->LoadData(buffer);
	
	update();
}

void RenderViewport::ChooseRenderer(RENDER_TYPE rt)
{
	renderType = rt; 
	if(renderType == SLICE_RENDER)
	{
		textureVolumeObject->SetVisible(true); 
		rayVolumeObject->SetVisible(false); 
	}
	else if(renderType == RAY_RENDER)
	{
		textureVolumeObject->SetVisible(false); 
		rayVolumeObject->SetVisible(true); 
	}
	update();
}

void RenderViewport::SetGradientThreshold(float threshold)
{
	rayVolumeObject->SetGradientThreshold(threshold);
	update();
}

void RenderViewport::SetBackFaceCulling(bool cull)
{
	rayVolumeObject->SetBackFaceCulling(cull);
	update();
}