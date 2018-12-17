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

#ifndef VIDEOPROCESSORCONSTANTS_H
#define VIDEOPROCESSORCONSTANTS_H

#include <string>

namespace cvqm {
	static constexpr int MASK_ZONE_COLOUR_R = 128;
	static constexpr int MASK_ZONE_COLOUR_G = 128;
	static constexpr int MASK_ZONE_COLOUR_B = 128;
	static constexpr int DETECTION_ZONE_COLOUR_R = 0;
	static constexpr int DETECTION_ZONE_COLOUR_G = 255;
	static constexpr int DETECTION_ZONE_COLOUR_B = 255;

	static constexpr char ORIGINAL_INPUT[] = "Original Input";
	static constexpr char BLURRED_INPUT[] = "Blurred Input";
	static constexpr char BACKGROUND_DIFFERENCE[] = "Input Difference";
	static constexpr char THRESHOLDED_DELTA[] = "Thresholded Difference";
	static constexpr char BACKGROUND_FRAME[] = "Background Average";
	static constexpr char DILATED_THRESHOLD[] = "Dilated Foreground";
	static constexpr char LABELED_OUTPUT[] = "Labeled Output";
	static constexpr char DILATED_BLENDING_THRESHOLD[] = "Dilated Blending Threshold";
}
#endif // VIDEOPROCESSORCONSTANTS_H
