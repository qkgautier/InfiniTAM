//
// Created by qkgautier on 8/5/16.
//

#pragma once

#include "../Interface/ITMExportEngine.h"

namespace ITMLib
{
	template<class TVoxel, class TIndex>
		class ITMExportEngine_CUDA : public ITMExportEngine <TVoxel, TIndex>
	{
		public:

			ITMExportEngine_CUDA(ITMScene<TVoxel, TIndex>* scene = 0, ITMTrackingState* tracking = 0):
				ITMExportEngine<TVoxel, TIndex>(scene, tracking) {}

			virtual ~ITMExportEngine_CUDA(){}

			virtual bool ExportTSDFToPcd(const char* basename);


		private:
			
			bool ExportTSDFToPcd_hashIndex(ITMScene<TVoxel,ITMVoxelBlockHash>* scene, const char *filename);
	};
}
