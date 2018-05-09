//
// Created by qkgautier on 8/5/16.
//

#pragma once

#include "../../../ITMLibDefines.h"
#include "../../../Objects/Scene/ITMScene.h"
#include "../../../Objects/Tracking/ITMTrackingState.h"

#include <fstream>

namespace ITMLib
{
	template<class TVoxel, class TIndex>
		class ITMExportEngine
		{
			public:
				ITMExportEngine(ITMScene<TVoxel, TIndex>* scene = 0, ITMTrackingState* tracking = 0):
					scene_(scene),
					trackingState_(tracking)
			{}
				virtual ~ITMExportEngine(){}

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
				ITMScene<TVoxel, TIndex>* scene_;
				ITMTrackingState* trackingState_;
		};
}
