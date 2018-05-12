// Created by Quentin Gautier on 2018/5/12

#pragma once

#include "CPU/ITMMultiExportEngine_CPU.h"
#ifndef COMPILE_WITHOUT_CUDA
#include "CUDA/ITMMultiExportEngine_CUDA.h"
#endif

namespace ITMLib
{

	/**
	 * \brief This struct provides functions that can be used to construct export engines.
	 */
	struct ITMMultiExportEngineFactory
	{
		//#################### PUBLIC STATIC MEMBER FUNCTIONS ####################

		/**
		 * \brief Makes an export engine.
		 *
		 * \param deviceType  The device on which the export engine should operate.
		 */
		template <typename TVoxel, typename TIndex>
		static ITMMultiExportEngine<TVoxel, TIndex> *MakeExportEngine(DeviceType deviceType, ITMVoxelMapGraphManager<TVoxel, TIndex>* sceneManager = 0, ITMTrackingState* tracking = 0)
		{
			ITMMultiExportEngine<TVoxel, TIndex> *exportEngine = NULL;

			switch (deviceType)
			{
			case DEVICE_CPU:
				exportEngine = new ITMMultiExportEngine_CPU<TVoxel, TIndex>(sceneManager, tracking);
				break;
			case DEVICE_CUDA:
#ifndef COMPILE_WITHOUT_CUDA
				exportEngine = new ITMMultiExportEngine_CUDA<TVoxel, TIndex>(sceneManager, tracking);
#endif
				break;
			case DEVICE_METAL:
#ifdef COMPILE_WITH_METAL
				exportEngine = new ITMMultiExportEngine_CPU<TVoxel, TIndex>(sceneManager, tracking);
#endif
				break;
			}

			return exportEngine;
		}
	};
}
