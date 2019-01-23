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

#include <cmath>

#include "entity.h"

using namespace cvqm;
using namespace std;
using namespace cv;

Entity::Entity(Rect &box, ulong lastUpdateFrameId)
{
	//this->id = id;
	this->box = box;
	this->lastUpdateFrameId = lastUpdateFrameId;
}

Entity::~Entity()
{
	for(auto it: bbHistory) {
		delete it;
	}
}

Rect Entity::deadRecon(double frameTime)
{
	double dt;
	if ( bbHistory.empty() )
		dt = 0;
	else
		dt = frameTime - bbHistory.front()->first;

	return Rect(box.x + static_cast<int>(vel[0]*dt),
			box.y + static_cast<int>(vel[1]*dt), box.width, box.height);
}

void Entity::assignId(ulong id)
{
	this->id = id;
}

void Entity::calculateVelocity(Rect &r1, Rect &r2, double velocity[], double dt, double multiplier)
{
	double dx = (r1.x + r1.width*0.5) - (r2.x + r2.width*0.5);
	double dy = (r1.y + r1.height*0.5) - (r2.y + r2.height*0.5);
	velocity[0] = dx/dt * multiplier;
	velocity[1] = dy/dt * multiplier;
}

double Entity::getBearingRadians()
{
	double bearing = atan2(this->vel[1],this->vel[0]);
	if ( bearing < 0 )
		bearing += 2 * M_PI;
	return bearing;
}

void Entity::calculateVelocityBearing(double &vel, double &bearing, double pixelsPerMeter)
{
	vel = sqrt(this->vel[0]*this->vel[0] + this->vel[1]*this->vel[1]);
	vel /= pixelsPerMeter;
	vel *= 3.6;  // m/s -> km/h
	bearing = (atan2(this->vel[1],this->vel[0]) + 0.5*M_PI) / M_PI * 180.0;
	if ( bearing < 0 )
		bearing += 360.0;
}

void Entity::update(Rect *newBox, ulong frameId, double frameTime)
{
	pair<double, Rect>* newer = new pair<double, Rect>(frameTime, *newBox);
	this->box = *newBox;
	this->bbHistory.push_front(newer);
	this->vel[0] = 0;
	this->vel[1] = 0;

	auto it=bbHistory.begin();
	it++;
	int count=0;
	for(; bbHistory.end() != it && count++ < 10; ++it) {
		double velocity[2];
		calculateVelocity(newer->second, (*it)->second, velocity, newer->first - (*it)->first, 1.0/10);
		this->vel[0] += velocity[0];
		this->vel[1] += velocity[1];
		newer = *it;
	}
	this->lastUpdateFrameId = frameId;
}

string Entity::str()
{
	ostringstream str;
	str << "E" << " " << id << " " << box;
	return str.str();
}
