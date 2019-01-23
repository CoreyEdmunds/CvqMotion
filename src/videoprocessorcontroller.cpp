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

#include <QPixmap>

#include "videoprocessorcontroller.h"

using namespace cvqm;
using namespace std;
using namespace cv;

VideoProcessorController::VideoProcessorWorkerThread::VideoProcessorWorkerThread(VideoProcessor *p, VideoProcessorController *c)
{
	this->setObjectName("VideoProcessorWorkerThread");
	this->processor = p;
	this->controller = c;
}

void VideoProcessorController::VideoProcessorWorkerThread::run()
{
	//cout << "Starting video processor" << endl;
	this->controller->runStateChanged(true);
	try {
		this->processor->run();
	} catch (const exception& e) {
		stringstream err;
		err << "Caught exception in VideoProcessor::run(): " << e.what();
		this->controller->runFailure(QString::fromStdString(err.str()));
	} catch (...) {
		this->controller->runFailure(QString("Caught exception in VideoProcessor::run(): catch-all"));
	}
	this->controller->runStateChanged(false);
}


VideoProcessorController::VideoProcessorController()
{
	p.detectionObserver = this;
	p.outputImageObserver = this;
}

VideoProcessorController::~VideoProcessorController() {
	if ( this->runThread ) {
		this->p.requestShutdown();
		this->runThread->wait(5 * 1000);
		delete this->runThread;
	}
}

QImage VideoProcessorController::matToQImage(const Mat *image)
{
	int channels = image->channels();

	assert(channels == 3);

	int size = channels * image->rows * image->cols;
	uchar *data = new uchar[size];

	for(int i=0; i < size; i+=channels) {
		data[i] = image->data[i+2];
		data[i+1] = image->data[i+1];
		data[i+2] = image->data[i];
	}

	return QImage(data, image->cols, image->rows, static_cast<int>(image->step), QImage::Format_RGB888,
			   [](void *data){
								delete reinterpret_cast<uchar*>(data);
				}, data);
}

void VideoProcessorController::detected(cvqm::DetectionZone *zone, cvqm::Entity *e, Mat &frame)
{
	double dir, vel;
	e->calculateVelocityBearing(vel, dir, zone->pixelsPerMeter);

	Mat image(frame, e->box);
	Mat image1(e->box.height, e->box.width, CV_8UC3); // matrix stride issues todo
	image.copyTo(image1);

	newDetection(QString::fromStdString(zone->name), dir, vel, matToQImage(&image1));
}

void VideoProcessorController::renderedImage(const Mat *image)
{
	this->newImage(matToQImage(image));
}

void VideoProcessorController::stop()
{
	if ( this->runThread && this->runThread->isRunning() )
		this->p.requestShutdown(true);
}

void VideoProcessorController::start(int deviceId, int xRes, int yRes)
{
	if ( !this->runThread || !this->runThread->isRunning() ) {
		p.setDeviceId(deviceId);
		p.setResolution(xRes, yRes);

		delete this->runThread;
		this->p.requestShutdown(false);
		this->runThread = new VideoProcessorWorkerThread(&this->p, this);
		this->runThread->start();
	}
}

void VideoProcessorController::newMaskZone(int x, int y, int w, int h)
{
	this->p.addMaskZone(Rect(x, y, w, h));
}

void VideoProcessorController::newDectionZone(const string name, int x, int y, int w, int h, double pixelsPerMeter, bool directional, double acceptAngle, double acceptWidth)
{
	acceptAngle = acceptAngle * M_PI / 180.0 - 0.5*M_PI;
	if ( acceptAngle < 0 )
		acceptAngle += 2*M_PI;
	acceptWidth = acceptWidth * M_PI / 180.0;
	this->p.addDetectionZone(DetectionZone(name, Rect(x, y, w, h), pixelsPerMeter, directional, acceptAngle, acceptWidth));
}

void VideoProcessorController::deleteZonesAt(int x, int y)
{
	this->p.removeDetectionZones([x, y](DetectionZone *z){
			return VideoProcessor::contains(z->zone, Point2i(x,y));
	});
	this->p.removeMaskZones([x, y](Rect *z){
			return VideoProcessor::contains(*z, Point2i(x,y));
	});
}

void VideoProcessorController::requestDetectionSettingsDialog()
{
	shared_ptr<cvqm::VideoProcessorDetectionSettings> settings(this->p.getCurrentConfiguration());
	this->invokeDetectionSettingsDialog(settings);
}

void VideoProcessorController::applyDetectionSettings(const shared_ptr<cvqm::VideoProcessorDetectionSettings> settings)
{
	this->p.setCurrentConfiguration(settings.get());
}

void VideoProcessorController::setShowOriginal(bool value)
{
	p.showOriginal.store(value);
}

void VideoProcessorController::setShowBlur(bool value)
{
	p.showBlur.store(value);
}

void VideoProcessorController::setShowDelta(bool value)
{
	p.showDelta.store(value);
}

void VideoProcessorController::setShowThreshold(bool value)
{
	p.showThreshold.store(value);
}

void VideoProcessorController::setShowBackground(bool value)
{
	p.showBackground.store(value);
}

void VideoProcessorController::setShowDilated(bool value)
{
	p.showDilated.store(value);
}

void VideoProcessorController::setShowDilatedBlending(bool value)
{
	p.showDilatedBlending.store(value);
}

void VideoProcessorController::setShowOutput(bool value)
{
	p.showOutput.store(value);
}
