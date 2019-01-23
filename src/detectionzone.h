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

#ifndef DETECTIONZONE_H
#define DETECTIONZONE_H

#include <opencv2/opencv.hpp>
#include <string>

namespace cvqm {
	class DetectionZone;
}

class cvqm::DetectionZone
{
public:
	const std::string name;
	const bool directional;
	const cv::Rect zone;
	const double pixelsPerMeter;
	const double acceptAngle;
	const double acceptWidth;

	DetectionZone(std::string name, cv::Rect zone, double pixelsPerMeter, bool directional, double acceptAngle, double acceptWidth);
	bool acceptableAngle(double angle);
};


#endif // DETECTIONZONE_H
