#pragma once
#include "cv.h"
#include "highgui.h"
#include "CvvImage.h"
#include "Structures.h"
#include <queue>
using namespace std;

typedef struct LHandMag
{
	int L;
	int H;
	float magnititude_value;
}LHandMag;


class LH_Histogram
{
public:
	LH_Histogram(void);
	~LH_Histogram(void);
	LH_Histogram(CString pathname, int width_in, int height_in, int depth_in);
	void GetNewHistgram(int th, int data_type);
	void ShowNewImage(CDC* p_DC, CRect rect);
	char* NewClassify(int data_L, int data_H, float mag_L, float mag_H, int type, float* mag, bool is_linear);
	char* PostProcess(int type, float mag_L_post, float mag_H_post, float* mag, bool is_linear);
	char* Retry(int type, float* mag);
	void Mergedata(int type1, int type2, float* mag, bool is_linear);
	void ChangeOpacity(int type, float* mag, bool linear);
	magnititude* data_matitude;
	char* newclassify_data;
private:
	bool Read_Data(char* filename);
	void CalcMatitude();
	void ChangeRawData();
	float GetBinearValue(float x, float y, float z);
	magnititude GetBinearValue(magnititude* data_matitude, float x, float y, float z);
	float FindL(magnititude* data_matitude, int* image_data, float step, int x, int y, int z, point3D& l_point);
	float FindH(magnititude* data_matitude, int* image_data, float step, int x, int y, int z, point3D& h_point);
	void PrintLPath(magnititude* data_matitude, int* image_data, float step, int x, int y, int z);
	void PrintHPath(magnititude* data_matitude, int* image_data, float step, int x, int y, int z);
	void Get_LHhistogram();
	void CheckNeibour(char* temp, int x, int y, int z, int substanceType, float mag_L_post, float mag_H_post);
	void RemoveIosdata(int type);
	int CheckIosdata(char* temp, int x, int y, int z, int type);
	void CheckLH(queue<point3D>&border_LH_points, int segment_type);
	bool checkRange(int index, int data_type);
	int width ;
	int height;
	int depth;
	int  D;
	int* raw_data;
	LHandMag* data_LHM;

	float*local_magnitude;
	int* local_raw_data;

	int histogram_width, histogram_height;
	int* data_histogram;
	point3D* hist_ranges;
};

