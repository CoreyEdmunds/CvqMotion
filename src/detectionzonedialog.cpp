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

#include <QDoubleValidator>
#include <string>

#include "detectionzonedialog.h"
#include "ui_detectionzonedialog.h"

using namespace std;

const QDoubleValidator DetectionZoneDialog::ANGLE_VALIDATOR(0.0, 360.0, 2);
const QDoubleValidator DetectionZoneDialog::PPM_VALIDATOR(0.0, numeric_limits<double>::max(), 2);

DetectionZoneDialog::DetectionZoneDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DetectionZoneDialog)
{
	ui->setupUi(this);
	ui->lineEdit_AcceptanceAngle->setValidator(&ANGLE_VALIDATOR);
	ui->lineEdit_Direction->setValidator(&ANGLE_VALIDATOR);
	ui->lineEdit_PixelsPerMeter->setValidator(&PPM_VALIDATOR);
}

void DetectionZoneDialog::invokeDialog(int x, int y, int w, int h)
{
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
	this->show();
}

DetectionZoneDialog::~DetectionZoneDialog()
{
	delete ui;
}

void DetectionZoneDialog::accept()
{
	double pixelsPerMeter = ui->lineEdit_PixelsPerMeter->text().toDouble();
	double direction = ui->lineEdit_Direction->text().toDouble();
	double acceptanceAngle = ui->lineEdit_AcceptanceAngle->text().toDouble();
	QString name = ui->lineEdit_ZoneName->text();
	bool directional = ui->checkBox_Directional->checkState();

	newDetectionZone(name.toStdString(), x, y, w, h, pixelsPerMeter, directional, direction, acceptanceAngle);
	QDialog::accept();
}


void DetectionZoneDialog::updatePixelsPerMeter(double ppm)
{
	if ( fpclassify(ppm) != FP_NORMAL )
		ppm = 0;
	ui->lineEdit_PixelsPerMeter->setText(QString::fromStdString(to_string(ppm)));
}
