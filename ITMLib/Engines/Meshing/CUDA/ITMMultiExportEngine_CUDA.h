//
// Created by Quentin Gautier on 2018/5/12
//

#pragma once

#include "../Interface/ITMMultiExportEngine.h"

namespace ITMLib
{
	template<class TVoxel, class TIndex>
		class ITMMultiExportEngine_CUDA : public ITMMultiExportEngine <TVoxel, TIndex>
	{
		public:

			ITMMultiExportEngine_CUDA(ITMVoxelMapGraphManager<TVoxel, TIndex>* sceneManager = 0, ITMTrackingState* tracking = 0):
				ITMMultiExportEngine<TVoxel, TIndex>(sceneManager, tracking) {}

			virtual ~ITMMultiExportEngine_CUDA(){}

			virtual bool ExportTSDFToPcd(const char* filename){}


		private:

			void getVoxelsFromPtrList(int noBlocks,
					const TVoxel* voxelBlocks_device,
					int* blockPtr_host,
					TVoxel* voxels_host);

			bool ExportTSDFToPcd_hashIndex(ITMVoxelMapGraphManager<TVoxel, ITMVoxelBlockHash> sceneManager, const char *filename);
	};
}
