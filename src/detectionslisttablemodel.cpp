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

#include <QPixmap>
#include <chrono>
#include <vector>
#include <ctime>

#include "detectionslisttablemodel.h"

using namespace std;

DetectionsListTableModel::Record::Record(time_t t, QString &name, double vel, double dir, QImage &img) :
	time(t),
	name(name),
	vel(vel),
	dir(dir),
	img(img)
{

}

DetectionsListTableModel::Record::~Record() = default;

DetectionsListTableModel::~DetectionsListTableModel()
{
	for(Record *r: records)
		delete r;
}

int DetectionsListTableModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return static_cast<int>(records.size());
}

int DetectionsListTableModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return Record::FIELD_COUNT;
}

QVariant DetectionsListTableModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	auto idx = static_cast<vector<Record*>::size_type>(index.row());

	if ( idx >= records.size() )
		return QVariant();

	if (role != Qt::DisplayRole)
		return QVariant();

	idx = records.size() - idx - 1; // newest first
	Record *r = records[idx];
	switch(index.column()) {
	case TIME: {
		char buffer[32];
		tm * ptm = localtime(&r->time);
		strftime(buffer, 32, "%a, %d.%m.%Y %H:%M:%S", ptm);
		return QString(buffer);
	}
	case NAME:
		return r->name;
	case VEL:
		return r->vel;
	case DIR:
		return r->dir;
	case PIX:
		return r->img;
	default:
		return QVariant();
	}
}

QVariant DetectionsListTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal )
		return QVariant();

	if ( role == Qt::DisplayRole ) {
		switch(section) {
		case TIME:
			return tr("Time");
		case NAME:
			return tr("Zone");
		case VEL:
			return tr("Speed km/h");
		case DIR:
			return tr("Bearing Deg.");
		case PIX:
			return tr("Snapshot");
		default:
			return tr("Unknown");
		}
	}

	if ( role == Qt::SizeHintRole ) {
		switch(section) {
		case TIME:
			return QSize(50,20);
		case NAME:
			return QSize(50,20);
		case VEL:
			return QSize(50,20);
		case DIR:
			return QSize(50,20);
		case PIX:
			return QSize(100,15);
		default:
			return QVariant();
		}
	}

	return QVariant();
}

Qt::ItemFlags DetectionsListTableModel::flags(const QModelIndex &index) const
{
	Q_UNUSED(index);
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

void DetectionsListTableModel::newDetection(QString zone, double dir, double vel, QImage image)
{
	time_t time = std::time(nullptr);

	Record *r = new Record(time, zone, vel, dir, image);
	QModelIndex idx;
	beginInsertRows(idx, 0, 0);
	this->records.push_back(r);
	endInsertRows();
}
