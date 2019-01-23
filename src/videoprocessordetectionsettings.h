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

#ifndef VIDEOPROCESSORDETECTIONSETTINGS_H
#define VIDEOPROCESSORDETECTIONSETTINGS_H

namespace cvqm {
	struct VideoProcessorDetectionSettings;
}

struct cvqm::VideoProcessorDetectionSettings {
	int blur_radius = 13;
	double blur_stdev = 1.5;
	int blending_threshold = 12;
	int detection_threshold = 20;
	unsigned long threshold_timeout = 25 * 5;
	unsigned long entity_timeout = 25 * 3;
	unsigned long detection_timeout = 25 * 3;
	float background_blend_ratio = 0.15f;
	float foreground_blend_ratio = 0.1f;
	float foreground_overload_level = 0.5f;
	int dilateDetectionFactor = 7;
	int dilateBlendingFactor = 11;
	int borderWidth = 20;
	bool greyscale = true;
};

#endif // VIDEOPROCESSORDETECTIONSETTINGS_H
