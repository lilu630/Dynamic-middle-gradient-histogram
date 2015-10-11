#pragma once

#include "gl\glew.h"
#include "gl\gl.h"
#include "gl\glu.h"

#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
#include <vector_types.h>
#include <vector_functions.h>
#include <driver_functions.h>
#include <helper_cuda.h>
#include <helper_cuda_gl.h>
// Helper functions
#include <helper_cuda.h>
#include <helper_functions.h>
#include <helper_timer.h>

typedef unsigned int uint;

extern "C" void setTextureFilterMode(bool bLinearFilter);
extern "C" void initCuda(void *h_volume, cudaExtent volumeSize);
extern "C" void freeCudaBuffers();
extern "C" void render_kernel(dim3 gridSize, dim3 blockSize, uint *d_output, uint imageW, uint imageH, cudaExtent volumeSize,
	float density, float brightness, float transferOffset, float transferScale);
extern "C" void copyInvViewMatrix(float *invViewMatrix, size_t sizeofMatrix);

class Show_CUDA_OpenGL
{
public:
	Show_CUDA_OpenGL(void);
	~Show_CUDA_OpenGL(void);
	Show_CUDA_OpenGL(CDC* Show_DC, cudaExtent volumeSize, float4* h_volume, CRect rect);
	void InitOpenGL();
	void InitOpenGLandCUDA();
	void Display();
	void Render();
	void Reshape(int w, int h);
	void EventResponse(UINT char_event);
	void Init();
	void ChangeData(float4* volume);
private:
	CDC* p_DC;
	cudaExtent volumeSize;
	float4* h_volume;
	BOOL bSetupPixelFormat();
	int iDivUp(int a, int b);
	GLuint pbo;     // OpenGL pixel buffer object
	GLuint tex;     // OpenGL texture object
	struct cudaGraphicsResource *cuda_pbo_resource; // CUDA Graphics Resource (to transfer PBO)

	int width, height;
	dim3 blockSize;
	dim3 gridSize;

	float3 viewRotation;
	float3 viewTranslation;
	float invViewMatrix[12];

	float density;
	float brightness ;
	float transferOffset;
	float transferScale;
};

