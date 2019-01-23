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
#include <cmath>

#include "measurementdialog.h"
#include "ui_measurementdialog.h"

using namespace std;

const QDoubleValidator MeasurementDialog::LINE_EDIT_VALIDATOR(-1000000, 1000000, 10);

MeasurementDialog::MeasurementDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::MeasurementDialog)
{
	ui->setupUi(this);
	this->ui->lineEdit_LengthMeters->setValidator(&LINE_EDIT_VALIDATOR);
}

void MeasurementDialog::reset(double pixelLength)
{
	setPixelLength(pixelLength);
	setMetersLength(0);
}

void MeasurementDialog::setMetersLength(double meters)
{
		this->metersLength = meters;
		this->ui->lineEdit_LengthMeters->setText(QString::fromStdString(to_string(meters)));
}

void MeasurementDialog::setPixelLength(double pixels)
{
		this->pixelLength = pixels;
		this->ui->lineEdit_LengthPixels->setText(QString::fromStdString(to_string(pixels)));
}

void MeasurementDialog::updateCalculation()
{
	this->ppm = pixelLength / metersLength;

	int classification = fpclassify(this->ppm);
	if ( classification == FP_NORMAL || classification == FP_ZERO ) {
		this->ui->lineEdit_PixelsPerMeter->setText(QString::fromStdString(to_string(this->ppm)));
	} else {
		this->ui->lineEdit_PixelsPerMeter->setText("0");
	}
}

void MeasurementDialog::lineEdit_LengthInMeters_TextChanged(QString text)
{
	bool ok;
	double result = text.toDouble(&ok);
	if ( ok ) {
		this->metersLength = result;
		updateCalculation();
	}
}

void MeasurementDialog::invokeDialog(double pixelLength)
{
	reset(pixelLength);
	this->show();
}

void MeasurementDialog::accept()
{
	lengthComputed(this->ppm);
	QDialog::accept();
}

MeasurementDialog::~MeasurementDialog()
{
	delete ui;
}
