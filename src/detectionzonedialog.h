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

#ifndef DETECTIONZONEDIALOG_H
#define DETECTIONZONEDIALOG_H

#include <QDialog>
#include <string>

class QDoubleValidator;

namespace Ui {
	class DetectionZoneDialog;
}

class DetectionZoneDialog : public QDialog
{
	Q_OBJECT
private:
	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;

	static const QDoubleValidator ANGLE_VALIDATOR;
	static const QDoubleValidator PPM_VALIDATOR;

	Ui::DetectionZoneDialog *ui;

public:
	explicit DetectionZoneDialog(QWidget *parent = nullptr);
	virtual ~DetectionZoneDialog() override;

signals:
	void newDetectionZone(std::string name, int x, int y, int w, int h, double directional, double pixelsPerMeter, double acceptAngle, double acceptWidth);

public slots:
	void invokeDialog(int x, int y, int w, int h);
	void updatePixelsPerMeter(double ppm);
	void accept() override;
};

#endif // DETECTIONZONEDIALOG_H
