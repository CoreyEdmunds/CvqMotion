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

#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <sstream>
#include <tuple>
#include <mutex>

#include "entity.h"
#include "videoprocessorconstants.h"
#include "videoprocessor.h"

using namespace cv;
using namespace std;
using namespace cvqm;

const Scalar VideoProcessor::BOUNDING_BOX_COLOUR = Scalar(0, 0, 255);
const Scalar VideoProcessor::STALE_BOX_COLOUR = Scalar(0, 255, 255);
const Scalar VideoProcessor::ENTITY_BOX_COLOUR = Scalar(0, 255, 0);
const Scalar VideoProcessor::DEADRECKON_BOX_COLOUR = Scalar(255, 0, 0);
const Scalar VideoProcessor::DETECTION_ZONE_COLOUR = Scalar(DETECTION_ZONE_COLOUR_B,DETECTION_ZONE_COLOUR_G,DETECTION_ZONE_COLOUR_R);
const Scalar VideoProcessor::MASK_ZONE_COLOUR = Scalar(MASK_ZONE_COLOUR_B,MASK_ZONE_COLOUR_G,MASK_ZONE_COLOUR_R);
const Scalar VideoProcessor::CONTOUR_COLOUR = Scalar( 0,0,255 );

DetectionObserver::~DetectionObserver() {}
OutputImageObserver::~OutputImageObserver() {}

VideoProcessor::VideoProcessor() = default;

VideoProcessor::~VideoProcessor()
{
	for(Entity *e: this->entities)
		delete e;
	for(DetectionZone *z: this->detectionZones)
		delete z;
	for(Rect *r: this->maskZones)
		delete r;
	delete this->thresholdTime;
}

void VideoProcessor::setDeviceId(int id)
{
	this->device_id = id;
}

void VideoProcessor::setResolution(int xRes, int yRes)
{
	this->xRes = xRes;
	this->yRes = yRes;
}

void VideoProcessor::run()
{
	VideoCapture cap(this->device_id);
	cap.set(CV_CAP_PROP_FRAME_WIDTH, xRes);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, yRes);

	if(!cap.isOpened()) {
		string err = "Unable to open " + to_string(this->device_id);
		throw new invalid_argument(err);
	}

	bool greyscale;
	{
			lock_guard<mutex> datastructureLock(this->dsMutex);
			greyscale = this->s.greyscale;
	}

	Mat backgroundFrame;
	{
		Mat frame;
		cap >> frame;
		if ( greyscale )
			cvtColor(frame, backgroundFrame, CV_BGR2GRAY);
		else
			backgroundFrame = frame;
	}

	Rect borderRect(1, 1, backgroundFrame.cols-2, backgroundFrame.rows-2);

	if ( this->thresholdTime != nullptr )
		delete this->thresholdTime;
	auto frameLength = static_cast<ulong>(backgroundFrame.rows * backgroundFrame.cols * backgroundFrame.channels());
	thresholdTime = new uint[frameLength];
	memset(thresholdTime, 0, frameLength * sizeof(uint));

	auto t0 = chrono::system_clock::now();
	double lastFrameTime = 0;

	for(;;) {
		Mat sourceFrame;
		cap >> sourceFrame;
		this->frameIdCounter++;
		showDebugWindow(sourceFrame, ORIGINAL_INPUT, showOriginal, shownOriginal);

		// Calculate frame timestamps for velocity calculations
		auto t1 = chrono::system_clock::now();
		chrono::duration<double> frameTimeDuration = t1-t0;

		double frameTime = frameTimeDuration.count();
		double dFrameTime = frameTime - lastFrameTime;
		lastFrameTime = frameTime;

		// Convert to greyscale if greyscale mode
		Mat frame;
		if ( greyscale ) {
			cvtColor(sourceFrame, frame, CV_BGR2GRAY);
		} else {
			frame = sourceFrame;
		}

		lock_guard<mutex> datastructureLock(this->dsMutex);

		// Calculate current frame difference from background
		Mat blurBaseFrame;
		Mat blurFrame;
		GaussianBlur(frame, blurFrame, Size(s.blur_radius,s.blur_radius), s.blur_stdev, 0, BORDER_REFLECT_101);
		GaussianBlur(backgroundFrame, blurBaseFrame, Size(s.blur_radius,s.blur_radius), s.blur_stdev, 0, BORDER_REFLECT_101);
		Mat delta;
		absdiff(blurFrame, blurBaseFrame, delta);
		showDebugWindow(blurFrame, BLURRED_INPUT, showBlur, shownBlur);
		showDebugWindow(delta, BACKGROUND_DIFFERENCE, showDelta, shownDelta);

		// Threshold frame difference to detect motion
		Mat detectionThresholdRgb;
		threshold(delta, detectionThresholdRgb, s.detection_threshold, 255, THRESH_BINARY);
		showDebugWindow(detectionThresholdRgb, THRESHOLDED_DELTA, showThreshold, shownThreshold);
		Mat detectionThreshold;
		if ( !greyscale )
			cvtColor(detectionThresholdRgb, detectionThreshold, COLOR_BGR2GRAY);
		else
			detectionThreshold = detectionThresholdRgb;

		// Dilate the thresholded frame
		Mat dilateDetectionKernel = getStructuringElement(
					MORPH_ELLIPSE,
					Size(2*s.dilateDetectionFactor, 2*s.dilateDetectionFactor),
					Point(s.dilateDetectionFactor,s.dilateDetectionFactor)
					);
		Mat dilatedDetection;
		for(Rect *maskZone: maskZones)
			rectangle(detectionThreshold, *maskZone, Scalar(0), -1);
		dilate(detectionThreshold, dilatedDetection, dilateDetectionKernel);
		showDebugWindow(dilatedDetection, DILATED_THRESHOLD, showDilated, shownDilated);

		// Find contours and bounding boxes around thresholded objects
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		findContours(dilatedDetection, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);

		Mat rectOutput;
		sourceFrame.copyTo(rectOutput);
		vector<Rect> rects(contours.size());
		for(ulong i=0; i<contours.size(); i++) {
				Rect r = boundingRect(contours[i]);
				rects[i] = r;
		}

		// Correlate and process detected motion
		correlate(rects, frame, this->frameIdCounter, frameTime);
		detect(this->frameIdCounter, sourceFrame);
		endEntities(this->frameIdCounter, &borderRect);
		paintEntities(rectOutput, this->frameIdCounter, frameTime, dFrameTime);
		performBackgroundBlending(frame, backgroundFrame, delta, thresholdTime);


		if ( this->showOutput.load() || this->outputImageObserver != nullptr )
			for(vector<vector<Point>>::size_type i = 0; i< contours.size(); i++ )
				drawContours( rectOutput, contours, static_cast<int>(i), CONTOUR_COLOUR, 1, 8, hierarchy, 0, Point() );
		showDebugWindow(rectOutput, LABELED_OUTPUT, showOutput, shownOutput);
		showDebugWindow(backgroundFrame, BACKGROUND_FRAME, showBackground, shownBackground);

		if ( this->outputImageObserver != nullptr ) {
			this->outputImageObserver->renderedImage(&rectOutput);
		}

		if ( shutdownRequested.load() ) {
			destroyDebugWindows();
			return;
		}
	}
}

void VideoProcessor::showDebugWindow(const Mat &image, const char label[], atomic<bool> &control, bool &shown)
{
	if ( control.load() ) {
		imshow(label, image);
		shown  = true;
	} else if (shown) {
		destroyWindow(label);
		shown = false;
	}
}

void VideoProcessor::destroyDebugWindow(const char label[], bool &shown) {
	if ( shown ) {
		destroyWindow(label);
		shown = false;
	}
}

void VideoProcessor::destroyDebugWindows()
{
	destroyDebugWindow(ORIGINAL_INPUT, this->shownOriginal);
	destroyDebugWindow(BLURRED_INPUT, this->shownBlur);
	destroyDebugWindow(BACKGROUND_DIFFERENCE, this->shownDelta);
	destroyDebugWindow(THRESHOLDED_DELTA, this->shownThreshold);
	destroyDebugWindow(BACKGROUND_FRAME, this->shownBackground);
	destroyDebugWindow(DILATED_THRESHOLD, this->shownDilated);
	destroyDebugWindow(LABELED_OUTPUT, this->shownOutput);
	destroyDebugWindow(DILATED_BLENDING_THRESHOLD,  this->shownDilatedBlending);
}

void VideoProcessor::detect(ulong frameId, Mat &frame)
{
	for(DetectionZone *zone: this->detectionZones) {
		for(Entity *e: this->entities) {
			double ratio = 0;
			if ( overlaps(e->box, zone->zone, ratio) != OVERLAP_TYPE_NONE ) {
				bool angleMatch = (!zone->directional) || zone->acceptableAngle(e->getBearingRadians());
				if ( angleMatch ) {
					if ( frameId - e->detections[zone] > s.detection_timeout ) {
						if ( this->detectionObserver )
							this->detectionObserver->detected(zone, e, frame);
						//cout << "Detected " << e->str() << " " << vel << "km/h " << dir << endl;
					}
					e->detections[zone] = frameId;
				}
			}
		}
	}
}

void VideoProcessor::endEntities(ulong frameId, Rect *borderRect)
{
	list<Entity*> toRemove;
	for(Entity *e: this->entities) {
		if ( (e->lastUpdateFrameId < frameId && e->bbHistory.size() == 1) ||  // remove blips
			 (e->lastUpdateFrameId + s.entity_timeout < frameId ) ) {  // unmatched for entity_timeout frames
			toRemove.push_back(e);
			continue;
		}

		if ( sharesBorders(&e->box, borderRect, s.borderWidth) ) {
			toRemove.push_back(e);
		}
	}
	for(Entity *e: toRemove) {
		this->entities.remove(e); // warning O(n)
		delete e;
	}
}

void VideoProcessor::paintEntities(Mat &paint, ulong frameId, double frameTime, double dFrameTime)
{
	for(Entity *e: this->entities) {
		ostringstream str;
		str << e->id;

		cv::putText(paint, str.str(), Point(e->box.x, e->box.y), FONT_HERSHEY_COMPLEX_SMALL, 0.5, Scalar(255,255,255));
		if ( frameId == e->lastUpdateFrameId ) {
			rectangle(paint, e->deadRecon(frameTime + dFrameTime), DEADRECKON_BOX_COLOUR);
			rectangle(paint, e->box, ENTITY_BOX_COLOUR);
		} else {
			Rect dead = e->deadRecon((frameTime));
			cv::putText(paint, str.str(), Point(dead.x, dead.y), FONT_HERSHEY_COMPLEX_SMALL, 0.5, Scalar(255,255,255));
			rectangle(paint, e->deadRecon(frameTime), DEADRECKON_BOX_COLOUR);
			rectangle(paint, e->box, STALE_BOX_COLOUR);
		}
		str.str("");
		str.clear();
		double vel = round(sqrt(e->vel[0]*e->vel[0] + e->vel[1]*e->vel[1])*100.0)/100.0;
		str << vel << "pps";

		cv::putText(paint, str.str(), Point(e->box.x, e->box.y + e->box.height+5), FONT_HERSHEY_COMPLEX_SMALL, 0.5, Scalar(255,255,255));
	}
	for(DetectionZone *r: detectionZones) {
		paintDetectionZone(paint, r);
	}
	for(Rect *r: maskZones) {
		rectangle(paint, *r, MASK_ZONE_COLOUR);
	}
}

void VideoProcessor::paintDetectionZone(Mat &paint, DetectionZone *z)
{
	rectangle(paint, z->zone, DETECTION_ZONE_COLOUR);

	if ( !z->directional )
		return;

	auto x = static_cast<int>(z->zone.x + z->zone.width/2.0);
	auto y = static_cast<int>(z->zone.y + z->zone.height/2.0);

	double angle = z->acceptAngle + M_PI;

	auto x1 = static_cast<int>(x + 15 * cos(angle));
	auto y1 = static_cast<int>(y + 15 * sin(angle));

	auto x2 = static_cast<int>(x + 10 * cos(angle + 0.5 * z->acceptWidth));
	auto y2 = static_cast<int>(y + 10 * sin(angle + 0.5 * z->acceptWidth));

	auto x3 = static_cast<int>(x + 10 * cos(angle - 0.5 * z->acceptWidth));
	auto y3 = static_cast<int>(y + 10 * sin(angle - 0.5 * z->acceptWidth));

	line(paint, Point(x,y), Point(x1, y1), DETECTION_ZONE_COLOUR);
	line(paint, Point(x,y), Point(x2, y2), DETECTION_ZONE_COLOUR);
	line(paint, Point(x,y), Point(x3, y3), DETECTION_ZONE_COLOUR);
}

void VideoProcessor::performBackgroundBlending(Mat& frame, Mat& baseFrame, Mat& delta, uint thresholdTime[])
{
	int frameLength = delta.rows * delta.cols * delta.channels();

	Mat blendingThreshold;
	threshold(delta, blendingThreshold, s.blending_threshold, 255, THRESH_BINARY);


	Mat dilateBlendingKernel = getStructuringElement(MORPH_ELLIPSE,
													 Size(2*s.dilateBlendingFactor, 2*s.dilateBlendingFactor),
													 Point(s.dilateBlendingFactor,s.dilateBlendingFactor)
													 );

	Mat dilatedBlending;
	dilate(blendingThreshold, dilatedBlending, dilateBlendingKernel);

	showDebugWindow(dilatedBlending, DILATED_BLENDING_THRESHOLD, showDilatedBlending, shownDilatedBlending);

	int threshCount = 0;
	for(int x=0; x < frameLength; x++) {
		if(!dilatedBlending.data[x]) {
			thresholdTime[x] = 0;
			baseFrame.data[x] = static_cast<uchar>(
						(1.0f-s.background_blend_ratio) * baseFrame.data[x] +
						s.background_blend_ratio * frame.data[x]);
		} else {
			thresholdTime[x]++;
			threshCount++;
			if ( thresholdTime[x] > s.threshold_timeout )
				baseFrame.data[x] = static_cast<uchar>(
							(1.0f-s.foreground_blend_ratio) * baseFrame.data[x] +
							s.foreground_blend_ratio * frame.data[x]);

		}
	}

	if ( threshCount > s.foreground_overload_level * frameLength )
		baseFrame = frame;
}

bool VideoProcessor::sharesBorders(Rect *r1, Rect *r2, int w)
{
	return r1->x <= r2->x + w ||
			r1->x+r1->width >= r2->x+r2->width - w ||
			r1->y <= r2->y + w ||
			r1->y+r1->height >= r2->y+r2->height - w;
}

void VideoProcessor::requestShutdown(bool shutdown)
{
	lock_guard<mutex> datastructureLock(this->dsMutex);
	this->shutdownRequested.store(shutdown);
}

void VideoProcessor::correlate(vector<Rect>& rects, Mat& frame,  ulong frameId, double frameTime)
{
	map<Rect*, list<tuple<Entity*, OverlapType, double>>> rectOverlaps;
	map<Entity*, list<tuple<Rect*, OverlapType, double>>> entityOverlaps;

	Rect borderRect(0, 0, frame.cols, frame.rows);

	for(vector<Rect>::size_type i=0; i<rects.size(); i++) {
		Rect *bb = &rects[i];

		for(Entity *e: entities) {
			double overlap = 0;

			Rect dr = e->deadRecon(frameTime);
			OverlapType o = overlaps(*bb, dr, overlap);

			if ( o != OVERLAP_TYPE_NONE || overlap > 0.75) {
				rectOverlaps[bb].push_back(tuple<Entity*, OverlapType, double>(e, o, overlap));
				entityOverlaps[e].push_back(tuple<Rect*, OverlapType, double>(bb, o, overlap));
			}
		}

		if ( rectOverlaps[bb].empty() && !sharesBorders(bb, &borderRect, s.borderWidth)) {
			Entity *e = new Entity(*bb, frameId);
			//cout << "  Created new Entity " << e->str() << endl;
			this->entities.push_back(e);
			rectOverlaps[bb].push_back(tuple<Entity*, OverlapType, double>(e, OVERLAP_TYPE_OVERLAPS, 1.0));
			for(uint j=0; j<=i; j++) {
				double overlap = 0;
				Rect *bbj = &rects[j];
				OverlapType o = overlaps(*bbj, e->box, overlap);
				if ( o != OVERLAP_TYPE_NONE || overlap > 0.75) {
					rectOverlaps[bbj].push_back(tuple<Entity*, OverlapType, double>(e, o, overlap));
					entityOverlaps[e].push_back(tuple<Rect*, OverlapType, double>(bbj, OVERLAP_TYPE_OVERLAPS, 1.0));
				}
			}
		}
	}


	map<Rect*, list<tuple<Entity*, OverlapType, double>*>> bestMatch;

	for(auto it:  entityOverlaps) {
		// todo convert to pointers
		tuple<Rect*, OverlapType, double> bestOverlap(nullptr, OVERLAP_TYPE_NONE, 0);
		double bestConfidence = 0;

		for(tuple<Rect*, OverlapType, double> overlap: it.second) {
			double confidence = 0;
			switch(get<1>(overlap)) {
			case OVERLAP_TYPE_CONTAINS:
				confidence = get<2>(overlap);
				break;
			case OVERLAP_TYPE_CONTAINED:
				confidence = get<2>(overlap);
				break;
			case OVERLAP_TYPE_OVERLAPS:
				confidence = get<2>(overlap);
				break;
			case OVERLAP_TYPE_NONE:
				confidence = get<2>(overlap) / 2;
				break;
			default:
				throw invalid_argument("Overlap Type Unknown");
			}
			if ( confidence > bestConfidence ) {
				bestConfidence = confidence;
				bestOverlap = overlap;
			}
		}

		if ( get<0>(bestOverlap) != nullptr ) {
			bestMatch[get<0>(bestOverlap)].push_back(new tuple<Entity*, OverlapType, double>(it.first, get<1>(bestOverlap), get<2>(bestOverlap)));
		}
	}

	for(pair<Rect*, list<tuple<Entity*, OverlapType, double>*>> matchSet: bestMatch) {
		if ( matchSet.second.size() == 1 ) {
			get<0>(*matchSet.second.front())->update(matchSet.first, frameId, frameTime);
		}
		for(tuple<Entity*, OverlapType, double>* match: matchSet.second) {
			Entity *e = get<0>(*match);
			if ( e->id == 0 && e->bbHistory.size() > 1 ) // don't assign IDs to blips
				e->assignId(this->entityIdCounter++);
			delete match;
		}
	}
}

bool VideoProcessor::contains(const Rect &r, Point2i p)
{
	int xl = r.x;
	int xh = r.x + r.width;
	int yl = r.y;
	int yh = r.y + r.width;

	return p.x >= xl && p.x <= xh && p.y >= yl && p.y <= yh;
}

void VideoProcessor::addDetectionZone(const DetectionZone &z)
{
	lock_guard<mutex> datastructureLock(this->dsMutex);
	this->detectionZones.push_back(new DetectionZone(z));
}

void VideoProcessor::removeDetectionZones(const function<bool(DetectionZone*)> &test)
{
	lock_guard<mutex> datastructureLock(this->dsMutex);
	this->detectionZones.remove_if(test);
}

void VideoProcessor::addMaskZone(const Rect &r)
{
	lock_guard<mutex> datastructureLock(this->dsMutex);
	this->maskZones.push_back(new Rect(r));
}

void VideoProcessor::removeMaskZones(const function<bool(Rect*)> &test)
{
	lock_guard<mutex> datastructureLock(this->dsMutex);
	this->maskZones.remove_if(test);
}

VideoProcessorDetectionSettings* VideoProcessor::getCurrentConfiguration()
{
	lock_guard<mutex> datastructureLock(this->dsMutex);
	return new VideoProcessorDetectionSettings(s);
}

void VideoProcessor::setCurrentConfiguration(VideoProcessorDetectionSettings *newSettings)
{
	lock_guard<mutex> datastructureLock(this->dsMutex);
	this->s = *newSettings;
}

VideoProcessor::OverlapType VideoProcessor::overlaps(const Rect& r, const Rect& e, double &overlapRatio)
{
	bool etl = contains(e, r.tl());//e.contains(r.tl());
	bool ebr = contains(e, r.br());//e.contains(r.br());
	bool rtl = contains(r, e.tl());//r.contains(e.tl());
	bool rbr = contains(r, e.br());//r.contains(e.br());

	double ea = e.area();
	double ra = r.area();

	if ( !etl && !ebr && !rtl && !rbr ) {
		Point2d rc = Point2d((-r.width/2.0) + r.x, (-r.height/2.0) + r.y);
		Point2d ec = Point2d((-e.width/2.0) + e.x, (-e.height/2.0) + e.y);
		double dist = sqrt( (rc.x - ec.x)*(rc.x - ec.x) + (rc.y - ec.y)*(rc.y - ec.y));

		double areaCorrelation = 1 - (abs(ea-ra)/(ea+ra));
		double distCorrelation = sqrt(min(ea, ra)) / dist;
		overlapRatio = areaCorrelation * 0.50 + distCorrelation * 0.50;
		return OVERLAP_TYPE_NONE;
	}

	if ( etl && ebr ) {
		overlapRatio = ra/ea;
		return OVERLAP_TYPE_CONTAINED;
	}

	if ( rtl && rbr ) {
		overlapRatio = ea/ra;
		return OVERLAP_TYPE_CONTAINS;
	}

	double oa = (r&e).area();
	overlapRatio = oa / (ea + ra - oa);
	return OVERLAP_TYPE_OVERLAPS;
}
