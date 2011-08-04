/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGLCYLINDER_H
#define QGLCYLINDER_H

#include "qt3dglobal.h"
#include "qglmaterialcollection.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3D)

class QGLBuilder;
class QVector2D;

class Q_QT3D_EXPORT QGLCylinder
{
public:
    explicit QGLCylinder(qreal diameterTop = 1.0f, qreal diameterBase = 1.0f, qreal height = 1.0f, int slices = 6, int layers = 3, bool top = true, bool base = true)
        : m_diameterTop(diameterTop), m_diameterBottom(diameterBase), m_height(height), m_slices(slices), m_layers(layers), m_top(top), m_base(base) {}

    //Cylinder dimensions
    qreal diameterTop() const {return m_diameterTop;}
    void setDiameterTop(qreal diameter) {m_diameterTop=diameter;}

    qreal diameterBottom() const {return m_diameterBottom;}
    void setDiameterBottom(qreal diameter) {m_diameterBottom=diameter;}

    qreal height() const {return m_height;}
    void setHeight(qreal height) {m_height = height;}

    //Cylinder geometrical subdivisions
    int slices() const {return m_slices;}
    void setSlices(int slices) {m_slices = slices;}

    int layers() const {return m_layers;}
    void setLayers(int layers) {m_layers = layers;}

    //End-caps attached?
    bool topEnabled() const {return m_top;}
    void setTopEnabled(bool top) {m_top = top;}

    bool baseEnabled() const {return m_base;}
    void setBaseEnabled(bool base) {m_base = base;}

protected:
    qreal m_diameterTop;
    qreal m_diameterBottom;
    qreal m_height;

    int m_slices;
    int m_layers;

    bool m_top;
    bool m_base;
};

Q_QT3D_EXPORT QGLBuilder& operator<<(QGLBuilder& builder, const QGLCylinder& cylinder);

QT_END_NAMESPACE

QT_END_HEADER

#endif // QGLCYLINDER_H
