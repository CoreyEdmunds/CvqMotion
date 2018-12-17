/************************************************************************
 * CvqMotion - A Motion Tracking Webcam Application
 * Copyright (C) 2018, Corey Edmunds
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 ************************************************************************/

#ifndef VIDEOPROCESSOR_H
#define VIDEOPROCESSOR_H

#include <QThread>
#include <mutex>
#include <opencv2/opencv.hpp>
#include <atomic>

#include "entity.h"
#include "detectionzone.h"
#include "videoprocessordetectionsettings.h"

namespace cvqm {
	class VideoProcessor;
	class DetectionObserver;
	class OutputImageObserver;
}

class cvqm::DetectionObserver {
public:
	virtual void detected(DetectionZone *zone, Entity *e, cv::Mat &frame) = 0;
//	virtual ~DetectionObserver() {}
};

class cvqm::OutputImageObserver {
public:
	virtual void renderedImage(const cv::Mat *image) = 0;
//	virtual ~OutputImageObserver() ;
};

class cvqm::VideoProcessor
{
private:
	enum OverlapType {
		OVERLAP_TYPE_NONE = 0,
		OVERLAP_TYPE_CONTAINS = 1,
		OVERLAP_TYPE_CONTAINED = 2,
		OVERLAP_TYPE_OVERLAPS = 4
	};

	std::mutex dsMutex;
	std::list<Entity*> entities;
	std::list<DetectionZone*> detectionZones;
	std::list<cv::Rect*> maskZones;
	uint *thresholdTime = nullptr;

	int device_id = 0;
	int xRes = 640;
	int yRes = 480;

	ulong entityIdCounter = 0;
	ulong frameIdCounter = 0;

	cvqm::VideoProcessorDetectionSettings s;

	void performBackgroundBlending(cv::Mat& frame, cv::Mat& baseFrame, cv::Mat& delta, cv::Mat &dilatedBlending, uint thresholdTime[]);
	void detect(ulong frameid, cv::Mat& frame);
	void correlate(std::vector<cv::Rect> &rects, cv::Mat& frame, cv::Mat& delta, ulong frameId, double frameTime);
	void endEntities(ulong frameId, cv::Rect *borderRect);
	void paintEntities(cv::Mat &paint, ulong frameId, double frameTime, double dFrameTime);
	void paintDetectionZone(cv::Mat &paint, DetectionZone *z);
	bool sharesBorders(cv::Rect *r1, cv::Rect *r2, int w);
	void destroyDebugWindows();
	void showDebugWindow(const cv::Mat &image, const char label[], std::atomic<bool> &control, bool &state);

	bool destroyOriginal = false;
	bool destroyBlur = false;
	bool destroyDelta = false;
	bool destroyThreshold = false;
	bool destroyBackground = false;
	bool destroyDilated = false;
	bool destroyDilatedBlending = false;
	bool destroyOutput = false;

	std::atomic<bool> shutdownRequested{false};
public:
	static const cv::Scalar BOUNDING_BOX_COLOUR;
	static const cv::Scalar STALE_BOX_COLOUR;
	static const cv::Scalar ENTITY_BOX_COLOUR;
	static const cv::Scalar DEADRECKON_BOX_COLOUR;
	static const cv::Scalar DETECTION_ZONE_COLOUR;
	static const cv::Scalar MASK_ZONE_COLOUR;
	static const cv::Scalar CONTOUR_COLOUR;

	std::atomic<bool> showOriginal{false};
	std::atomic<bool> showBlur{false};
	std::atomic<bool> showDelta{false};
	std::atomic<bool> showThreshold{false};
	std::atomic<bool> showBackground{false};
	std::atomic<bool> showDilated{false};
	std::atomic<bool> showDilatedBlending{false};
	std::atomic<bool> showOutput{false};

	static bool contains(const cv::Rect &r, cv::Point2i p);
	static OverlapType overlaps(const cv::Rect& r, const cv::Rect& e, double& overlapRatio);

	void addDetectionZone(const DetectionZone &z);
	void addMaskZone(const cv::Rect &r);
	void removeDetectionZones(const std::function<bool(DetectionZone*)> &test);
	void removeMaskZones(const std::function<bool(cv::Rect*)> &test);
	VideoProcessorDetectionSettings *getCurrentConfiguration();
	void setCurrentConfiguration(VideoProcessorDetectionSettings *);

	VideoProcessor();
	~VideoProcessor();

	void run();
	void requestShutdown(bool shutdown = true);
	void setDeviceId(int id);
	void setResolution(int xRes, int yRes);

	OutputImageObserver *outputImageObserver = nullptr;
	DetectionObserver *detectionObserver = nullptr;
};

#endif // VIDEOPROCESSOR_H
