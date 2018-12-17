# CvqMotion

A motion detection application written with the objective of measuring the apparent speed of road traffic from an overhead perspective using a webcam.  A variety of features are implemented in a cohesive graphical user interface written in C++ using Qt and OpenCV.

The application uses a simple background-difference-threshold algorithm to detect areas in video frames that contain motion.  The areas that contain motion are correlated and tracked as moving entities, and are labeled with unique identifiers.  Dead reckoning of entity movement is used to estimate or predict where occluded entities may reappear.  Velocities are estimated based on motion history, and are logged when an entity enters a user-defined detection zone.  

Detection zones are configured with a conversion factor that allows a speed in km/h (as well as a direction, in cardinal degrees relative to the camera view) to be logged.  The detection zones can be configured to only accept from certain directions (e.g. 'westbound', 'eastbound'), configured as cardinal degrees relative to the image frame.

A measurement tool is incorporated to facilitate the calculation of the pixels-per-meter conversion factor that is needed by the detection zones.  Objects of known length can be measured using this tool (e.g. a traffic lane dash mark) to calculate the value.

## Screenshots
![Screenshot](https://i.imgur.com/I07LUSu.jpg "Screenshot")

## Building
In project directory, perform the following commands:
```
cd build/
qmake -qt5 ../
make -j4
```

## Running
In project directory, run the build/QcvMotion executable.


## License
This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
