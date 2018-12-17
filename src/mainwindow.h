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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QActionGroup>
#include <QButtonGroup>

#include "videoprocessorcontroller.h"
#include "measurementdialog.h"
#include "detectionzonedialog.h"
#include "detectionslisttablemodel.h"
#include "detectionsettingsdialog.h"
#include "aboutdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
	cvqm::VideoProcessorController *p;
	QActionGroup *toolGroup;
	MeasurementDialog *measurementDialog;
	DetectionZoneDialog *detectionZoneDialog;
	DetectionsListTableModel *detectionListModel;
	DetectionSettingsDialog *detectionSettingsDialog;
	AboutDialog *aboutDialog;
	Ui::MainWindow *ui;

	void configureDebugMenu();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
	void toolgroupExclusive(QAction *trigger);
};

#endif // MAINWINDOW_H
