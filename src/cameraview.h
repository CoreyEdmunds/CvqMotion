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

#ifndef CAMERAVIEW_H
#define CAMERAVIEW_H

#include <QGraphicsView>
#include <QGraphicsPixmapItem>

#include "videoprocessorcontroller.h"

namespace cvqm {
	class CameraView;
}
using namespace cvqm;

class cvqm::CameraView : public QGraphicsView
{
	Q_OBJECT
private:
	enum CameraViewMode {
		NONE = 0,
		DETECTION_ZONE,
		MASK_ZONE,
		MEASURE_DISTANCE,
		REMOVE_ZONE
	};

	static const QPen MASK_ZONE_PEN;
	static const QPen DETECT_ZONE_PEN;
	static const QPen MEASUREMENT_PEN;

	CameraViewMode currentMode = NONE;
	CameraViewMode pressedMode = NONE;
	QGraphicsScene scene;
	QGraphicsItem *background = nullptr;
	QGraphicsItem *foreground = nullptr;
	QLine measurementLine;
	QRect selectionArea;

	QPoint convertAndClamp(QPoint pt);
	void getRectangleCoordinates(int &x, int &y, int &w, int &h);

	void updateForegroundScene();
	void addMaskZone();
	void addDetectionZone();
	void removeZones();
	void performMeasure();

public:
	explicit CameraView(QWidget *parent = nullptr);

	void setVideoProcessorController(cvqm::VideoProcessorController *p);

	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;

signals:
	void newIgnoreZone(int x, int y, int w, int h);
	void newDetectionZone(int x, int y, int w, int h);
	void deleteZonesAt(int x, int y);
	void newMeasurement(double pixelLength);

public slots:
	void newImage(QImage img);
	void actionAdd_Detection_Zone_Toggled(bool val);
	void actionAdd_Mask_Zone_Toggled(bool val);
	void actionMeasure_Distance_Toggled(bool val);
	void actionRemove_Zone_Toggled(bool val);
};

#endif // CAMERAVIEW_H
