/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef CYLINDER_H
#define CYLINDER_H

#include "qdeclarativemesh.h"
#include "cylindermesh_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class CylinderMesh : public QDeclarativeMesh
{
    Q_OBJECT
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(qreal length READ length WRITE setLength NOTIFY lengthChanged)
    Q_PROPERTY(int levelOfDetail READ levelOfDetail WRITE setLevelOfDetail NOTIFY levelOfDetailChanged)

public:
    explicit CylinderMesh(QObject *parent = 0);
    ~CylinderMesh() {}

    qreal radius() const;
    void setRadius(qreal radius);

    qreal length() const;
    void setLength(qreal length);

    int levelOfDetail() const;
    void setLevelOfDetail(int lod);

    void draw(QGLPainter *painter, int branchId);

Q_SIGNALS:
    void radiusChanged();
    void lengthChanged();
    void levelOfDetailChanged();

private:
    void createGeometry();

    Q_DISABLE_COPY(CylinderMesh)
    Q_DECLARE_PRIVATE(CylinderMesh)

    QScopedPointer<CylinderMeshPrivate> d_ptr;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(CylinderMesh)

QT_END_HEADER

#endif // CYLINDER_H
