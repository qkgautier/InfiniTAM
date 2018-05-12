// Created by Quentin Gautier on 2018/5/11

#pragma once

#include "CPU/ITMExportEngine_CPU.h"
#ifndef COMPILE_WITHOUT_CUDA
#include "CUDA/ITMExportEngine_CUDA.h"
#endif

namespace ITMLib
{

	/**
	 * \brief This struct provides functions that can be used to construct export engines.
	 */
	struct ITMExportEngineFactory
	{
		//#################### PUBLIC STATIC MEMBER FUNCTIONS ####################

		/**
		 * \brief Makes an export engine.
		 *
		 * \param deviceType  The device on which the export engine should operate.
		 */
		template <typename TVoxel, typename TIndex>
		static ITMExportEngine<TVoxel, TIndex> *MakeExportEngine(DeviceType deviceType, ITMScene<TVoxel, TIndex>* scene = 0, ITMTrackingState* tracking = 0)
		{
			ITMExportEngine<TVoxel, TIndex> *exportEngine = NULL;

			switch (deviceType)
			{
			case DEVICE_CPU:
				exportEngine = new ITMExportEngine_CPU<TVoxel, TIndex>(scene, tracking);
				break;
			case DEVICE_CUDA:
#ifndef COMPILE_WITHOUT_CUDA
				exportEngine = new ITMExportEngine_CUDA<TVoxel, TIndex>(scene, tracking);
#endif
				break;
			case DEVICE_METAL:
#ifdef COMPILE_WITH_METAL
				exportEngine = new ITMExportEngine_CPU<TVoxel, TIndex>(scene, tracking);
#endif
				break;
			}

			return exportEngine;
		}
	};
}
