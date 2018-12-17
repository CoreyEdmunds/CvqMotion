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

#ifndef DETECTIONSLISTTABLEMODEL_H
#define DETECTIONSLISTTABLEMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <string>
#include <memory>
#include <QPixmap>
#include <ctime>


class DetectionsListTableModel : public QAbstractTableModel
{
	Q_OBJECT
private:
	class Record {
	public:
		static long constexpr FIELD_COUNT = 5;
		Record(std::time_t t, QString &name, double vel, double dir, QImage &img);
		const std::time_t time;
		const QString name;
		const double vel;
		const double dir;
		const QImage img;
		~Record();
	};

	enum ColumnOrder {
		TIME=0,
		NAME,
		VEL,
		DIR,
		PIX
	};

	std::vector<Record*> records;

public:
	DetectionsListTableModel(QObject *parent=nullptr) {Q_UNUSED(parent)}
	virtual ~DetectionsListTableModel() override;

	int rowCount(const QModelIndex &parent) const override;
	int columnCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;


public slots:
	void newDetection(QString zone, double dir, double vel, QImage image);
};

#endif // DETECTIONSLISTTABLEMODEL_H
