//
// Created by Quentin Gautier on 2018/5/12
//

#pragma once

#include "../Interface/ITMMultiExportEngine.h"

namespace ITMLib
{
	template<class TVoxel, class TIndex>
		class ITMMultiExportEngine_CPU : public ITMMultiExportEngine <TVoxel, TIndex>
	{
		public:

			ITMMultiExportEngine_CPU(ITMVoxelMapGraphManager<TVoxel, TIndex>* sceneManager = 0, ITMTrackingState* tracking = 0):
				ITMMultiExportEngine<TVoxel, TIndex>(sceneManager, tracking) {}

			virtual ~ITMMultiExportEngine_CPU(){}

			virtual bool ExportTSDFToPcd(const char* filename);

		public:
			typedef typename ITMMultiIndex<ITMVoxelBlockHash>::IndexData MultiIndexData;
			typedef ITMMultiVoxel<TVoxel> MultiVoxelData;
			typedef ITMVoxelMapGraphManager<TVoxel, ITMVoxelBlockHash> MultiSceneManager;

		private:

			bool ExportTSDFToPcd_hashIndex(ITMVoxelMapGraphManager<TVoxel, ITMVoxelBlockHash>* sceneManager, const char *filename);
	};
}
