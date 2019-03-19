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

				virtual bool ExportTSDFToPcd(const char* basename) = 0;

				virtual bool ExportPoses(const char* filename)
				{
					if(!trackingState_){ return false; }
					std::ofstream file(filename);
					if(!file.is_open()){ return false; }

					for(size_t i = 0; i < trackingState_->poses.size(); i++)
					{
						ORUtils::Matrix4<float>& m = trackingState_->poses[i];
						ORUtils::Matrix4<float> mInv;
						m.inv(mInv);
						for (int r = 0; r < 3; r++)
							for (int c = 0 ; c < 4; c++)
							{
								file << mInv(c,r);
								if (c != 3 || r != 2){ file << " "; }
							}
						file << "\n";
					}
					return true;
				}

			protected:
				ITMScene<TVoxel, TIndex>* scene_;
				ITMTrackingState* trackingState_;
		};
}
