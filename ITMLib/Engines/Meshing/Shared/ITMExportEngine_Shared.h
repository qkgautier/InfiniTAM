// Created by Quentin Gautier on 2018/5/12
//

#pragma once

#include "../../../Utils/ITMMath.h"

#include <fstream>

namespace ITMLib
{
	template<class TVoxel, class TIndex>
		int countVoxelsInBlock(const TVoxel* voxelBlocks, int block_ptr)
		{
			int noPoints = 0;

			for(int z = 0; z < SDF_BLOCK_SIZE; z++)
			{
				for(int y = 0; y < SDF_BLOCK_SIZE; y++)
				{
					for(int x = 0; x < SDF_BLOCK_SIZE; x++)
					{
						int point_offset = x + y*SDF_BLOCK_SIZE + z*SDF_BLOCK_SIZE*SDF_BLOCK_SIZE;

						const TVoxel& voxel = voxelBlocks[block_ptr * SDF_BLOCK_SIZE3 + point_offset];

						if(voxel.sdf != TVoxel::SDF_initialValue()){ noPoints++; }
					}
				}
			}

			return noPoints;
		}

	template<class TVoxel, class TIndex>
		void exportVoxelsInBlock(std::ofstream& file, const TVoxel* voxelBlocks, int block_ptr, Vector3s blockPos)
		{
			Vector3f pointPos = blockPos.toFloat() * SDF_BLOCK_SIZE;

			for(int z = 0; z < SDF_BLOCK_SIZE; z++)
			{
				for(int y = 0; y < SDF_BLOCK_SIZE; y++)
				{
					for(int x = 0; x < SDF_BLOCK_SIZE; x++)
					{
						int point_offset = x + y*SDF_BLOCK_SIZE + z*SDF_BLOCK_SIZE*SDF_BLOCK_SIZE;
						int voxel_ptr = block_ptr * SDF_BLOCK_SIZE3 + point_offset;
						const TVoxel& voxel = voxelBlocks[voxel_ptr];

						if(voxel.sdf == TVoxel::SDF_initialValue()){ continue; }

						// Export X,Y,Z
						Vector3f currPointPos = pointPos + Vector3f(x, y, z);
						//file << currPointPos.x << " " << currPointPos.y << " " << currPointPos.z << " ";
						file.write((const char*)&currPointPos, 3 * sizeof(float));


						// Export colors
						if(TVoxel::hasColorInformation)
						{
							// hack to get color information without using templates
							// TODO use templates
							const uchar* raw_ptr = reinterpret_cast<const uchar*>(voxelBlocks);
							const uchar* clr_ptr = raw_ptr + (voxel_ptr+1) * sizeof(TVoxel) -
								sizeof(uchar) - sizeof(Vector3u) - 1;

							unsigned int rgb = ((unsigned int)clr_ptr[0] << 16 | (unsigned int)clr_ptr[1] << 8 | (unsigned int)clr_ptr[2]);
							float rgb_f = *reinterpret_cast<float*>(&rgb);

							//file << rgb_f << " ";
							file.write((const char*)&rgb_f, sizeof(float));
						}

						// Export SDF value
						float tsdf = TVoxel::valueToFloat(voxel.sdf);
						//file << tsdf << "\n";
						file.write((const char*)&tsdf, sizeof(float));
					}
				}
			}
		}


} // namespace
