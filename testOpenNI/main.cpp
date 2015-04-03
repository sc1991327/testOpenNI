#include <stdio.h>

// 1. include OpenNI Header
#include <OpenNI.h>

// OpenCV Header
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "OniSampleUtilities.h"

#define SAMPLE_READ_WAIT_TIMEOUT 2000 //2000ms

using namespace std;
using namespace openni;

int main()
{
	// 2. initialize OpenNI
	Status rc = OpenNI::initialize();
	if (rc != STATUS_OK)
	{
		printf("Initialize failed\n%s\n", OpenNI::getExtendedError());
		return 1;
	}

	// 3. open a device
	Device device;
	rc = device.open(ANY_DEVICE);
	if (rc != STATUS_OK)
	{
		printf("Couldn't open device\n%s\n", OpenNI::getExtendedError());
		return 2;
	}

	// 4. create depth stream
	VideoStream depth;
	if (device.getSensorInfo(SENSOR_DEPTH) != NULL){
		rc = depth.create(device, SENSOR_DEPTH);
		if (rc != STATUS_OK){
			printf("Couldn't create depth stream\n%s\n", OpenNI::getExtendedError());
			return 3;
		}
	}
	VideoStream color;
	if (device.getSensorInfo(SENSOR_COLOR) != NULL){
		rc = color.create(device, SENSOR_COLOR);
		if (rc != STATUS_OK){
			printf("Couldn't create color stream\n%s\n", OpenNI::getExtendedError());
			return 4;
		}
	}

	// 5. create OpenCV Window
	cv::namedWindow("Depth Image", CV_WINDOW_AUTOSIZE);
	cv::namedWindow("Color Image", CV_WINDOW_AUTOSIZE);

	// 6. start
	rc = depth.start();
	if (rc != STATUS_OK)
	{
		printf("Couldn't start the depth stream\n%s\n", OpenNI::getExtendedError());
		return 5;
	}
	rc = color.start();
	if (rc != STATUS_OK){
		printf("Couldn't start the depth stream\n%s\n", OpenNI::getExtendedError());
		return 6;
	}
	VideoFrameRef colorframe;
	VideoFrameRef depthframe;
	int iMaxDepth = depth.getMaxPixelValue();

	cv::Mat colorimageRGB;
	cv::Mat colorimageBGR;
	cv::Mat depthimage;
	cv::Mat depthimageScaled;

	// 7. main loop, continue read
	while (!wasKeyboardHit())
	{
		// 8. check is color stream is available
		if (color.isValid()){
			if (color.readFrame(&colorframe) == STATUS_OK){
				colorimageRGB = { colorframe.getHeight(), colorframe.getWidth(), CV_8UC3, (void*)colorframe.getData() };
				cv::cvtColor(colorimageRGB, colorimageBGR, CV_RGB2BGR);
			}
		}

		// 9. check is depth stream is available
		if (depth.isValid()){
			if (depth.readFrame(&depthframe) == STATUS_OK){
				depthimage = { depthframe.getHeight(), depthframe.getWidth(), CV_16UC1, (void*)depthframe.getData() };
				depthimage.convertTo(depthimageScaled, CV_8U, 255.0 / iMaxDepth);
			}
		}

		cv::imshow("Color Image", colorimageBGR);
		cv::imshow("Depth Image", depthimageScaled);
		cv::waitKey(10);
	}

	color.stop();
	depth.stop();

	color.destroy();
	depth.destroy();

	device.close();
	OpenNI::shutdown();

	return 0;
}
