##########################################
# Top-level CMakeLists.txt for InfiniTAM #
##########################################

cmake_minimum_required(VERSION 2.8)

PROJECT(infinitam)

#################################
# Specify the CMake module path #
#################################

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

####################################
# Find catkin macros and libraries #
####################################

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  message_filters
)

find_package(catkin_simple REQUIRED)
catkin_simple(ALL_DEPS_REQUIRED)

find_package(Boost REQUIRED
  system
)

include_directories(
  ${Boost_INCLUDE_DIRS}
  ${catkin_INCLUDE_DIRS}
)

#catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES using_markers
#  CATKIN_DEPENDS roscpp visualization_msgs
#  DEPENDS system_lib
#)

find_package(OpenCV)
include_directories(${OpenCV_INCLUDE_DIRS})

#####################
# Offer ROS Support (Disable as my modifications were based on a pull request and not on infinitam_v3.5) #
#####################

#INCLUDE(${PROJECT_SOURCE_DIR}/cmake/OfferROSSupport.cmake)

#################################
# Add additional compiler flags #
#################################

IF(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++ -Wno-deprecated-declarations -Wno-unused-function")
ENDIF()

IF(NOT MSVC_IDE)
  SET(CFLAGS_WARN "-Wall -Wextra -Wno-unused-parameter -Wno-strict-aliasing")
  SET(CMAKE_CXX_FLAGS "-fPIC -O3 -march=native ${CFLAGS_WARN} ${CMAKE_CXX_FLAGS}")
  #SET(CMAKE_CXX_FLAGS "-fPIC -g ${CFLAGS_WARN} ${CMAKE_CXX_FLAGS}")
ENDIF()

#########################
# Suppress auto-linking #
#########################

IF(MSVC_IDE)
  ADD_DEFINITIONS(-DUSING_CMAKE=1)
ENDIF()

######################
# Add subdirectories #
######################

ADD_SUBDIRECTORY(Apps)
ADD_SUBDIRECTORY(FernRelocLib)
ADD_SUBDIRECTORY(InputSource)
ADD_SUBDIRECTORY(ITMLib)
ADD_SUBDIRECTORY(MiniSlamGraphLib)
ADD_SUBDIRECTORY(ORUtils)


cs_install()
cs_export()
