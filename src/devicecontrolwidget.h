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

#ifndef DEVICECONTROLWIDGET_H
#define DEVICECONTROLWIDGET_H

#include <QWidget>
#include <QMessageBox>
#include <QIntValidator>

namespace Ui {
class DeviceControlWidget;
}

class DeviceControlWidget : public QWidget
{
	Q_OBJECT
private:
	bool runState = false;
	QMessageBox *errorMsgbox;
	QIntValidator *validator;
	void setControlsEnabled(bool state);
	Ui::DeviceControlWidget *ui;

public:
	explicit DeviceControlWidget(QWidget *parent = nullptr);
	~DeviceControlWidget();

signals:
	void start(int cameraId, int xRes, int yRes);
	void stop();

public slots:
	void startButtonClicked();
	void runStateChanged(bool state);
	void runFailure(QString reason);

};


#endif // DEVICECONTROLWIDGET_H
