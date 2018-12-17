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

#include <QPushButton>

#include "detectionsettingsdialog.h"
#include "ui_detectionsettingsdialog.h"
#include "videoprocessordetectionsettings.h"

using namespace std;

const QDoubleValidator DetectionSettingsDialog::POSITIVE_DOUBLE(0, 1000000, 3);
const QIntValidator DetectionSettingsDialog::POSITIVE_INTEGER(0, 1000000);

DetectionSettingsDialog::DetectionSettingsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DetectionSettingsDialog)
{
	ui->setupUi(this);
	ui->lineEdit_BackgroundBlendRatio->setValidator(&POSITIVE_DOUBLE);
	ui->lineEdit_ForegroundBlendRatio->setValidator(&POSITIVE_DOUBLE);
	ui->lineEdit_ForegroundOverloadRatio->setValidator(&POSITIVE_DOUBLE);
	ui->lineEdit_blendingThreshold->setValidator(&POSITIVE_INTEGER);
	ui->lineEdit_blurRadius->setValidator(&POSITIVE_DOUBLE);
	ui->lineEdit_blurStdev->setValidator(&POSITIVE_DOUBLE);
	ui->lineEdit_borderWidth->setValidator(&POSITIVE_INTEGER);
	ui->lineEdit_detectionThreshold->setValidator(&POSITIVE_INTEGER);
	ui->lineEdit_detectionTimeout->setValidator(&POSITIVE_INTEGER);
	ui->lineEdit_dilateBlendingFactor->setValidator(&POSITIVE_DOUBLE);
	ui->lineEdit_dilateDetectionFactor->setValidator(&POSITIVE_DOUBLE);
	ui->lineEdit_entityTimeout->setValidator(&POSITIVE_INTEGER);
	ui->lineEdit_thresholdTimeout->setValidator(&POSITIVE_INTEGER);

}

DetectionSettingsDialog::~DetectionSettingsDialog()
{
	delete ui;
}

void DetectionSettingsDialog::invokeDialog(const shared_ptr<cvqm::VideoProcessorDetectionSettings> settings) {
	cvqm::VideoProcessorDetectionSettings *s = settings.get();
	load(s);
	this->show();
}


double DetectionSettingsDialog::doubleFrom(QLineEdit *edit) {
	bool ok = false;
	double result = edit->text().toDouble(&ok);
	if ( !ok )
		result = 0;
	return result;
}

float DetectionSettingsDialog::floatFrom(QLineEdit *edit) {
	bool ok = false;
	float result = edit->text().toFloat(&ok);
	if ( !ok )
		result = 0;
	return result;
}

unsigned long DetectionSettingsDialog::longFrom(QLineEdit *edit) {
	bool ok = false;
	long result = edit->text().toLong(&ok);
	if ( !ok )
		result = 0;
	return static_cast<unsigned long>(result);
}

int DetectionSettingsDialog::intFrom(QLineEdit *edit) {
	bool ok = false;
	int result = edit->text().toInt(&ok);
	if ( !ok )
		result = 0;
	return result;
}

void DetectionSettingsDialog::handleButton(QAbstractButton *button) {
	if ( button == ui->buttonBox->button(QDialogButtonBox::Apply) ) {
		apply();
	} else if ( button == ui->buttonBox->button(QDialogButtonBox::Reset) ) {
		cvqm::VideoProcessorDetectionSettings settings;
		load(&settings);
	}
}

void DetectionSettingsDialog::load(cvqm::VideoProcessorDetectionSettings *s) {
	ui->lineEdit_BackgroundBlendRatio->setText(QString::fromStdString(to_string(s->background_blend_ratio)));
	ui->lineEdit_ForegroundBlendRatio->setText(QString::fromStdString(to_string(s->foreground_blend_ratio)));
	ui->lineEdit_ForegroundOverloadRatio->setText(QString::fromStdString(to_string(s->foreground_overload_level)));
	ui->lineEdit_blendingThreshold->setText(QString::fromStdString(to_string(s->blending_threshold)));
	ui->lineEdit_blurRadius->setText(QString::fromStdString(to_string(s->blur_radius)));
	ui->lineEdit_blurStdev->setText(QString::fromStdString(to_string(s->blur_stdev)));
	ui->lineEdit_borderWidth->setText(QString::fromStdString(to_string(s->borderWidth)));;
	ui->lineEdit_detectionThreshold->setText(QString::fromStdString(to_string(s->detection_threshold)));
	ui->lineEdit_detectionTimeout->setText(QString::fromStdString(to_string(s->detection_timeout)));
	ui->lineEdit_dilateBlendingFactor->setText(QString::fromStdString(to_string(s->dilateBlendingFactor)));
	ui->lineEdit_dilateDetectionFactor->setText(QString::fromStdString(to_string(s->dilateDetectionFactor)));
	ui->lineEdit_entityTimeout->setText(QString::fromStdString(to_string(s->entity_timeout)));
	ui->lineEdit_thresholdTimeout->setText(QString::fromStdString(to_string(s->threshold_timeout)));
}

void DetectionSettingsDialog::apply() {
	auto *s = new cvqm::VideoProcessorDetectionSettings();
	s->background_blend_ratio = floatFrom(ui->lineEdit_BackgroundBlendRatio);
	s->foreground_blend_ratio = floatFrom(ui->lineEdit_ForegroundBlendRatio);
	s->foreground_overload_level = floatFrom(ui->lineEdit_ForegroundOverloadRatio);
	s->blending_threshold = intFrom(ui->lineEdit_blendingThreshold);
	s->blur_radius = intFrom(ui->lineEdit_blurRadius);
	s->blur_stdev = doubleFrom(ui->lineEdit_blurStdev);
	s->borderWidth = intFrom(ui->lineEdit_borderWidth);
	s->detection_threshold = intFrom(ui->lineEdit_detectionThreshold);
	s->detection_timeout = longFrom(ui->lineEdit_detectionTimeout);
	s->dilateBlendingFactor = intFrom(ui->lineEdit_dilateBlendingFactor);
	s->dilateDetectionFactor = intFrom(ui->lineEdit_dilateDetectionFactor);
	s->entity_timeout = longFrom(ui->lineEdit_entityTimeout);
	s->threshold_timeout = longFrom(ui->lineEdit_thresholdTimeout);

	applySettings(shared_ptr<cvqm::VideoProcessorDetectionSettings>(s));
}

void DetectionSettingsDialog::accept() {
	apply();
	QDialog::accept();
}
