#include "StdAfx.h"
#include "Show_CUDA_OpenGL.h"

#pragma comment(lib,"glew64.lib")

int Show_CUDA_OpenGL::iDivUp(int a, int b)
{
	return (a % b != 0) ? (a / b + 1) : (a / b);
}
Show_CUDA_OpenGL::Show_CUDA_OpenGL(CDC* Show_DC, cudaExtent volumeSize_in, float4* h_volume_in, CRect rect)
{
	p_DC = Show_DC;
	volumeSize = volumeSize_in;
	h_volume = h_volume_in;
	pbo = 0;
	tex = 0;
	width = rect.Width();
	height = rect.Height();
	density = 1.0f;
	brightness = 1.0f;
	transferOffset = 0.0f;
	transferScale = 1.0f;
	blockSize = dim3(16, 16);
	gridSize = dim3(iDivUp(width, blockSize.x), iDivUp(height, blockSize.y));
	viewTranslation = make_float3(0.0, 0.0, -4.0f);
	viewRotation = make_float3(0, 0, 0);
}

void Show_CUDA_OpenGL::InitOpenGL()
{
	PIXELFORMATDESCRIPTOR pfd;
	int n;
	HGLRC hrc;

	ASSERT(p_DC != NULL);

	if(!bSetupPixelFormat())
		return;

	n=::GetPixelFormat(p_DC->GetSafeHdc());

	::DescribePixelFormat(p_DC->GetSafeHdc(), n,sizeof(pfd),&pfd);

	hrc=wglCreateContext(p_DC->GetSafeHdc());
	wglMakeCurrent(p_DC->GetSafeHdc(),hrc);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);

	glewInit();

	if (!glewIsSupported("GL_VERSION_2_0 GL_ARB_pixel_buffer_object"))
	{
		printf("Required OpenGL extensions missing.");
		exit(EXIT_SUCCESS);
	}
}

BOOL Show_CUDA_OpenGL::bSetupPixelFormat()
{
	static PIXELFORMATDESCRIPTOR pfd = 
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL|
		PFD_DOUBLEBUFFER, 
		PFD_TYPE_RGBA,
		24,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		32,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};
	int pixelformat;

	if((pixelformat = ChoosePixelFormat(p_DC->GetSafeHdc(), &pfd)) == 0)
	{
		return FALSE;
	}

	if(SetPixelFormat(p_DC->GetSafeHdc(), pixelformat, &pfd) == FALSE)
	{
		return FALSE;
	}
	return TRUE;
}
void Show_CUDA_OpenGL::InitOpenGLandCUDA()
{
	if (pbo)
	{
		// unregister this buffer object from CUDA C
		checkCudaErrors(cudaGraphicsUnregisterResource(cuda_pbo_resource));

		// delete old buffer
		glDeleteBuffersARB(1, &pbo);
		glDeleteTextures(1, &tex);
	}

	// create pixel buffer object for display
	glGenBuffersARB(1, &pbo);
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pbo);
	glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, width*height*sizeof(GLubyte)*4, 0, GL_STREAM_DRAW_ARB);
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

	// register this buffer object with CUDA
	checkCudaErrors(cudaGraphicsGLRegisterBuffer(&cuda_pbo_resource, pbo, cudaGraphicsMapFlagsWriteDiscard));

	// create texture for display
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Show_CUDA_OpenGL::Display()
{
	// use OpenGL to build view matrix
	GLfloat modelView[16];
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRotatef(-viewRotation.x, 1.0, 0.0, 0.0);
	glRotatef(-viewRotation.y, 0.0, 1.0, 0.0);
	glRotatef(-viewRotation.z, 0.0, 0.0, 1.0);
	glTranslatef(-viewTranslation.x, -viewTranslation.y, -viewTranslation.z);
	glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
	glPopMatrix();

	invViewMatrix[0] = modelView[0];
	invViewMatrix[1] = modelView[4];
	invViewMatrix[2] = modelView[8];
	invViewMatrix[3] = modelView[12];
	invViewMatrix[4] = modelView[1];
	invViewMatrix[5] = modelView[5];
	invViewMatrix[6] = modelView[9];
	invViewMatrix[7] = modelView[13];
	invViewMatrix[8] = modelView[2];
	invViewMatrix[9] = modelView[6];
	invViewMatrix[10] = modelView[10];
	invViewMatrix[11] = modelView[14];

	Render();

	// display results
	glClear(GL_COLOR_BUFFER_BIT);

	// draw image from PBO
	glDisable(GL_DEPTH_TEST);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
#if 0
	// draw using glDrawPixels (slower)
	glRasterPos2i(0, 0);
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pbo);
	glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
#else
	// draw using texture

	// copy from pbo to texture
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, pbo);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

	// draw textured quad
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(0, 0);
	glTexCoord2f(1, 0);
	glVertex2f(1, 0);
	glTexCoord2f(1, 1);
	glVertex2f(1, 1);
	glTexCoord2f(0, 1);
	glVertex2f(0, 1);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
#endif

	glFinish();
	SwapBuffers(wglGetCurrentDC());
}

void Show_CUDA_OpenGL::Render()
{
	copyInvViewMatrix(invViewMatrix, sizeof(float4)*3);

	// map PBO to get CUDA device pointer
	uint *d_output;
	// map PBO to get CUDA device pointer
	checkCudaErrors(cudaGraphicsMapResources(1, &cuda_pbo_resource, 0));
	size_t num_bytes;
	checkCudaErrors(cudaGraphicsResourceGetMappedPointer((void **)&d_output, &num_bytes,
		cuda_pbo_resource));
	//printf("CUDA mapped PBO: May access %ld bytes\n", num_bytes);

	// clear image
	checkCudaErrors(cudaMemset(d_output, 0, width*height*4));

	// call CUDA kernel, writing results to PBO
	render_kernel(gridSize, blockSize, d_output, width, height, volumeSize, density, brightness, transferOffset, transferScale);


	getLastCudaError("kernel failed");

	checkCudaErrors(cudaGraphicsUnmapResources(1, &cuda_pbo_resource, 0));
}

void Show_CUDA_OpenGL::Reshape(int w, int h)
{
	width = w;
	height = h;
	InitOpenGLandCUDA();

	// calculate new grid size
	gridSize = dim3(iDivUp(width, blockSize.x), iDivUp(height, blockSize.y));

	glViewport(0, 0, w, h);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, 0.0, 1.0);
}

void Show_CUDA_OpenGL::EventResponse(UINT char_event)
{
	if(char_event == VK_NUMPAD0 || char_event == VK_NUMPAD1 || char_event == VK_NUMPAD2 || char_event == VK_NUMPAD3 || char_event == VK_NUMPAD4 || char_event == VK_NUMPAD5 || char_event == VK_NUMPAD6 || char_event == VK_NUMPAD7 || char_event == VK_LEFT || char_event == VK_RIGHT || char_event == VK_UP || char_event == VK_DOWN)
	{
		switch(char_event)
		{
		case VK_NUMPAD0:
			viewRotation.x += 5.0;
			viewRotation.x = (viewRotation.x >= 360.0) ? 0.0 : viewRotation.x;
			break;
		case VK_NUMPAD1:
			viewRotation.x -= 5.0;
			viewRotation.x = (viewRotation.x >= 360.0) ? 0.0 : viewRotation.x;
			break;
		case VK_NUMPAD2:
			viewRotation.y += 5.0;
			viewRotation.y = (viewRotation.y >= 360.0) ? 0.0 : viewRotation.y;
			break;
		case VK_NUMPAD3:
			viewRotation.y -= 5.0;
			viewRotation.y = (viewRotation.y >= 360.0) ? 0.0 : viewRotation.y;
			break;
		case VK_NUMPAD4:
			viewRotation.z += 5.0;
			viewRotation.z = (viewRotation.z >= 360.0) ? 0.0 : viewRotation.z;
			break;
		case VK_NUMPAD5:
			viewRotation.z -= 5.0;
			viewRotation.z = (viewRotation.z >= 360.0) ? 0.0 : viewRotation.z;
			break;
		case VK_NUMPAD6:
			viewTranslation.z += 0.1;
			break;
		case VK_NUMPAD7:
			viewTranslation.z -= 0.1;
			break;
		case VK_LEFT:
			viewTranslation.x -= 0.1;
			break;
		case VK_RIGHT:
			viewTranslation.x += 0.1;
			break;
		case VK_DOWN:
			viewTranslation.y -= 0.1;
			break;
		case VK_UP:
			viewTranslation.y += 0.1; 
			break;
		}
		Display();
	}
}

Show_CUDA_OpenGL::~Show_CUDA_OpenGL(void)
{
	freeCudaBuffers();
	if (pbo)
	{
		cudaGraphicsUnregisterResource(cuda_pbo_resource);
		glDeleteBuffersARB(1, &pbo);
		glDeleteTextures(1, &tex);
	}
}

void Show_CUDA_OpenGL::Init()
{
	InitOpenGL();
	initCuda(h_volume, volumeSize);
	//InitOpenGLandCUDA();
	Reshape(width, height);
	Display();
}
void Show_CUDA_OpenGL::ChangeData(float4* volume)
{
	freeCudaBuffers();
	h_volume = volume;
	initCuda(h_volume, volumeSize);
	Display();
}
