// Copyright 2014 Isis Innovation Limited and the authors of InfiniTAM

#pragma once

#include "../../Utils/ITMLibDefines.h"
#include "ITMPixelUtils.h"

_CPU_AND_GPU_CODE_ inline float getColorDifferenceSq(DEVICEPTR(Vector4f) *locations, DEVICEPTR(Vector4f) *colours, DEVICEPTR(Vector4u) *rgb,
	const CONSTPTR(Vector2i) & imgSize, int locId_global, Vector4f projParams, Matrix4f M)
{
	Vector4f pt_model, pt_camera, colour_known, colour_obs;
	Vector3f colour_diff;
	Vector2f pt_image;

	pt_model = locations[locId_global];
	colour_known = colours[locId_global];

	pt_camera = M * pt_model;

	if (pt_camera.z <= 0) return -1.0f;

	pt_image.x = projParams.x * pt_camera.x / pt_camera.z + projParams.z;
	pt_image.y = projParams.y * pt_camera.y / pt_camera.z + projParams.w;

	if (pt_image.x < 0 || pt_image.x > imgSize.x - 1 || pt_image.y < 0 || pt_image.y > imgSize.y - 1) return -1.0f;

	colour_obs = interpolateBilinear(rgb, pt_image, imgSize);
	if (colour_obs.w < 254.0f) return -1.0f;

	colour_diff.x = colour_obs.x - 255.0f * colour_known.x;
	colour_diff.y = colour_obs.y - 255.0f * colour_known.y;
	colour_diff.z = colour_obs.z - 255.0f * colour_known.z;
	
	return colour_diff.x * colour_diff.x + colour_diff.y * colour_diff.y + colour_diff.z * colour_diff.z;
}

_CPU_AND_GPU_CODE_ inline bool computePerPointGH_rt_Color(THREADPTR(float) *localGradient, THREADPTR(float) *localHessian,
	DEVICEPTR(Vector4f) *locations, DEVICEPTR(Vector4f) *colours, DEVICEPTR(Vector4u) *rgb, const CONSTPTR(Vector2i) & imgSize,
    int locId_global, Vector4f projParams, Matrix4f M, DEVICEPTR(Vector4s) *gx, DEVICEPTR(Vector4s) *gy, int numPara, int startPara)
{
	Vector4f pt_model, pt_camera, colour_known, colour_obs, gx_obs, gy_obs;
	Vector3f colour_diff_d, d_pt_cam_dpi, d[6];
	Vector2f pt_image, d_proj_dpi;

	pt_model = locations[locId_global];
	colour_known = colours[locId_global];

	pt_camera = M * pt_model;

	if (pt_camera.z <= 0) return false;

	pt_image.x = projParams.x * pt_camera.x / pt_camera.z + projParams.z;
	pt_image.y = projParams.y * pt_camera.y / pt_camera.z + projParams.w;

	if (pt_image.x < 0 || pt_image.x > imgSize.x - 1 || pt_image.y < 0 || pt_image.y > imgSize.y - 1) return false;

	colour_obs = interpolateBilinear(rgb, pt_image, imgSize);
	gx_obs = interpolateBilinear(gx, pt_image, imgSize);
	gy_obs = interpolateBilinear(gy, pt_image, imgSize);

	if (colour_obs.w < 254.0f) return false;

	colour_diff_d.x = 2.0f * (colour_obs.x - 255.0f * colour_known.x);
	colour_diff_d.y = 2.0f * (colour_obs.y - 255.0f * colour_known.y);
	colour_diff_d.z = 2.0f * (colour_obs.z - 255.0f * colour_known.z);

	for (int para = 0, counter = 0; para < numPara; para++)
	{
		switch (para + startPara)
		{
		case 0: d_pt_cam_dpi.x = pt_camera.w;  d_pt_cam_dpi.y = 0.0f;         d_pt_cam_dpi.z = 0.0f;         break;
		case 1: d_pt_cam_dpi.x = 0.0f;         d_pt_cam_dpi.y = pt_camera.w;  d_pt_cam_dpi.z = 0.0f;         break;
		case 2: d_pt_cam_dpi.x = 0.0f;         d_pt_cam_dpi.y = 0.0f;         d_pt_cam_dpi.z = pt_camera.w;  break;
		case 3: d_pt_cam_dpi.x = 0.0f;         d_pt_cam_dpi.y = -pt_camera.z;  d_pt_cam_dpi.z = pt_camera.y;  break;
		case 4: d_pt_cam_dpi.x = pt_camera.z;  d_pt_cam_dpi.y = 0.0f;         d_pt_cam_dpi.z = -pt_camera.x;  break;
		default:
		case 5: d_pt_cam_dpi.x = -pt_camera.y;  d_pt_cam_dpi.y = pt_camera.x;  d_pt_cam_dpi.z = 0.0f;         break;
		};

		d_proj_dpi.x = projParams.x * ((pt_camera.z * d_pt_cam_dpi.x - d_pt_cam_dpi.z * pt_camera.x) / (pt_camera.z * pt_camera.z));
		d_proj_dpi.y = projParams.y * ((pt_camera.z * d_pt_cam_dpi.y - d_pt_cam_dpi.z * pt_camera.y) / (pt_camera.z * pt_camera.z));

		d[para].x = d_proj_dpi.x * gx_obs.x + d_proj_dpi.y * gy_obs.x;
		d[para].y = d_proj_dpi.x * gx_obs.y + d_proj_dpi.y * gy_obs.y;
		d[para].z = d_proj_dpi.x * gx_obs.z + d_proj_dpi.y * gy_obs.z;

		localGradient[para] = d[para].x * colour_diff_d.x + d[para].y * colour_diff_d.y + d[para].z * colour_diff_d.z;

		for (int col = 0; col <= para; col++)
			localHessian[counter++] = 2.0f * (d[para].x * d[col].x + d[para].y * d[col].y + d[para].z * d[col].z);
	}

	return true;
}
