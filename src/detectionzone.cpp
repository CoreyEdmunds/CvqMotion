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

#include <stdexcept>
#include <string>

#include "detectionzone.h"

using namespace std;
using namespace cvqm;

DetectionZone::DetectionZone(string name, cv::Rect zone, double pixelsPerMeter, bool directional, double acceptAngle, double acceptWidth) :
	name(name),
	directional(directional),
	zone(zone),
	pixelsPerMeter(pixelsPerMeter),
	acceptAngle(acceptAngle),
	acceptWidth(acceptWidth)
{
	if ( acceptAngle >= M_PI*2 || acceptAngle < 0)
		throw out_of_range("DetectionZone::DetectionZone: acceptAngle out of range");
	if ( acceptWidth > M_PI*2 || acceptWidth < 0)
		throw out_of_range("DetectionZone::DetectionZone: acceptWidth out of range");
}

bool DetectionZone::acceptableAngle(double angle) {
	double minAngle = acceptAngle + M_PI*2.0 - 0.5 * acceptWidth;
	double maxAngle = acceptAngle + M_PI*2.0 + 0.5 * acceptWidth;
	angle += M_PI*2.0;
	return angle > minAngle && angle < maxAngle;
}
