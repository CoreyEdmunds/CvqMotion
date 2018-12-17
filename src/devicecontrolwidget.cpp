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

#include <QString>
#include <QVariant>
#include <string>

#include "devicecontrolwidget.h"
#include "ui_devicecontrolwidget.h"

using namespace std;

DeviceControlWidget::DeviceControlWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::DeviceControlWidget)
{
	ui->setupUi(this);
	this->errorMsgbox = new QMessageBox();
	errorMsgbox->setModal(true);

	this->validator = new QIntValidator(0, 10000, this);
	ui->lineEdit_XResolution->setValidator(this->validator);
	ui->lineEdit_YResolution->setValidator(this->validator);

	ui->lineEdit_XResolution->setText("640");
	ui->lineEdit_YResolution->setText("480");

	for(int i=1; i<11; i++)  // todo populate from device scan
		ui->comboBox_InputDevice->addItem(QString::fromStdString(to_string(i)));
}

DeviceControlWidget::~DeviceControlWidget()
{
	delete this->errorMsgbox;
	delete this->validator;
	delete ui;
}

void DeviceControlWidget::startButtonClicked() {
	bool deviceIdOk = false;
	int deviceId = ui->comboBox_InputDevice->currentText().toInt(&deviceIdOk) - 1;
	bool xResOk = false;
	int xRes = ui->lineEdit_XResolution->text().toInt(&xResOk);
	bool yResOk = false;
	int yRes = ui->lineEdit_YResolution->text().toInt(&yResOk);

	if ( !(deviceIdOk && xResOk && yResOk) ) {
		string error = "Invalid parameter: ";
		if ( !deviceIdOk )
			error += "Device_ID";
		if ( !xResOk )
			error += " X_Resolution ";
		if ( !yResOk )
			error += " Y_Resolution";
		errorMsgbox->setText(QString::fromStdString(error));
		errorMsgbox->show();
		return;
	}

	setControlsEnabled(false);
	ui->pushButton_StartStop->setEnabled(false);

	if ( runState )
		stop();
	else
		start(deviceId, xRes, yRes);
}

void DeviceControlWidget::setControlsEnabled(bool state) {
	ui->comboBox_InputDevice->setEnabled(state);
	ui->lineEdit_XResolution->setEnabled(state);
	ui->lineEdit_YResolution->setEnabled(state);
}

void DeviceControlWidget::runStateChanged(bool state) {
	setControlsEnabled(!state);
	if ( state )
		ui->pushButton_StartStop->setText("Stop");
	else
		ui->pushButton_StartStop->setText("Start");
	ui->pushButton_StartStop->setEnabled(true);
	this->runState = state;
}

void DeviceControlWidget::runFailure(QString reason) {
	errorMsgbox->setText(reason);
	errorMsgbox->show();
	ui->pushButton_StartStop->setEnabled(true);
}

