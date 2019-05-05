// Copyright 2014-2017 Oxford University Innovation Limited and the authors of InfiniTAM

#include <cstdlib>
#include <iostream>
#include <chrono>
#include <sstream>

#include <ros/ros.h>

#include "UIEngine.h"
#include "../InfiniTAM_cli/CLIEngine.h"

#include "../../InputSource/ROSEngine.h"
#include "../../InputSource/OpenNIEngine.h"
#include "../../InputSource/Kinect2Engine.h"
#include "../../InputSource/LibUVCEngine.h"
#include "../../InputSource/PicoFlexxEngine.h"
#include "../../InputSource/RealSenseEngine.h"
#include "../../InputSource/LibUVCEngine.h"
#include "../../InputSource/RealSense2Engine.h"
#include "../../InputSource/FFMPEGReader.h"
#include "../../ITMLib/ITMLibDefines.h"
#include "../../ITMLib/Core/ITMBasicEngine.h"
#include "../../ITMLib/Core/ITMBasicSurfelEngine.h"
#include "../../ITMLib/Core/ITMMultiEngine.h"

using namespace InfiniTAM::Engine;
using namespace InputSource;
using namespace ITMLib;

/** Create a default source of depth images from a list of command line
    arguments. Typically, @para arg1 would identify the calibration file to
    use, @para arg2 the colour images, @para arg3 the depth images and
    @para arg4 the IMU images. If images are omitted, some live sources will
    be tried.
*/
static void CreateDefaultImageSource(ImageSourceEngine* & imageSource, IMUSourceEngine* & imuSource, const char *arg1, const char *arg2, const char *arg3, const char *arg4)
{
	const char *calibFile = arg1;
	const char *filename1 = arg2;
	const char *filename2 = arg3;
	const char *filename_imu = arg4;

	if (strcmp(calibFile, "viewer") == 0)
	{
		imageSource = new BlankImageGenerator("", Vector2i(640, 480));
		printf("starting in viewer mode: make sure to press n first to initiliase the views ... \n");
		return;
	}

	printf("using calibration file: %s\n", calibFile);

	if ((imageSource == NULL) && (filename2 != NULL))
	{
		printf("using rgb images: %s\nusing depth images: %s\n", filename1, filename2);
		if (filename_imu == NULL)
		{
			ImageMaskPathGenerator pathGenerator(filename1, filename2);
			imageSource = new ImageFileReader<ImageMaskPathGenerator>(calibFile, pathGenerator);
		}
		else
		{
			printf("using imu data: %s\n", filename_imu);
			imageSource = new RawFileReader(calibFile, filename1, filename2, Vector2i(320, 240), 0.5f);
			imuSource = new IMUSourceEngine(filename_imu);
		}

		if (imageSource->getDepthImageSize().x == 0)
		{
			delete imageSource;
			if (imuSource != NULL) delete imuSource;
			imuSource = NULL;
			imageSource = NULL;
		}
	}

	if ((imageSource == NULL) && (filename1 != NULL) && (filename_imu == NULL))
	{
		imageSource = new InputSource::FFMPEGReader(calibFile, filename1, filename2);
		if (imageSource->getDepthImageSize().x == 0)
		{
			delete imageSource;
			imageSource = NULL;
		}
	}

	if (imageSource == NULL)
	{
		printf("trying ROS input: /camera/depth/image_raw, /camera/rgb/image_color \n");
		imageSource = new ROSEngine(calibFile);
		if (imageSource->getDepthImageSize().x == 0)
		{
			delete imageSource;
			imageSource = NULL;
		}
	}

	if (imageSource == NULL)
	{
		// If no calibration file specified, use the factory default calibration
		bool useInternalCalibration = !calibFile || strlen(calibFile) == 0;

		printf("trying OpenNI device: %s - calibration: %s\n",
				filename1 ? filename1 : "<OpenNI default device>",
				useInternalCalibration ? "internal" : "from file");
		imageSource = new OpenNIEngine(calibFile, filename1, useInternalCalibration);
		if (imageSource->getDepthImageSize().x == 0)
		{
			delete imageSource;
			imageSource = NULL;
		}
	}

	if (imageSource == NULL)
	{
		printf("trying UVC device\n");
		imageSource = new LibUVCEngine(calibFile);
		if (imageSource->getDepthImageSize().x == 0)
		{
			delete imageSource;
			imageSource = NULL;
		}
	}

	if (imageSource == NULL)
	{
		printf("trying RealSense device\n");
		imageSource = new RealSenseEngine(calibFile);
		if (imageSource->getDepthImageSize().x == 0)
		{
			delete imageSource;
			imageSource = NULL;
		}
	}

    if (imageSource == NULL)
    {
        printf("trying RealSense device with SDK 2.X (librealsense2)\n");
        imageSource = new RealSense2Engine(calibFile);
        if (imageSource->getDepthImageSize().x == 0)
        {
            delete imageSource;
            imageSource = NULL;
        }
    }

    if (imageSource == NULL)
	{
		printf("trying MS Kinect 2 device\n");
		imageSource = new Kinect2Engine(calibFile);
		if (imageSource->getDepthImageSize().x == 0)
		{
			delete imageSource;
			imageSource = NULL;
		}
	}

	if (imageSource == NULL)
	{
		printf("trying PMD PicoFlexx device\n");
		imageSource = new PicoFlexxEngine(calibFile);
		if (imageSource->getDepthImageSize().x == 0)
		{
			delete imageSource;
			imageSource = NULL;
		}
	}
}

int main(int argc, char** argv)
try
{
	// Start ROS
	ros::init(argc, argv, "infinitam_ros");
	ros::start();
	
	const char *arg1 = "";
	const char *arg2 = NULL;
	const char *arg3 = NULL;
	const char *arg4 = NULL;


	if(argc > 1){ arg1 = argv[1]; }
	if(argc > 2){ arg2 = argv[2]; }
	if(argc > 3){ arg3 = argv[3]; }
	if(argc > 4){ arg4 = argv[4]; }

	if (argc == 1) {
		printf("usage: %s [<calibfile> [<imagesource>] ]\n"
		       "  <calibfile>   : path to a file containing intrinsic calibration parameters\n"
		       "  <imagesource> : either one argument to specify OpenNI device ID\n"
		       "                  or two arguments specifying rgb and depth file masks\n"
		       "\n"
		       "examples:\n"
		       "  %s ./Files/Teddy/calib.txt ./Files/Teddy/Frames/%%04i.ppm ./Files/Teddy/Frames/%%04i.pgm\n"
		       "  %s ./Files/Teddy/calib.txt\n\n", argv[0], argv[0], argv[0]);
	}

	printf("initialising ...\n");
	ImageSourceEngine *imageSource = NULL;
	IMUSourceEngine *imuSource = NULL;

	CreateDefaultImageSource(imageSource, imuSource, arg1, arg2, arg3, arg4);
	if (imageSource==NULL)
	{
		std::cout << "failed to open any image stream" << std::endl;
		return -1;
	}

	ITMLibSettings *internalSettings = new ITMLibSettings();
	internalSettings->sceneParams.viewFrustum_max = 5.f; // default is 3
	internalSettings->sceneParams.voxelSize = 0.01f;
	internalSettings->sceneParams.mu = 0.03f;
	internalSettings->createMeshingEngine = false;
	internalSettings->libMode = ITMLibSettings::LIBMODE_BASIC;
	internalSettings->deviceType = DEVICE_CUDA;


	ITMMainEngine *mainEngine = NULL;
	switch (internalSettings->libMode)
	{
	case ITMLibSettings::LIBMODE_BASIC:
		mainEngine = new ITMBasicEngine<ITMVoxel, ITMVoxelIndex>(internalSettings, imageSource->getCalib(), imageSource->getRGBImageSize(), imageSource->getDepthImageSize());
		break;
	case ITMLibSettings::LIBMODE_BASIC_SURFELS:
		mainEngine = new ITMBasicSurfelEngine<ITMSurfelT>(internalSettings, imageSource->getCalib(), imageSource->getRGBImageSize(), imageSource->getDepthImageSize());
		break;
	case ITMLibSettings::LIBMODE_LOOPCLOSURE:
		mainEngine = new ITMMultiEngine<ITMVoxel, ITMVoxelIndex>(internalSettings, imageSource->getCalib(), imageSource->getRGBImageSize(), imageSource->getDepthImageSize());
		break;
	default: 
		throw std::runtime_error("Unsupported library mode!");
		break;
	}


	// Note: requires c++11
	std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
	std::time_t current_time = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;
	std::stringstream ss2;
	std::string ss_str;
	std::string ss2_str;

	std::string extension = ".pcd";
	if (internalSettings->libMode == ITMLibSettings::LIBMODE_LOOPCLOSURE){ extension = ""; }


	if (true)
	{
		UIEngine::Instance()->Initialise(argc, argv, imageSource, imuSource, mainEngine, "./Files/Out", internalSettings->deviceType);
		std::cout << "Starting InfiniTAM..." << std::endl;
		UIEngine::Instance()->Run();

		char mbstr[100];
		std::strftime(mbstr, sizeof(mbstr), "%Y_%m_%d_%H_%M_%S", std::localtime(&current_time));
		ss << "world_" << mbstr; // << extension;
		ss_str = ss.str();
		ss2 << "poses_" << mbstr << ".txt";
		ss2_str = ss2.str();
		printf("Saving TSDF to \"%s%s\" ...\n", ss_str.c_str(), extension.c_str()); fflush(stdout);
		mainEngine->SaveTSDFToFile(ss_str.c_str());
		if (internalSettings->libMode == ITMLibSettings::LIBMODE_BASIC)
		{
			printf("Saving poses to \"%s\" ...\n", ss2_str.c_str()); fflush(stdout);
			mainEngine->SavePosesToFile(ss2_str.c_str());
		}

		std::cout << "Stopping InfiniTAM..." << std::endl;
		UIEngine::Instance()->Shutdown();
		std::cout << "InfiniTAM stopped" << std::endl;
	}
	else 
	{
		CLIEngine::Instance()->Initialise(imageSource, imuSource, mainEngine, internalSettings->deviceType);
		std::cout << "Starting InfiniTAM..." << std::endl;
		CLIEngine::Instance()->Run();

		char mbstr[100];
		std::strftime(mbstr, sizeof(mbstr), "%Y_%m_%d_%H_%M_%S", std::localtime(&current_time));
		ss << "world_" << mbstr; // << extension;
		ss_str = ss.str();
		ss2 << "poses_" << mbstr << ".txt";
		ss2_str = ss2.str();
		printf("Saving TSDF to \"%s%s\" ...\n", ss_str.c_str(), extension.c_str()); fflush(stdout);
		mainEngine->SaveTSDFToFile(ss_str.c_str());
		if (internalSettings->libMode == ITMLibSettings::LIBMODE_BASIC)
		{
			printf("Saving poses to \"%s\" ...\n", ss2_str.c_str()); fflush(stdout);
			mainEngine->SavePosesToFile(ss2_str.c_str());
		}

		std::cout << "Stopping InfiniTAM..." << std::endl;
		CLIEngine::Instance()->Shutdown();
		std::cout << "InfiniTAM stopped" << std::endl;
	}

	delete mainEngine;
	delete internalSettings;
	delete imageSource;
	if (imuSource != NULL) delete imuSource;

	ros::shutdown();
	return 0;
}
catch(std::exception& e)
{
	std::cerr << e.what() << '\n';
	return EXIT_FAILURE;
}

