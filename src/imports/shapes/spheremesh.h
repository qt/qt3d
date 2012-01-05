/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef SPHERE_H
#define SPHERE_H

#include "qglscenenode.h"
#include "qdeclarativemesh.h"
#include "spheremesh_p.h"

#include <QtCore/qmap.h>
#include <QtCore/qscopedpointer.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class SphereMesh : public QDeclarativeMesh
{
    Q_OBJECT
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(int levelOfDetail READ levelOfDetail WRITE setLevelOfDetail NOTIFY levelOfDetailChanged)
    Q_PROPERTY(Qt::Axis axis READ axis WRITE setAxis NOTIFY axisChanged)
public:
    SphereMesh(QObject *parent = 0);
    ~SphereMesh() {}

    qreal radius() const;
    void setRadius(qreal radius);

    int levelOfDetail() const;
    void setLevelOfDetail(int lod);

    Qt::Axis axis() const;
    void setAxis(Qt::Axis axis);

Q_SIGNALS:
    void radiusChanged();
    void levelOfDetailChanged();
    void axisChanged();

private:
    void createGeometry();

    Q_DISABLE_COPY(SphereMesh)
    Q_DECLARE_PRIVATE(SphereMesh)

    QScopedPointer<SphereMeshPrivate> d_ptr;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(SphereMesh)

QT_END_HEADER

#endif
