/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
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

#include "qdeclarativeitem3d.h"
#include "qglscenenode.h"
#include <QtCore/qmap.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class Sphere : public QDeclarativeItem3D
{
    Q_OBJECT
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(int levelOfDetail READ levelOfDetail WRITE setLevelOfDetail NOTIFY levelOfDetailChanged)
    Q_PROPERTY(Qt::Axis axis READ axis WRITE setAxis NOTIFY axisChanged)
public:
    Sphere(QObject *parent = 0);
    ~Sphere() {}

    qreal radius() const { return m_radius; }
    void setRadius(qreal radius);

    int levelOfDetail() const { return m_lod; }
    void setLevelOfDetail(int lod);

    Qt::Axis axis() const { return m_axis; }
    void setAxis(Qt::Axis axis);

Q_SIGNALS:
    void radiusChanged();
    void levelOfDetailChanged();
    void axisChanged();

protected:
    void drawItem(QGLPainter *painter);

private:
    qreal m_radius;
    int m_lod;
    Qt::Axis m_axis;
    QMap<int, QGLSceneNode *> m_lodGeometry;
};

QML_DECLARE_TYPE(Sphere)

QT_END_NAMESPACE

QT_END_HEADER

#endif
