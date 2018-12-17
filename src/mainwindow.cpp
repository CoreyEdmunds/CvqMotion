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

#include <QList>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "videoprocessorcontroller.h"
#include "videoprocessorconstants.h"
#include "cameraview.h"

using namespace cvqm;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	this->p = new VideoProcessorController();

	this->detectionListModel = new DetectionsListTableModel(ui->tableView);
	ui->tableView->setModel(this->detectionListModel);
	ui->tableView->reset();
	connect(this->p, &VideoProcessorController::newDetection, this->detectionListModel, &DetectionsListTableModel::newDetection);
	ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	this->toolGroup = new QActionGroup(this->ui->toolBar);
	this->toolGroup->setExclusive(false);
	this->toolGroup->addAction(this->ui->actionAdd_Detection_Zone);
	this->toolGroup->addAction(this->ui->actionAdd_Mask_Out_Zone);
	this->toolGroup->addAction(this->ui->actionMeasure_Distance);
	this->toolGroup->addAction(this->ui->actionRemove_Zone);
	connect(this->toolGroup, &QActionGroup::triggered, this, &MainWindow::toolgroupExclusive);
	connect(this->ui->actionAdd_Detection_Zone, &QAction::toggled, this->ui->cameraView, &CameraView::actionAdd_Detection_Zone_Toggled);
	connect(this->ui->actionAdd_Mask_Out_Zone, &QAction::toggled, this->ui->cameraView, &CameraView::actionAdd_Mask_Zone_Toggled);
	connect(this->ui->actionMeasure_Distance, &QAction::toggled, this->ui->cameraView, &CameraView::actionMeasure_Distance_Toggled);
	connect(this->ui->actionRemove_Zone, &QAction::toggled, this->ui->cameraView, &CameraView::actionRemove_Zone_Toggled);

	this->measurementDialog = new MeasurementDialog(this);
	connect(this->ui->cameraView, &CameraView::newMeasurement, this->measurementDialog, &MeasurementDialog::invokeDialog);

	this->detectionZoneDialog = new DetectionZoneDialog(this);
	connect(this->ui->cameraView, &CameraView::newDetectionZone, this->detectionZoneDialog, &DetectionZoneDialog::invokeDialog);
	connect(this->detectionZoneDialog, &DetectionZoneDialog::newDetectionZone, this->p, &VideoProcessorController::newDectionZone);
	connect(this->measurementDialog, &MeasurementDialog::lengthComputed, this->detectionZoneDialog, &DetectionZoneDialog::updatePixelsPerMeter);

	this->detectionSettingsDialog = new DetectionSettingsDialog(this);
	connect(this->detectionSettingsDialog, &DetectionSettingsDialog::applySettings, this->p, &VideoProcessorController::applyDetectionSettings);
	connect(this->p, &VideoProcessorController::invokeDetectionSettingsDialog, this->detectionSettingsDialog, &DetectionSettingsDialog::invokeDialog);
	connect(ui->actionMotion_Detection_Parameters, &QAction::triggered, this->p, &VideoProcessorController::requestDetectionSettingsDialog);

	connect(this->ui->cameraView, &CameraView::newIgnoreZone, this->p, &VideoProcessorController::newMaskZone);
	connect(this->ui->cameraView, &CameraView::deleteZonesAt, this->p, &VideoProcessorController::deleteZonesAt);

	configureDebugMenu();

	connect(this->p, &VideoProcessorController::runFailure, ui->deviceControlWidget, &DeviceControlWidget::runFailure);
	connect(this->p, &VideoProcessorController::runStateChanged, ui->deviceControlWidget, &DeviceControlWidget::runStateChanged);
	connect(ui->deviceControlWidget, &DeviceControlWidget::start, this->p, &VideoProcessorController::start);
	connect(ui->deviceControlWidget, &DeviceControlWidget::stop, this->p, &VideoProcessorController::stop);

	connect(this->p, &VideoProcessorController::newImage, this->ui->cameraView, &CameraView::newImage);

	this->aboutDialog = new AboutDialog();
	connect(ui->actionAbout, &QAction::triggered, this->aboutDialog, &AboutDialog::invoke);

	QList<int> sizes;
	sizes.push_back(400);
	sizes.push_back(200);
	ui->splitter->setSizes(sizes);
}

void MainWindow::configureDebugMenu() {
	QString view("View ");
	ui->actionView_Background_Delta->setText(view + BACKGROUND_DIFFERENCE);
	ui->actionView_Blur->setText(view + BLURRED_INPUT);
	ui->actionView_Background->setText(view + BACKGROUND_FRAME);
	ui->actionView_Dilated_Background->setText(view + DILATED_BLENDING_THRESHOLD);
	ui->actionView_Dilated_Foreground->setText(view + DILATED_THRESHOLD);
	ui->actionView_Foreground_Threshold->setText(view + THRESHOLDED_DELTA);
	ui->actionView_Original_Frame->setText(view + ORIGINAL_INPUT);
	ui->actionView_Output->setText(view + LABELED_OUTPUT);
	ui->actionView_Source->setText(view + ORIGINAL_INPUT);

	connect(this->ui->actionView_Background_Delta, &QAction::toggled, this->p, &VideoProcessorController::setShowDelta);
	connect(this->ui->actionView_Blur, &QAction::toggled, this->p, &VideoProcessorController::setShowBlur);
	connect(this->ui->actionView_Background, &QAction::toggled, this->p, &VideoProcessorController::setShowBackground);
	connect(this->ui->actionView_Dilated_Foreground, &QAction::toggled, this->p, &VideoProcessorController::setShowDilated);
	connect(this->ui->actionView_Dilated_Background, &QAction::toggled, this->p, &VideoProcessorController::setShowDilatedBlending);
	connect(this->ui->actionView_Foreground_Threshold, &QAction::toggled, this->p, &VideoProcessorController::setShowThreshold);
	connect(this->ui->actionView_Original_Frame, &QAction::toggled, this->p, &VideoProcessorController::setShowOriginal);
	connect(this->ui->actionView_Output, &QAction::toggled, this->p, &VideoProcessorController::setShowOutput);
	connect(this->ui->actionView_Source, &QAction::toggled, this->p, &VideoProcessorController::setShowOriginal);

}

void MainWindow::toolgroupExclusive(QAction *trigger) {
	if ( trigger->isChecked() )
		for(QAction *i: trigger->actionGroup()->actions())
			if ( i != trigger )
				i->setChecked(false);
}

MainWindow::~MainWindow()
{
	delete this->measurementDialog;
	delete this->toolGroup;
	delete this->detectionSettingsDialog;
	delete this->p;
	delete ui;
	delete this->detectionListModel;
}


