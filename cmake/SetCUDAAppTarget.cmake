##########################
# SetCUDAAppTarget.cmake #
##########################

INCLUDE(${PROJECT_SOURCE_DIR}/cmake/Flags.cmake)

#IF(WITH_CUDA)
#  CUDA_ADD_EXECUTABLE(${targetname} ${sources} ${headers} ${templates})
#ELSE()
#  ADD_EXECUTABLE(${targetname} ${sources} ${headers} ${templates})
#ENDIF()
if(WITH_CUDA)
	list(APPEND CUDA_NVCC_FLAGS "-std=c++11") # If ROS
  include_directories(${CUDA_INCLUDE_DIRS})
endif()
cs_add_executable(${targetname} ${sources} ${headers} ${templates})
cs_install()


IF(MSVC_IDE)
  SET_TARGET_PROPERTIES(${targetname} PROPERTIES LINK_FLAGS_DEBUG "/DEBUG")
ENDIF()
