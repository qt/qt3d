/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGLCYLINDER_H
#define QGLCYLINDER_H

#include <Qt3D/qt3dglobal.h>
#include <Qt3D/qglmaterialcollection.h>

QT_BEGIN_NAMESPACE

class QGLBuilder;
class QVector2D;

class Q_QT3D_EXPORT QGLCylinder
{
public:
    explicit QGLCylinder(float diameterTop_ = 1.0f, float diameterBase = 1.0f, float height_ = 1.0f, int slices_ = 6, int layers_ = 3, bool top = true, bool base = true)
        : m_diameterTop(diameterTop_), m_diameterBottom(diameterBase), m_height(height_), m_slices(slices_), m_layers(layers_), m_top(top), m_base(base) {}

    //Cylinder dimensions
    float diameterTop() const {return m_diameterTop;}
    void setDiameterTop(float diameter) {m_diameterTop=diameter;}

    float diameterBottom() const {return m_diameterBottom;}
    void setDiameterBottom(float diameter) {m_diameterBottom=diameter;}

    float height() const {return m_height;}
    void setHeight(float height_) {m_height = height_;}

    //Cylinder geometrical subdivisions
    int slices() const {return m_slices;}
    void setSlices(int slices_) {m_slices = slices_;}

    int layers() const {return m_layers;}
    void setLayers(int layers_) {m_layers = layers_;}

    //End-caps attached?
    bool topEnabled() const {return m_top;}
    void setTopEnabled(bool top) {m_top = top;}

    bool baseEnabled() const {return m_base;}
    void setBaseEnabled(bool base) {m_base = base;}

protected:
    float m_diameterTop;
    float m_diameterBottom;
    float m_height;

    int m_slices;
    int m_layers;

    bool m_top;
    bool m_base;
};

Q_QT3D_EXPORT QGLBuilder& operator<<(QGLBuilder& builder, const QGLCylinder& cylinder);

QT_END_NAMESPACE

#endif // QGLCYLINDER_H
