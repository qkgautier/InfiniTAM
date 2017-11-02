// authored by https://github.com/ravich2-7183

#include "ROSEngine.h"

using namespace ORUtils;
using namespace InputSource;
using namespace sensor_msgs;
using namespace message_filters;

void ROSEngine::processMessage(const ImageConstPtr& rgb_image, const ImageConstPtr& depth_image)
{
	std::lock_guard<std::mutex> process_message_lock(images_mutex_);

	// TODO document that rgb_image must have an encoding equivalent to RGBA8

	// copy rgb_image into rgb_image_
	Vector4u *rgb = rgb_image_.GetData(MEMORYDEVICE_CPU);
	for(int i = 0; i < rgb_image_.noDims.x * rgb_image_.noDims.y; i++) {
		Vector4u newPix;
		newPix.x = (rgb_image->data)[i*4+0];
		newPix.y = (rgb_image->data)[i*4+1];
		newPix.z = (rgb_image->data)[i*4+2];
		newPix.w = 255;

		rgb[i] = newPix;
	}

	// copy depth_image into depth_image_
	short *depth = depth_image_.GetData(MEMORYDEVICE_CPU);
	for(int i = 0; i < depth_image_.noDims.x * depth_image_.noDims.y; i++) {
		depth[i] = (depth_image->data)[i*sizeof(short)];
	}
}

void ROSEngine::topicListenerThread()
{
	// subscribe to rgb and depth topics
	message_filters::Subscriber<sensor_msgs::Image> rgb_sub_(nh_, "/dtam/rgb", 1); // TODO remove dtam and generalize to: /namespace/rgb, /namespace/depth
	message_filters::Subscriber<sensor_msgs::Image> depth_sub_(nh_, "/dtam/depth", 1); // uint16 depth image in mm. Native OpenNI format, preferred by InfiniTAM.
	TimeSynchronizer<sensor_msgs::Image, sensor_msgs::Image> sync(rgb_sub_, depth_sub_, 10);
	sync.registerCallback(boost::bind(&ROSEngine::processMessage, this, _1, _2));

	ros::spin();
}

ROSEngine::ROSEngine(const char *calibFilename,
					 Vector2i requested_imageSize_rgb,
					 Vector2i requested_imageSize_d) :
	BaseImageSourceEngine(calibFilename),
	nh_(),
	rgb_image_(requested_imageSize_rgb, MEMORYDEVICE_CPU),
	depth_image_(requested_imageSize_d, MEMORYDEVICE_CPU)
{
	// Start ROS
	ros::start();

	// Start up topic listener thread
	std::thread topic_listener_thread(&ROSEngine::topicListenerThread, this);
	topic_listener_thread.join();

	// TODO document that depth images must be in millimeters
	this->calib.disparityCalib.SetStandard(); // assumes depth is in millimeters
}

void ROSEngine::getImages(ITMUChar4Image *rgbImage, ITMShortImage *rawDepthImage)
{
	std::lock_guard<std::mutex> get_images_lock(images_mutex_);

	rgbImage->SetFrom(&rgb_image_,        MemoryBlock<Vector4u>::CPU_TO_CPU); 
	rawDepthImage->SetFrom(&depth_image_, MemoryBlock<short>::CPU_TO_CPU);
}

bool ROSEngine::hasMoreImages(void) const
{
	return ros::ok();
}

Vector2i ROSEngine::getDepthImageSize(void) const
{
	return Vector2i(depth_image_.noDims.x , depth_image_.noDims.y);
}

Vector2i ROSEngine::getRGBImageSize(void) const
{
	return Vector2i(rgb_image_.noDims.x , rgb_image_.noDims.y);
}

ROSEngine::~ROSEngine()
{
	ros::shutdown();
}
