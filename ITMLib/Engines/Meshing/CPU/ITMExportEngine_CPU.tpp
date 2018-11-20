
#include "ITMExportEngine_CPU.h"

#include "../Shared/ITMExportEngine_Shared.h"

#include <fstream>
#include <vector>


using namespace ITMLib;



namespace ITMLib
{
	template<class TVoxel, class TIndex>
		bool ITMExportEngine_CPU<TVoxel, TIndex>::ExportTSDFToPcd_hashIndex(ITMScene<TVoxel, ITMVoxelBlockHash>* scene, const char *basename)
		{
			if(!scene){ return false; }

			ITMGlobalCache<TVoxel>* cache = scene->globalCache;

			bool hasGlobalCache = (cache != NULL);


			TVoxel* voxelBlocks_global = 0;
			TVoxel* voxelBlocks_local = scene->localVBA.GetVoxelBlocks();

			if(hasGlobalCache){ voxelBlocks_global = cache->GetStoredVoxelBlock(0); }

			ITMHashEntry* hash = scene->index.GetEntries();

			int noEntries = SDF_BUCKET_NUM + SDF_EXCESS_LIST_SIZE;

			ITMHashSwapState swapStateDefault;
			swapStateDefault.state = 2;
			std::vector<ITMHashSwapState> swapStates(noEntries, swapStateDefault);

			if(hasGlobalCache)
			{
				memcpy(swapStates.data(), cache->GetSwapStates(false), noEntries*sizeof(ITMHashSwapState));
			}


			int noPoints = 0;
			for(int i = 0; i < noEntries; i++)
			{
				if(hash[i].ptr >= -1 && swapStates[i].state != 2)
				{
					noPoints += countVoxelsInBlock<TVoxel, TIndex>(voxelBlocks_global, i);
				}
				else if(hash[i].ptr >= 0 && swapStates[i].state == 2)
				{
					noPoints += countVoxelsInBlock<TVoxel, TIndex>(voxelBlocks_local, hash[i].ptr);
				}
			}


			if(noPoints <= 0){ return false; }

			std::string filename = std::string(basename) + ".pcd";

			std::ofstream file(filename.c_str());
			if(!file.is_open()){ return false; }

			file << "VERSION 0.7\n";
			file << "FIELDS x y z " << (TVoxel::hasColorInformation? "rgb": "") << " intensity\n";
			file << "SIZE 4 4 4 " << (TVoxel::hasColorInformation? "4": "") << " 4\n";
			file << "TYPE F F F " << (TVoxel::hasColorInformation? "F": "") << " F\n";
			file << "COUNT 1 1 1 " << (TVoxel::hasColorInformation? "1": "") << " 1\n";
			file << "WIDTH " << noPoints << "\n";
			file << "HEIGHT 1\n";
			file << "VIEWPOINT 0 0 0 1 0 0 0\n";
			file << "POINTS " << noPoints << "\n";
			file << "DATA binary\n";


			for(int i = 0; i < noEntries; i++)
			{
				if(hash[i].ptr >= -1 && swapStates[i].state != 2)
				{
					exportVoxelsInBlock<TVoxel, TIndex>(file, voxelBlocks_global, i, hash[i].pos);
				}
				else if(hash[i].ptr >= 0 && swapStates[i].state == 2)
				{
					exportVoxelsInBlock<TVoxel, TIndex>(file, voxelBlocks_local, hash[i].ptr, hash[i].pos);
				}
			}


			return true;
		}



	template<class TVoxel, class TIndex>
		bool ITMExportEngine_CPU<TVoxel, TIndex>::ExportTSDFToPcd(const char *filename)
		{ return false; }


	template <>
		bool ITMExportEngine_CPU<ITMVoxel_s_rgb, ITMVoxelBlockHash>::ExportTSDFToPcd(const char *filename)
		{
			return ExportTSDFToPcd_hashIndex(scene_, filename);
		}

	template <>
		bool ITMExportEngine_CPU<ITMVoxel_s, ITMVoxelBlockHash>::ExportTSDFToPcd(const char *filename)
		{
			return ExportTSDFToPcd_hashIndex(scene_, filename);
		}

	template <>
		bool ITMExportEngine_CPU<ITMVoxel_f, ITMVoxelBlockHash>::ExportTSDFToPcd(const char *filename)
		{
			return ExportTSDFToPcd_hashIndex(scene_, filename);
		}

	template <>
		bool ITMExportEngine_CPU<ITMVoxel_f_rgb, ITMVoxelBlockHash>::ExportTSDFToPcd(const char *filename)
		{
			return ExportTSDFToPcd_hashIndex(scene_, filename);
		}


} // namespace
