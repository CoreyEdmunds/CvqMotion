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

#ifndef MEASUREMENTDIALOG_H
#define MEASUREMENTDIALOG_H

#include <QDialog>
#include <QDoubleValidator>

namespace Ui {
class MeasurementDialog;
}

class MeasurementDialog : public QDialog
{
	Q_OBJECT
private:
	double pixelLength = 0;
	double metersLength = 0;
	double ppm = 0;

	static const QDoubleValidator LINE_EDIT_VALIDATOR;
	Ui::MeasurementDialog *ui;

	void updateCalculation();

public:
	explicit MeasurementDialog(QWidget *parent = nullptr);
	virtual ~MeasurementDialog() override;

	void reset(double pixels = 0);
	void setPixelLength(double pixelLength);
	void setMetersLength(double meters);

signals:
	void lengthComputed(double length);

public slots:
	void invokeDialog(double pixelLength);
	void lineEdit_LengthInMeters_TextChanged(QString text);
	void accept() override;
};

#endif // MEASUREMENTDIALOG_H
