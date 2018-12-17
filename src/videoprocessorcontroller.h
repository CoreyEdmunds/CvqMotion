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

#ifndef VIDEOPROCESSORCONTROLLER_H
#define VIDEOPROCESSORCONTROLLER_H

#include <QThread>
#include <QImage>
#include <memory>
#include <QPixmap>

#include "videoprocessor.h"

namespace cvqm {
	class VideoProcessorController;
}

class cvqm::VideoProcessorController : public QObject, cvqm::OutputImageObserver, cvqm::DetectionObserver
{
	Q_OBJECT
private:
	class VideoProcessorWorkerThread : QThread {
		friend VideoProcessorController;
		cvqm::VideoProcessor *processor;
		VideoProcessorController *controller;
		VideoProcessorWorkerThread(cvqm::VideoProcessor *p, cvqm::VideoProcessorController *c);
		void run() override;
	};

	cvqm::VideoProcessor p;
	VideoProcessorWorkerThread *runThread = nullptr;
	QImage matToQImage(const cv::Mat *image);

public:
	VideoProcessorController();
	virtual ~VideoProcessorController() override;
	void detected(cvqm::DetectionZone *zone, cvqm::Entity *e, cv::Mat &frame) override;
	void renderedImage(const cv::Mat *image) override;


public slots:
	void start(int deviceId, int xRes, int yRes);
	void stop();

	void setShowOriginal(bool value);
	void setShowBlur(bool value);
	void setShowDelta(bool value);
	void setShowThreshold(bool value);
	void setShowBackground(bool value);
	void setShowDilated(bool value);
	void setShowDilatedBlending(bool value);
	void setShowOutput(bool value);

	void newMaskZone(int x, int y, int w, int h);
	void newDectionZone(const std::string name, int x, int y, int w, int h, double pixelsPerMeter, bool directional, double acceptAngle, double acceptWidth);
	void deleteZonesAt(int x, int y);
	void requestDetectionSettingsDialog();
	void applyDetectionSettings(const std::shared_ptr<cvqm::VideoProcessorDetectionSettings> settings);


signals:
	void runStateChanged(bool running);
	void newImage(QImage img);
	void newDetection(QString zone, double dir, double vel, QImage image);
	void invokeDetectionSettingsDialog(std::shared_ptr<cvqm::VideoProcessorDetectionSettings> settings);
	void runFailure(QString reason);

};

#endif // VIDEOPROCESSORCONTROLLER_H
