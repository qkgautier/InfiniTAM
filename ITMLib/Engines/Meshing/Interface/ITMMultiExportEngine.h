//
// Created by Quentin Gautier on 2018/5/12
//

#pragma once

#include "../../MultiScene/ITMMapGraphManager.h"
#include "../../../Objects/Tracking/ITMTrackingState.h"

#include <fstream>

namespace ITMLib
{
	template<class TVoxel, class TIndex>
		class ITMMultiExportEngine
		{
			public:
				ITMMultiExportEngine(ITMVoxelMapGraphManager<TVoxel, TIndex>* sceneManager = 0, ITMTrackingState* tracking = 0):
					sceneManager_(sceneManager),
					trackingState_(tracking)
			{}
				virtual ~ITMMultiExportEngine(){}

				virtual bool ExportTSDFToPcd(const char* filename) = 0;

				virtual bool ExportPoses(const char* filename)
				{
					//if(!trackingState_){ return false; }
					//std::ofstream file(filename);
					//if(!file.is_open()){ return false; }

					//for(size_t i = 0; i < trackingState_->poses.size(); i++)
					//{
					//	file << trackingState_->poses[i];
					//}
					return true;
				}

			protected:
				ITMVoxelMapGraphManager<TVoxel, TIndex>* sceneManager_;
				ITMTrackingState* trackingState_;
		};
}
