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

#ifndef ENTITY_H
#define ENTITY_H

#include <list>
#include <map>
#include <opencv2/opencv.hpp>

#include "detectionzone.h"

namespace cvqm {
	class Entity;
}

class cvqm::Entity
{
public:
	std::list<std::pair<double, cv::Rect>*> bbHistory;
	std::map<DetectionZone *, ulong> detections;

	ulong id = 0;
	ulong lastUpdateFrameId;
	double vel[2] = {0, 0};
	cv::Rect box;
	ulong drFrameId = 0;
	cv::Rect dr;

	Entity(cv::Rect &box, ulong lastUpdateFrameId);
	~Entity();

	cv::Rect deadRecon(double frameTime);
	void assignId(ulong id);
	void calculateVelocityBearing(double &vel, double &bearing, double pixelsPerMeter);
	double getBearingRadians();
	void update(cv::Rect *newBox, ulong frameId, double frameTime);
	std::string str();

	private:
	void calculateVelocity(cv::Rect &r1, cv::Rect &r2, double velocity[], double dt, double multiplier);
};


#endif // ENTITY_H
