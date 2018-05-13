
#include "ITMMultiExportEngine_CPU.h"

#include "../Shared/ITMExportEngine_Shared.h"

#include <fstream>
#include <vector>


using namespace ITMLib;



namespace ITMLib
{
	template<class TVoxel, class TIndex>
		bool ITMMultiExportEngine_CPU<TVoxel, TIndex>::ExportTSDFToPcd_hashIndex(ITMVoxelMapGraphManager<TVoxel, ITMVoxelBlockHash>* sceneManager, const char *filename)
		{
			if(!sceneManager){ return false; }

			// Some of this code is copied from ITMMultiMeshingEngine_CPU
			// Probably needs to be cleaned a little
			int numLocalMaps = (int)sceneManager->numLocalMaps();
			if (numLocalMaps > MAX_NUM_LOCALMAPS) numLocalMaps = MAX_NUM_LOCALMAPS;

			MultiIndexData hashTables;
			//MultiVoxelData localVBAs;

			const ITMSceneParams & sceneParams = *(sceneManager->getLocalMap(0)->scene->sceneParams);
			hashTables.numLocalMaps = numLocalMaps;

			std::vector<TVoxel*> voxelBlocks_global_v(numLocalMaps);
			std::vector<TVoxel*> voxelBlocks_local_v(numLocalMaps);
			std::vector< std::vector<ITMHashSwapState> > swapStates_v(numLocalMaps);

			int noEntries = ITMVoxelBlockHash::noTotalEntries;
			int noPointsTotal = 0;
			
			for (int localMapId = 0; localMapId < numLocalMaps; ++localMapId)
			{
				hashTables.poses_vs[localMapId] = sceneManager->getEstimatedGlobalPose(localMapId).GetM();
				//hashTables.poses_vs[localMapId].m30 /= sceneParams.voxelSize;
				//hashTables.poses_vs[localMapId].m31 /= sceneParams.voxelSize;
				//hashTables.poses_vs[localMapId].m32 /= sceneParams.voxelSize;
				
				hashTables.posesInv[localMapId] = sceneManager->getEstimatedGlobalPose(localMapId).GetInvM();
				//hashTables.posesInv[localMapId].m30 /= sceneParams.voxelSize;
				//hashTables.posesInv[localMapId].m31 /= sceneParams.voxelSize;
				//hashTables.posesInv[localMapId].m32 /= sceneParams.voxelSize;

				ITMScene<TVoxel, TIndex>* scene = sceneManager->getLocalMap(localMapId)->scene;



				ITMGlobalCache<TVoxel>* cache = scene->globalCache;

				bool hasGlobalCache = (cache != NULL);


				voxelBlocks_global_v[localMapId] = 0;
				voxelBlocks_local_v[localMapId]  = scene->localVBA.GetVoxelBlocks();

				if(hasGlobalCache){ voxelBlocks_global_v[localMapId] = cache->GetStoredVoxelBlock(0); }

				ITMHashEntry* hash = scene->index.GetEntries();
				hashTables.index[localMapId] = hash;

				ITMHashSwapState swapStateDefault;
				swapStateDefault.state = 2;
				swapStates_v[localMapId].resize(noEntries, swapStateDefault);

				if(hasGlobalCache)
				{
					memcpy(swapStates_v[localMapId].data(), cache->GetSwapStates(false), noEntries*sizeof(ITMHashSwapState));
				}


				int noPoints = 0;
				for(int i = 0; i < noEntries; i++)
				{
					if(hash[i].ptr >= -1 && swapStates_v[localMapId][i].state != 2)
					{
						noPoints += countVoxelsInBlock<TVoxel, TIndex>(voxelBlocks_global_v[localMapId], i);
					}
					else if(hash[i].ptr >= 0 && swapStates_v[localMapId][i].state == 2)
					{
						noPoints += countVoxelsInBlock<TVoxel, TIndex>(voxelBlocks_local_v[localMapId], hash[i].ptr);
					}
				}
				noPointsTotal += noPoints;
			}

			if(noPointsTotal <= 0){ return false; }

			std::ofstream file(filename);
			if(!file.is_open()){ return false; }

			file << "VERSION 0.7\n";
			file << "FIELDS x y z " << (TVoxel::hasColorInformation? "rgb": "") << " intensity\n";
			file << "SIZE 4 4 4 " << (TVoxel::hasColorInformation? "4": "") << " 4\n";
			file << "TYPE F F F " << (TVoxel::hasColorInformation? "F": "") << " F\n";
			file << "COUNT 1 1 1 " << (TVoxel::hasColorInformation? "1": "") << " 1\n";
			file << "WIDTH " << noPointsTotal << "\n";
			file << "HEIGHT 1\n";
			file << "VIEWPOINT 0 0 0 1 0 0 0\n";
			file << "POINTS " << noPointsTotal << "\n";
			file << "DATA binary\n";


			for (int localMapId = 0; localMapId < numLocalMaps; ++localMapId)
			{
				ITMHashEntry* hash = hashTables.index[localMapId];
				
				for(int i = 0; i < noEntries; i++)
				{
					if(hash[i].ptr >= -1 && swapStates_v[localMapId][i].state != 2)
					{
						exportVoxelsInBlock<TVoxel, TIndex>(file, voxelBlocks_global_v[localMapId], i, hash[i].pos, &hashTables.posesInv[localMapId]);
					}
					else if(hash[i].ptr >= 0 && swapStates_v[localMapId][i].state == 2)
					{
						exportVoxelsInBlock<TVoxel, TIndex>(file, voxelBlocks_local_v[localMapId], hash[i].ptr, hash[i].pos, &hashTables.posesInv[localMapId]);
					}
				}



				//localVBAs.voxels[localMapId] = scene->localVBA.GetVoxelBlocks();
			}

			return true;
		}



	template<class TVoxel, class TIndex>
		bool ITMMultiExportEngine_CPU<TVoxel, TIndex>::ExportTSDFToPcd(const char *filename)
		{ return false; }


	template <>
		bool ITMMultiExportEngine_CPU<ITMVoxel_s_rgb, ITMVoxelBlockHash>::ExportTSDFToPcd(const char *filename)
		{
			return ExportTSDFToPcd_hashIndex(sceneManager_, filename);
		}

	template <>
		bool ITMMultiExportEngine_CPU<ITMVoxel_s, ITMVoxelBlockHash>::ExportTSDFToPcd(const char *filename)
		{
			return ExportTSDFToPcd_hashIndex(sceneManager_, filename);
		}

	template <>
		bool ITMMultiExportEngine_CPU<ITMVoxel_f, ITMVoxelBlockHash>::ExportTSDFToPcd(const char *filename)
		{
			return ExportTSDFToPcd_hashIndex(sceneManager_, filename);
		}

	template <>
		bool ITMMultiExportEngine_CPU<ITMVoxel_f_rgb, ITMVoxelBlockHash>::ExportTSDFToPcd(const char *filename)
		{
			return ExportTSDFToPcd_hashIndex(sceneManager_, filename);
		}


} // namespace
