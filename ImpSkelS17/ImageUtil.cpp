#include "impressionist.h"
#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "paintview.h"
#include "ImpBrush.h"
#include "MathUtil.hpp"
#include <math.h>

void apply_filter(const unsigned char* src, unsigned char* dst, 
	int image_width, int image_height,
	const float* filter, int filter_rows, int filter_cols) 
{
	for (int j = 0; j < image_height; j++) {
		for (int i = 0; i < image_width; i++) {
			float sum[3] = { 0.0f };
			for (int fy = 0; fy < filter_rows; fy++) {
				for (int fx = 0; fx < filter_cols; fx++) {
					int org_x = i + fx - 1;
					int org_y = j + fy - 1;
					org_x = cap_range(org_x, 0, image_width);
					org_y = cap_range(org_y, 0, image_height);

					const unsigned char* rgb = &src[(org_y*image_width + org_x)*3];
					sum[0] += rgb[0] * filter[fy*filter_cols + fx];
					sum[1] += rgb[1] * filter[fy*filter_cols + fx];
					sum[2] += rgb[2] * filter[fy*filter_cols + fx];
				}
			}
			for (int k = 0; k < 3; k++) {
				if (sum[k] < 0) sum[k] = 0; else if (sum[k] > 255) sum[k] = 255;
				dst[(j*image_width + i) * 3 + k] = sum[k];
			}
		}
	}

}
