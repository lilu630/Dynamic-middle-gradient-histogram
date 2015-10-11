#include "StdAfx.h"
#include "LH_Histogram.h"
#include "string.h"


LH_Histogram::LH_Histogram(void)
{
}


LH_Histogram::~LH_Histogram(void)
{
	if(raw_data)
		delete []raw_data;
	if(data_LHM)
		delete []data_LHM;
	if(data_matitude)
		delete []data_matitude;
	if(data_histogram)
		delete []data_histogram;
	if(newclassify_data)
		delete []newclassify_data;
}

LH_Histogram::LH_Histogram(CString pathname, int width_in, int height_in, int depth_in)
{
	Read_Data((LPSTR)(LPCTSTR)pathname);
	width = width_in;
	depth = depth_in;
	height = height_in;
	D = 1;

	data_matitude = new magnititude[(height-2*D)*(width-2*D)*(depth-2*D)];

	data_LHM = new LHandMag[(width-2*D)*(height-2*D)*(depth-2*D)];

	local_magnitude = new float[(height-2*D)*(width-2*D)*(depth-2*D)];
	local_raw_data = new int[(height-2*D)*(width-2*D)*(depth-2*D)];
	memset(local_magnitude, 0, sizeof(float)*(height-2*D)*(width-2*D)*(depth-2*D));
	memset(local_raw_data, 0, sizeof(int)*(height-2*D)*(width-2*D)*(depth-2*D));
	hist_ranges= new point3D[7];
	memset(hist_ranges, 0, sizeof(point3D)*7);

	for(int i = 0; i < (width-2*D)*(height-2*D)*(depth-2*D); i++)
	{
		data_LHM[i].H = 0;
		data_LHM[i].L = 0;
		data_LHM[i].magnititude_value = 0;
	}

	histogram_width = 256;
	histogram_height = (int)(histogram_width*sqrt(3.0f))+1;
	data_histogram = new int[histogram_width*histogram_height];

	newclassify_data = new char[(width-2*D)*(height-2*D)*(depth-2*D)];
	for(int i = 0; i < (width-2*D)*(height-2*D)*(depth-2*D); i++)
	{
		newclassify_data[i] = 0;
	}
	CalcMatitude();
	ChangeRawData();
	Get_LHhistogram();
}
bool LH_Histogram::Read_Data(char* filename)
{
	FILE *fp;
	if((fp = fopen(filename, "r")) == NULL)
	{
		return false;
	}
	int length;
	fseek(fp, 0, SEEK_END);
	length = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *raw = new char[length];
	if(fread_s(raw, length, 1, length, fp) == NULL)
	{
		return false;
	}
	raw_data = new int[length+1];
	for(int i = 0; i < length+1; i++)
	{
		raw_data[i] = (raw[i]&0x00ff);
	}
	delete(raw);
	fclose(fp);
	return true;
}

void LH_Histogram::CalcMatitude()
{ 
	for(int i = 0; i < depth-2*D; i++)
	{
		for(int j = 0; j < height-2*D; j++)
		{
			for(int k = 0; k < width-2*D; k++)
			{
				float dx = raw_data[k+2*D + (width)*(j+D) + (height)*(width)*(i+D)] - raw_data[k + (width)*(j+D) + (height)*(width)*(i+D)];
				float dy = raw_data[k+D + (width)*(j+2*D) + (height)*(width)*(i+D)] - raw_data[k+D + (width)*(j) + (height)*(width)*(i+D)];
				float dz = raw_data[k+D + (width)*(j+D) + (height)*(width)*(i+2*D)] - raw_data[k+D + (width)*(j+D) + (height)*(width)*(i)];
				float temp_gradient = dx*dx +dy*dy +dz*dz;
				data_matitude[k + (width-2*D)*j + (width-2*D)*(height-2*D)*i].magnititude_value = sqrt((float)temp_gradient);
				if(temp_gradient != 0)
				{
					data_matitude[k + (width-2*D)*j + (width-2*D)*(height-2*D)*i].direction_x = dx/sqrt((float)temp_gradient);
					data_matitude[k + (width-2*D)*j + (width-2*D)*(height-2*D)*i].direction_y = dy/sqrt((float)temp_gradient);
					data_matitude[k + (width-2*D)*j + (width-2*D)*(height-2*D)*i].direction_z = dz/sqrt((float)temp_gradient);
				}
				else
				{
					data_matitude[k + (width-2*D)*j + (width-2*D)*(height-2*D)*i].direction_x = 0;
					data_matitude[k + (width-2*D)*j + (width-2*D)*(height-2*D)*i].direction_y = 0;
					data_matitude[k + (width-2*D)*j + (width-2*D)*(height-2*D)*i].direction_z = 0;
				}
				data_LHM[k + (width-2*D)*j + (width-2*D)*(height-2*D)*i].magnititude_value = data_matitude[k + (width-2*D)*j + (width-2*D)*(height-2*D)*i].magnititude_value;
			}
		}
	}
}

void LH_Histogram::ChangeRawData()
{
	int *temp_data = new int[(width-2*D)*(height-2*D)*(depth-2*D)];
	for(int i = 0; i < depth-2*D; i++)
	{
		for(int j = 0; j < height-2*D; j++)
		{
			for(int k = 0; k < width-2*D; k++)
			{
				temp_data[k + (width-2*D)*j + (width-2*D)*(height-2*D)*i] = raw_data[k+D + width*(j+D) + (width)*(height)*(i+D)];
			}
		}
	}
	delete(raw_data);
	raw_data = temp_data;
}
magnititude LH_Histogram::GetBinearValue(magnititude* data_matitude, float x, float y, float z)
{
	int x_1 = x;
	int y_1 = y;
	int z_1 = z;
	int x_2 = x_1 + 1;
	int y_2 = y_1 + 1;
	int z_2 = z_1 + 1;
	if(x_2 >= (width-2*D)-1)
		x_2 = x_1;
	if(y_2 >= (height -2*D)-1)
		y_2 = y_1;
	if(z_2 >= (depth -2*D)-1)
		z_2 = z_1;
	float a = x - x_1;
	float b = y - y_1;
	float c = z - z_1;
	magnititude temp_magnititude;
	temp_magnititude.magnititude_value = \
		(1-a)*(1-b)*(1-c)*data_matitude[x_1 + y_1*(width-2*D) + z_1*(width-2*D)*(height -2*D)].magnititude_value + \
		a*(1-b)*(1-c)*data_matitude[x_2 + y_1*(width-2*D) + z_1*(width-2*D)*(height -2*D)].magnititude_value + \
		(1-a)*(b)*(1-c)*data_matitude[x_1 + y_2*(width-2*D) + z_1*(width-2*D)*(height -2*D)].magnititude_value + \
		(a)*(b)*(1-c)*data_matitude[x_2 + y_2*(width-2*D) + z_1*(width-2*D)*(height -2*D)].magnititude_value +\
		(1-a)*(1-b)*(c)*data_matitude[x_1 + y_1*(width-2*D) + z_2*(width-2*D)*(height -2*D)].magnititude_value + \
		a*(1-b)*(c)*data_matitude[x_2 + y_1*(width-2*D) + z_2*(width-2*D)*(height -2*D)].magnititude_value + \
		(1-a)*(b)*(c)*data_matitude[x_1 + y_2*(width-2*D) + z_2*(width-2*D)*(height -2*D)].magnititude_value + \
		(a)*(b)*(c)*data_matitude[x_2 + y_2*(width-2*D) + z_2*(width-2*D)*(height -2*D)].magnititude_value;
	if(temp_magnititude.magnititude_value == 0)
	{
		temp_magnititude.direction_x = 0;
		temp_magnititude.direction_y = 0;
		temp_magnititude.direction_z = 0;
	}
	else
	{
		temp_magnititude.direction_x = \
			(1-a)*(1-b)*(1-c)*data_matitude[x_1 + y_1*(width-2*D) + z_1*(width-2*D)*(height -2*D)].direction_x + \
			a*(1-b)*(1-c)*data_matitude[x_2 + y_1*(width-2*D) + z_1*(width-2*D)*(height -2*D)].direction_x + \
			(1-a)*(b)*(1-c)*data_matitude[x_1 + y_2*(width-2*D) + z_1*(width-2*D)*(height -2*D)].direction_x + \
			(a)*(b)*(1-c)*data_matitude[x_2 + y_2*(width-2*D) + z_1*(width-2*D)*(height -2*D)].direction_x +\
			(1-a)*(1-b)*(c)*data_matitude[x_1 + y_1*(width-2*D) + z_2*(width-2*D)*(height -2*D)].direction_x + \
			a*(1-b)*(c)*data_matitude[x_2 + y_1*(width-2*D) + z_2*(width-2*D)*(height -2*D)].direction_x + \
			(1-a)*(b)*(c)*data_matitude[x_1 + y_2*(width-2*D) + z_2*(width-2*D)*(height -2*D)].direction_x + \
			(a)*(b)*(c)*data_matitude[x_2 + y_2*(width-2*D) + z_2*(width-2*D)*(height -2*D)].direction_x;
		temp_magnititude.direction_y = \
			(1-a)*(1-b)*(1-c)*data_matitude[x_1 + y_1*(width-2*D) + z_1*(width-2*D)*(height -2*D)].direction_y + \
			a*(1-b)*(1-c)*data_matitude[x_2 + y_1*(width-2*D) + z_1*(width-2*D)*(height -2*D)].direction_y + \
			(1-a)*(b)*(1-c)*data_matitude[x_1 + y_2*(width-2*D) + z_1*(width-2*D)*(height -2*D)].direction_y + \
			(a)*(b)*(1-c)*data_matitude[x_2 + y_2*(width-2*D) + z_1*(width-2*D)*(height -2*D)].direction_y +\
			(1-a)*(1-b)*(c)*data_matitude[x_1 + y_1*(width-2*D) + z_2*(width-2*D)*(height -2*D)].direction_y + \
			a*(1-b)*(c)*data_matitude[x_2 + y_1*(width-2*D) + z_2*(width-2*D)*(height -2*D)].direction_y + \
			(1-a)*(b)*(c)*data_matitude[x_1 + y_2*(width-2*D) + z_2*(width-2*D)*(height -2*D)].direction_y + \
			(a)*(b)*(c)*data_matitude[x_2 + y_2*(width-2*D) + z_2*(width-2*D)*(height -2*D)].direction_y;
		temp_magnititude.direction_z = \
			(1-a)*(1-b)*(1-c)*data_matitude[x_1 + y_1*(width-2*D) + z_1*(width-2*D)*(height -2*D)].direction_z + \
			a*(1-b)*(1-c)*data_matitude[x_2 + y_1*(width-2*D) + z_1*(width-2*D)*(height -2*D)].direction_z + \
			(1-a)*(b)*(1-c)*data_matitude[x_1 + y_2*(width-2*D) + z_1*(width-2*D)*(height -2*D)].direction_z + \
			(a)*(b)*(1-c)*data_matitude[x_2 + y_2*(width-2*D) + z_1*(width-2*D)*(height -2*D)].direction_z +\
			(1-a)*(1-b)*(c)*data_matitude[x_1 + y_1*(width-2*D) + z_2*(width-2*D)*(height -2*D)].direction_z + \
			a*(1-b)*(c)*data_matitude[x_2 + y_1*(width-2*D) + z_2*(width-2*D)*(height -2*D)].direction_z + \
			(1-a)*(b)*(c)*data_matitude[x_1 + y_2*(width-2*D) + z_2*(width-2*D)*(height -2*D)].direction_z + \
			(a)*(b)*(c)*data_matitude[x_2 + y_2*(width-2*D) + z_2*(width-2*D)*(height -2*D)].direction_z;
		float normaliezed = 1.0/(temp_magnititude.direction_z*temp_magnititude.direction_z +\
			temp_magnititude.direction_y*temp_magnititude.direction_y +\
			temp_magnititude.direction_x*temp_magnititude.direction_x);
		temp_magnititude.direction_x = normaliezed*temp_magnititude.direction_x;
		temp_magnititude.direction_y = normaliezed*temp_magnititude.direction_y;
		temp_magnititude.direction_z = normaliezed*temp_magnititude.direction_z;
	}
	return temp_magnititude;
}

float LH_Histogram::GetBinearValue(float x, float y, float z)
{
	int x_1 = x;
	int y_1 = y;
	int z_1 = z;
	int x_2 = x_1 + 1;
	int y_2 = y_1 + 1;
	int z_2 = z_1 + 1;
	if(x_2 >= (width-2*D)-1)
		x_2 = x_1;
	if(y_2 >= (height -2*D)-1)
		y_2 = y_1;
	if(z_2 >= (depth -2*D)-1)
		z_2 = z_1;
	float a = x - x_1;
	float b = y - y_1;
	float c = z - z_1;
	return (1-a)*(1-b)*(1-c)*raw_data[x_1 + y_1*(width-2*D) + z_1*(width-2*D)*(height -2*D)] + \
		a*(1-b)*(1-c)*raw_data[x_2 + y_1*(width-2*D) + z_1*(width-2*D)*(height -2*D)] + \
		(1-a)*(b)*(1-c)*raw_data[x_1 + y_2*(width-2*D) + z_1*(width-2*D)*(height -2*D)] + \
		(a)*(b)*(1-c)*raw_data[x_2 + y_2*(width-2*D) + z_1*(width-2*D)*(height -2*D)] +\
		(1-a)*(1-b)*(c)*raw_data[x_1 + y_1*(width-2*D) + z_2*(width-2*D)*(height -2*D)] + \
		a*(1-b)*(c)*raw_data[x_2 + y_1*(width-2*D) + z_2*(width-2*D)*(height -2*D)] + \
		(1-a)*(b)*(c)*raw_data[x_1 + y_2*(width-2*D) + z_2*(width-2*D)*(height -2*D)] + \
		(a)*(b)*(c)*raw_data[x_2 + y_2*(width-2*D) + z_2*(width-2*D)*(height -2*D)];
}
float LH_Histogram::FindL(magnititude* data_matitude, int* image_data, float step, int x, int y, int z, point3D& l_point)
{
	float positon_x = x;
	float positon_y = y;
	float positon_z = z;
	magnititude temp_magnititude = data_matitude[x + (width-2*D)*y + (width-2*D)*(height-2*D)*z];
	int iterate_counter = 0;
	float pre_data = GetBinearValue(positon_x, positon_y, positon_z);
	while(temp_magnititude.magnititude_value >= 1)
	{
		positon_x -= temp_magnititude.direction_x*step;
		positon_y -= temp_magnititude.direction_y*step;
		positon_z -= temp_magnititude.direction_z*step;
		if(positon_x >= 0 && positon_x <= ((width-2*D)-1) && positon_y >= 0 && positon_y <= ((height-2*D)-1) && positon_z >= 0 && positon_z <= ((depth-2*D)-1))
		{
			temp_magnititude = GetBinearValue(data_matitude, positon_x, positon_y, positon_z);
			if(pre_data < GetBinearValue(positon_x, positon_y, positon_z))
			{
				l_point.x = positon_x;
				l_point.y = positon_y;
				l_point.z = positon_z;
				return pre_data;
			}
			else
				pre_data = GetBinearValue(positon_x, positon_y, positon_z);
		}
		else
			return -1;
		iterate_counter += 1;
		if(iterate_counter > 15)
		{
			step *= 2;
			iterate_counter = 0;
		}
	}
	//printf("%d\t", GetBinearValue(image_data,positon_x, positon_y));
	l_point.x = positon_x;
	l_point.y = positon_y;
	l_point.z = positon_z;
	return GetBinearValue(positon_x, positon_y, positon_z);
}

float LH_Histogram::FindH(magnititude* data_matitude, int* image_data, float step, int x, int y, int z, point3D& h_point)
{
	float positon_x = x;
	float positon_y = y;
	float positon_z = z;
	magnititude temp_magnititude = data_matitude[x + (width-2*D)*y + (width-2*D)*(height-2*D)*z];
	int iterate_counter = 0;
	float pre_data = GetBinearValue(positon_x, positon_y, positon_z);
	while(temp_magnititude.magnititude_value >= 1)
	{
		positon_x += temp_magnititude.direction_x*step;
		positon_y += temp_magnititude.direction_y*step;
		positon_z += temp_magnititude.direction_z*step;
		if(positon_x >= 0 && positon_x <= ((width-2*D)-1) && positon_y >= 0 && positon_y <= ((height-2*D)-1) && positon_z >= 0 && positon_z <= ((depth-2*D)-1))
		{
			temp_magnititude = GetBinearValue(data_matitude, positon_x, positon_y, positon_z);
			if(pre_data > GetBinearValue(positon_x, positon_y, positon_z))
			{
				h_point.x = positon_x;
				h_point.y = positon_y;
				h_point.z = positon_z;
				return pre_data;
			}
			else
				pre_data = GetBinearValue(positon_x, positon_y, positon_z);
		}
		else
			return -1;
		iterate_counter += 1;
		if(iterate_counter > 15)
		{
			step *= 2;
			iterate_counter = 0;
		}
	}
	//printf("%d\t", GetBinearValue(image_data,positon_x, positon_y));
	h_point.x = positon_x;
	h_point.y = positon_y;
	h_point.z = positon_z;
	return GetBinearValue(positon_x, positon_y, positon_z);
}

void LH_Histogram::PrintLPath(magnititude* data_matitude, int* image_data, float step, int x, int y, int z)
{
	float positon_x = x;
	float positon_y = y;
	float positon_z = z;
	magnititude temp_magnititude = data_matitude[x + (width-2*D)*y + (width-2*D)*(height-2*D)*z];
	int iterate_counter = 0;
	float pre_data = GetBinearValue(positon_x, positon_y, positon_z);
	while(temp_magnititude.magnititude_value >= 2)
	{
		positon_x -= temp_magnititude.direction_x*step;
		positon_y -= temp_magnititude.direction_y*step;
		positon_z -= temp_magnititude.direction_z*step;
		if(positon_x >= 0 && positon_x <= ((width-2*D)-1) && positon_y >= 0 && positon_y <= ((height-2*D)-1) && positon_z >= 0 && positon_z <= ((depth-2*D)-1))
		{
			temp_magnititude = GetBinearValue(data_matitude, positon_x, positon_y, positon_z);
			if(pre_data < GetBinearValue(positon_x, positon_y, positon_z))
				return ;
			else
				pre_data = GetBinearValue(positon_x, positon_y, positon_z);
		}
		else
			return ;
		//printf("PrintLPath:%f,%f,%f,%f\n", GetBinearValue(positon_x, positon_y, positon_z), positon_x, positon_y, positon_z);
		iterate_counter += 1;
		if(iterate_counter > 15)
		{
			step *= 2;
			iterate_counter = 0;
		}
	}
	//printf("\n");
	//printf("%d\t", GetBinearValue(image_data,positon_x, positon_y));
}

void LH_Histogram::PrintHPath(magnititude* data_matitude, int* image_data, float step, int x, int y, int z)
{
	float positon_x = x;
	float positon_y = y;
	float positon_z = z; 
	int iterate_counter = 0;
	float pre_data = GetBinearValue(positon_x, positon_y, positon_z);
	magnititude temp_magnititude = data_matitude[x + (width-2*D)*y + (width-2*D)*(height-2*D)*z];
	while(temp_magnititude.magnititude_value >= 2)
	{
		positon_x += temp_magnititude.direction_x*step;
		positon_y += temp_magnititude.direction_y*step;
		positon_z += temp_magnititude.direction_z*step;
		if(positon_x >= 0 && positon_x <= ((width-2*D)-1) && positon_y >= 0 && positon_y <= ((height-2*D)-1) && positon_z >= 0 && positon_z <= ((depth-2*D)-1))
		{
			if(pre_data > GetBinearValue(positon_x, positon_y, positon_z))
				return ;
			else
				pre_data = GetBinearValue(positon_x, positon_y, positon_z);
			temp_magnititude = GetBinearValue(data_matitude, positon_x, positon_y, positon_z);
		}
		else
			return ;
		//printf("PrintLPath:%f,%f,%f,%f\n", GetBinearValue(positon_x, positon_y, positon_z), positon_x, positon_y, positon_z);
		iterate_counter += 1;
		if(iterate_counter > 15)
		{
			step *= 2;
			iterate_counter = 0;
		}
	}
}

void LH_Histogram::Get_LHhistogram()
{
	float step = 1;

	for(int k = 0; k < (depth -2*D); k++)
	{
		for(int i = 0; i < (height-2*D); i++)
		{
			for(int j = 0; j < (width-2*D); j++)
			{
				if(data_matitude[j + i*(width-2*D) + k*(width-2*D)*(height-2*D)].magnititude_value >= 2)
				{
					point3D l_point, h_point;
					int L = FindL(data_matitude, raw_data, step, j, i, k, l_point);
					int H = FindH(data_matitude, raw_data, step, j, i, k, h_point);
					if(H < L && L != -1 && H != -1)
					{
						//printf("x=%d,y=%d,z=%d\n", j, i, k);
						//PrintLPath(data_matitude, raw_data, step, j, i, k);
						//PrintHPath(data_matitude, raw_data, step, j, i, k);
					}
					if(L != -1 && H != -1 && H > L)
					{
						float distance_l = l_point.Distance(j, i, k);
						float distance_h = h_point.Distance(j, i, k);
						//float percent_h = distance_h/data_matitude[j + i*(width-2*D) + k*(width-2*D)*(height-2*D)].magnititude_value;
						//float percent_l = distance_l/data_matitude[j + i*(width-2*D) + k*(width-2*D)*(height-2*D)].magnititude_value
						if(distance_l < 10 && distance_h < 10)
						{
							data_LHM[j + i*(width-2*D) + k*(width-2*D)*(height-2*D)].H = H;
							data_LHM[j + i*(width-2*D) + k*(width-2*D)*(height-2*D)].L = L;
						}
					}
				}
			}
		}
	}
}

void LH_Histogram::GetNewHistgram(int th, int data_type)
{

	for(int i = 0; i < histogram_height; i++)
		for(int j = 0; j < histogram_width; j++)
			data_histogram[j + histogram_width*i] = 0;

	for(int i = 0; i < (width-2*D)*(height-2*D)*(depth-2*D); i++)
	{
		if(data_LHM[i].magnititude_value> 5 && newclassify_data[i] == 0)
		{
			if(checkRange(i, data_type))
			{
				int intensity,matitude;
				if((data_LHM[i].H - data_LHM[i].L) > th)
				{
					intensity = (data_LHM[i].H + data_LHM[i].L)/2;
					matitude = data_matitude[i].magnititude_value;
					if(intensity != 0)
						data_histogram[intensity + matitude*256] += 1;
				}
			}
		}
	}
	for(int i = 0; i < histogram_height*histogram_width; i++)
	{
		if(data_histogram[i] < 100)
		{
			data_histogram[i] = 0;	
		}
	}

	FILE *fp;
	fp = fopen("data_histogram.dat", "w");
	for(int i = 0; i < histogram_height; i++)
	{
		for(int j = 0; j < histogram_width; j++)
		{
			fprintf(fp, "%d\t", data_histogram[j + i*histogram_width]);
		}
		fprintf(fp, "\n");
	}
	for(int i = 0; i < histogram_width*histogram_height; i++)
	{
		data_histogram[i] += 1;
		data_histogram[i] = log((float)data_histogram[i]);
	}
	int data_max = 0;
	for(int i = 0; i < histogram_width*histogram_height; i++)
	{
		if(data_histogram[i] > data_max)
			data_max = data_histogram[i];
	}

	for(int i = 0; i < histogram_width*histogram_height; i++)
	{
		data_histogram[i] = (int)(data_histogram[i]*256.0f/(float)data_max);
	}
}

bool LH_Histogram::checkRange(int index, int data_type)
{
	int EM_value = (data_LHM[index].H + data_LHM[index].L)/2;
	for(int i = 1; i <= data_type; i++)
	{
		if(EM_value >= hist_ranges[i].x && EM_value <= hist_ranges[i].y)
			return false;
	}
	return true;
}

void LH_Histogram::ShowNewImage(CDC* p_DC, CRect rect)
{
	CvvImage m_image;
	CvSize image_size;
	image_size.height = histogram_height;
	image_size.width = histogram_width;
	IplImage* out_image = cvCreateImage(image_size,
		IPL_DEPTH_8U,
		3);
	for(int i = 0; i < histogram_height; i++)
		for(int j = 0; j < histogram_width; j++)
		{
			out_image->imageData[j*3 + (histogram_height-1-i)*out_image->widthStep] = (char)data_histogram[j + i*histogram_width];
			out_image->imageData[j*3 + 1 + (histogram_height-1-i)*out_image->widthStep] = (char)data_histogram[j + i*histogram_width];
			out_image->imageData[j*3 + 2 + (histogram_height-1-i)*out_image->widthStep] = (char)data_histogram[j + i*histogram_width];
		}
	for(int i = 8; i < histogram_width; i=i+8)
	{
		CvPoint begin_point, end_point;
		begin_point.x = i;
		begin_point.y = histogram_height-10;
		end_point.x = i;
		end_point.y = histogram_height-1;
		cvLine(out_image, begin_point, end_point, CV_RGB(255, 0, 0), 0.5);
	}
	m_image.CopyOf(out_image, 1);
	m_image.DrawToHDC(p_DC->GetSafeHdc(),rect);
	cvReleaseImage(&out_image);
}

char* LH_Histogram::NewClassify(int data_L, int data_H, float mag_L, float mag_H, int type, float* mag, bool is_linear)
{
	memset(local_magnitude, 0, sizeof(float)*(height-2*D)*(width-2*D)*(depth-2*D));
	memset(local_raw_data, 0, sizeof(int)*(height-2*D)*(width-2*D)*(depth-2*D));
	hist_ranges[type].x = data_L*0.8;
	hist_ranges[type].y = data_H*1.1;

	float temp_magnititude = 0;
	int count_times = 0;
	for(int i = 0; i < (width-2*D)*(height-2*D)*(depth-2*D); i++)
	{
		int add_data = (data_LHM[i].H+data_LHM[i].L)/2;
		if(add_data > data_L && add_data < data_H)
		{
			temp_magnititude += data_matitude[i].magnititude_value;
			count_times++;
		}
	}
	float mean_magnitude = temp_magnititude/count_times;

	for(int i = 0; i < (width-2*D)*(height-2*D)*(depth-2*D); i++)
	{
		int add_data = (data_LHM[i].H+data_LHM[i].L)/2;	
		if(add_data > data_L && add_data < data_H && newclassify_data[i] == 0 && data_LHM[i].magnititude_value > mean_magnitude)
		{
			newclassify_data[i] = type;
			local_magnitude[i] = data_matitude[i].magnititude_value;
			local_raw_data[i] = raw_data[i];
		}
	}
	RemoveIosdata(type);
	ChangeOpacity(type, mag, is_linear);
	return newclassify_data;
}

void LH_Histogram::CheckNeibour(char* temp, int x, int y, int z, int substanceType, float mag_L_post, float mag_H_post)
{
	float step = 1;
	for(int i = -1; i <= 1; i++)
	{
		for(int j = -1; j <= 1; j++)
		{
			int k;
			for(k = -1; k <= 1; k++)
			{
				int L=-1, H=-1;
				if(x > 0 && y > 0 && z > 0 && x <(width-2*D)-2 && y < (height-2*D)-2 && z <(depth-2*D)-2)
				{
					if(data_LHM[x+k + (width-2*D)*(y+j) + (width-2*D)*(height-2*D)*(z+i)].magnititude_value >= mag_L_post &&
						data_LHM[x+k + (width-2*D)*(y+j) + (width-2*D)*(height-2*D)*(z+i)].magnititude_value <= mag_H_post &&
						newclassify_data[x+k + (width-2*D)*(y+j) + (width-2*D)*(height-2*D)*(z+i)] == 0)
						temp[x+k + (width-2*D)*(y+j) + (width-2*D)*(height-2*D)*(z+i)] = substanceType;
				}	
			}
		}
	}
}

char* LH_Histogram::PostProcess(int type, float mag_L_post, float mag_H_post, float* mag, bool is_linear)
{
	queue<point3D> local_LH_points;
	for(int k = 0; k < (depth -2*D); k++)
	{
		for(int i = 0; i < (height-2*D); i++)
		{
			for(int j = 0; j < (width-2*D); j++)
			{
				if(newclassify_data[j + i*(width-2*D) + (width-2*D)*(height-2*D)*k] == type)
				{
					local_LH_points.push(point3D(j, i, k));
				}
			}
		}
	}
	//local_LH_points.push(point3D(0, 0, 0));
	int temp = 0;
	while(!local_LH_points.empty())
	{
		/*point3D p = local_LH_points.front();
		if(p.x ==0 && p.y == 0 && p.z == 0)
		{
		local_LH_points.pop();
		local_LH_points.push(p);
		temp += 1;
		if(temp > mag_L_post)
		break;
		}
		else*/
			CheckLH(local_LH_points, type);
	}
	 
	ChangeOpacity(type, mag, is_linear);
	return newclassify_data;
}

void LH_Histogram::CheckLH(queue<point3D>& border_LH_points, int segment_type)
{
	point3D p;
	p = border_LH_points.front();
	border_LH_points.pop();

	for(int i = p.z-1; i <= p.z+1; i++)
	{
		for(int j = p.y-1; j <= p.y+1; j++)
		{
			for(int k = p.x-1; k <= p.x+1; k++)
			{
				if(i >= 0 && j >= 0 && k >= 0 && i < (depth-2*D) && j < (height-2*D) && k <(width-2*D))
				{
					int index = k + j*(width-2*D) + i*(width-2*D)*(height-2*D);
					int index_value = p.x + p.y*(width-2*D) + p.z*(width-2*D)*(height-2*D);

					if(newclassify_data[index] == 0)
					{
						point3D temp(p.x, p.y, p.z);
						temp.point3D_minius(point3D(k, j, i));
						if(temp.IsSurface(data_matitude[index_value].direction_x, data_matitude[index_value].direction_y, data_matitude[index_value].direction_z))
						{
							bool temp2 = abs((static_cast<float>(local_raw_data[index_value])-static_cast<float>(raw_data[index])))/static_cast<float>(local_raw_data[index_value]) < 0.05;
							bool temp3 = data_matitude[index].magnititude_value > 0.8*local_magnitude[index_value] && data_matitude[index].magnititude_value < 1.25*local_magnitude[index_value];
							if(temp2 && temp3)
							{
								local_magnitude[index] = local_magnitude[index_value];
								local_raw_data[index] = local_raw_data[index_value];

								newclassify_data[index] = segment_type;
								border_LH_points.push(point3D(k, j, i));
							}
						}
					}
				}
			}
		}
	}
}

char* LH_Histogram::Retry(int type, float* mag)
{

	for(int i = 0; i < (width-2*D)*(height-2*D)*(depth-2*D); i++)
	{
		if(newclassify_data[i] == type)
		{
			mag[i] = 0;
			newclassify_data[i] = 0;
		}
	}
	return newclassify_data;
}

void LH_Histogram::ChangeOpacity(int type, float* mag, bool linear)
{
	if(linear)
	{
		for(int i = 0; i < (width-2*D)*(height-2*D)*(depth-2*D); i++)
		{
			if(newclassify_data[i] == type)
			{
				mag[i] = 0.1;
			}
		}
	}
	else
	{
		float max = 0;
		for(int i = 0; i < (width-2*D)*(height-2*D)*(depth-2*D); i++)
		{
			if(data_LHM[i].magnititude_value > max && newclassify_data[i] == type)
			{
				max = data_LHM[i].magnititude_value;
			}
		}
		for(int i = 0; i < (width-2*D)*(height-2*D)*(depth-2*D); i++)
		{
			if(newclassify_data[i] == type)
			{
				float x = data_LHM[i].magnititude_value/max;
				mag[i] = x;//1 - sqrt(1 - x*x);
			}
		}
	}
}

void LH_Histogram::RemoveIosdata(int type)
{
	char* temp = new char[(width-2*D)*(height-2*D)*(depth-2*D)];
	for(int i = 0; i < (width-2*D)*(height-2*D)*(depth-2*D); i++)
	{
		temp[i] = newclassify_data[i];
	}
	for(int k = 0; k < (depth -2*D); k++)
	{
		for(int i = 0; i < (height-2*D); i++)
		{
			for(int j = 0; j < (width-2*D); j++)
			{
				int count = 0;
				if(newclassify_data[j + i*(width-2*D) + (width-2*D)*(height-2*D)*k] == type)
				{
					count = CheckIosdata(temp, j, i, k, type);
					if(count < 10)
					{
						local_magnitude[i] = 0;
						local_raw_data[i] = 0;
						temp[j + i*(width-2*D) + (width-2*D)*(height-2*D)*k] = 0;
					}
				}
			}
		}
	}
	for(int i = 0; i < (width-2*D)*(height-2*D)*(depth-2*D); i++)
	{
		newclassify_data[i] = temp[i];
	}
	delete [] temp;
}

int LH_Histogram::CheckIosdata(char* temp, int x, int y, int z, int type)
{
	int count = 0;
	for(int i = -1; i <= 1; i++)
	{
		for(int j = -1; j <= 1; j++)
		{
			for(int k = -1; k <= 1; k++)
			{
				if(x > 0 && y > 0 && z > 0 && x <(width-2*D)-2 && y < (height-2*D)-2 && z <(depth-2*D)-2)
				{
					if(temp[x+k + (width-2*D)*(y+j) + (width-2*D)*(height-2*D)*(z+i)] == type)
					{
						count += 1;
					}
				}	
			}
		}
	}
	return count;
}

void LH_Histogram::Mergedata(int type1, int type2, float* mag, bool is_linear)
{
	for(int i = 0; i < (width-2*D)*(height-2*D)*(depth-2*D); i++)
	{
		if(newclassify_data[i] == type2)
			newclassify_data[i] = type1;
	}
	ChangeOpacity(type2, mag, is_linear);
}