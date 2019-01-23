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

#include <QMouseEvent>
#include <QAction>
#include <sstream>
#include <exception>
#include <algorithm>

#include "videoprocessor.h"
#include "videoprocessorconstants.h"
#include "cameraview.h"

using namespace std;
using namespace cvqm;

const QPen CameraView::MASK_ZONE_PEN =
	QPen(QBrush(QColor(MASK_ZONE_COLOUR_B, MASK_ZONE_COLOUR_G,MASK_ZONE_COLOUR_R),
				Qt::BrushStyle::SolidPattern), 1, Qt::PenStyle::DashLine);
const QPen CameraView::DETECT_ZONE_PEN =
	QPen(QBrush(QColor(DETECTION_ZONE_COLOUR_R, DETECTION_ZONE_COLOUR_G, DETECTION_ZONE_COLOUR_B),
				Qt::BrushStyle::SolidPattern), 1, Qt::PenStyle::DashLine);
const QPen CameraView::MEASUREMENT_PEN = QPen(QColor(255,255,255));

CameraView::CameraView(QWidget *parent) : QGraphicsView(parent)
{
	this->setScene(&this->scene);
	this->setDragMode(DragMode::RubberBandDrag);
}

void CameraView::newImage(QImage img)
{
	if ( this->background ) {
		this->scene.removeItem(this->background);
		delete this->background;
	}
	this->background = this->scene.addPixmap(QPixmap::fromImage(img));
	updateForegroundScene();
}

void CameraView::updateForegroundScene()
{
	if ( this->foreground ) {
		this->scene.removeItem(this->foreground);
		delete this->foreground;
		this->foreground = nullptr;
	}

	switch(this->pressedMode) {
	case CameraViewMode::DETECTION_ZONE:
		this->foreground = this->scene.addRect(selectionArea, DETECT_ZONE_PEN);
		break;
	case CameraViewMode::MASK_ZONE:
		this->foreground = this->scene.addRect(selectionArea, MASK_ZONE_PEN);
		break;
	case CameraViewMode::MEASURE_DISTANCE:
		this->foreground = this->scene.addLine(measurementLine, MEASUREMENT_PEN);
		break;
	case CameraViewMode::REMOVE_ZONE:
		break;
	case CameraViewMode::NONE:
		break;
	default:
		stringstream ss;
		ss << "CameraView::updateRender:  Invalid CameraViewMode " << this->pressedMode;
		throw invalid_argument(ss.str());
	}
}

void CameraView::mousePressEvent(QMouseEvent *event)
{
	QPoint pt = convertAndClamp(event->pos());

	this->measurementLine.setP1(pt);
	this->measurementLine.setP2(pt);
	this->selectionArea.setTopLeft(pt);
	this->selectionArea.setBottomRight(pt);

	this->pressedMode = this->currentMode;
	updateForegroundScene();

	//cout << " CameraView::mousePressEvent X=" << pt.x() << " Y=" << pt.y()  << endl;
}

void CameraView::getRectangleCoordinates(int &x, int &y, int &w, int &h)
{
	x = min(this->selectionArea.topLeft().x(), this->selectionArea.bottomRight().x());
	y = min(this->selectionArea.topLeft().y(), this->selectionArea.bottomRight().y());
	w = max(this->selectionArea.topLeft().x(), this->selectionArea.bottomRight().x()) - x;
	h = max(this->selectionArea.topLeft().y(), this->selectionArea.bottomRight().y()) - y;
};

void CameraView::addMaskZone()
{
	int x, y, w, h;
	getRectangleCoordinates(x, y, w, h);
	newIgnoreZone(x, y, w, h);
}

void CameraView::addDetectionZone()
{
	int x, y, w, h;
	getRectangleCoordinates(x, y, w, h);
	newDetectionZone(x, y, w, h);
}

void CameraView::removeZones()
{
	int x, y, w, h;
	getRectangleCoordinates(x, y, w, h);
	deleteZonesAt(x, y);
}

void CameraView::performMeasure()
{
	int x, y, w, h;
	getRectangleCoordinates(x, y, w, h);
	newMeasurement(sqrt(w*w + h*h));
}

void CameraView::mouseReleaseEvent(QMouseEvent *event)
{
	Q_UNUSED(event);
	switch(this->pressedMode) {
	case CameraViewMode::DETECTION_ZONE:
		addDetectionZone();
		break;
	case CameraViewMode::MASK_ZONE:
		addMaskZone();
		break;
	case CameraViewMode::MEASURE_DISTANCE:
		performMeasure();
		break;
	case CameraViewMode::REMOVE_ZONE:
		removeZones();
		break;
	case CameraViewMode::NONE:
		break;
	default:
		stringstream ss;
		ss << "CameraView::mouseReleaseEvent:  Invalid CameraViewMode " << this->pressedMode;
		throw invalid_argument(ss.str());
	}

	this->pressedMode = CameraViewMode::NONE;
	updateForegroundScene();

	//cout << " CameraView::mouseReleaseEvent X=" << event->x() << " Y=" << event->y() << endl;
}

QPoint CameraView::convertAndClamp(QPoint pt)
{
	QPointF pt_f = mapToScene(pt);
	double x = pt_f.x();
	x = min(max(0.0, x), this->scene.width() + 0.0);
	double y = pt_f.y();
	y = min(max(0.0, y), this->scene.height() + 0.0);
	return QPoint(static_cast<int>(x), static_cast<int>(y));
}

void CameraView::mouseMoveEvent(QMouseEvent *event)
{
	QPoint pt = convertAndClamp(event->pos());

	this->measurementLine.setP2(pt);
	this->selectionArea.setTopLeft(pt);

	//cout << " CameraView::mouseMoveEvent X=" << pt.x() << " Y=" << pt.y() << " Scene w=" << this->scene.width() << " h=" << this->scene.height() << endl;
}


void CameraView::actionAdd_Detection_Zone_Toggled(bool val)
{
	if ( val )
		this->currentMode = CameraViewMode::DETECTION_ZONE;
	else if ( this->currentMode == CameraViewMode::DETECTION_ZONE )
		this->currentMode = CameraViewMode::NONE;

}

void CameraView::actionAdd_Mask_Zone_Toggled(bool val)
{
	if ( val )
		this->currentMode = CameraViewMode::MASK_ZONE;
	else if ( this->currentMode == CameraViewMode::MASK_ZONE )
		this->currentMode = CameraViewMode::NONE;
}

void CameraView::actionMeasure_Distance_Toggled(bool val)
{
	if ( val )
		this->currentMode = CameraViewMode::MEASURE_DISTANCE;
	else if ( this->currentMode == CameraViewMode::MEASURE_DISTANCE )
		this->currentMode = CameraViewMode::NONE;
}

void CameraView::actionRemove_Zone_Toggled(bool val)
{
	if ( val )
		this->currentMode = CameraViewMode::REMOVE_ZONE;
	else if ( this->currentMode == CameraViewMode::REMOVE_ZONE )
		this->currentMode = CameraViewMode::NONE;
}
