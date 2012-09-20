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

#ifndef QGLSIMULATOR_H
#define QGLSIMULATOR_H

#include <QtGui/qpainter.h>
#include <QtGui/qcolor.h>
#include <QtGui/qmatrix4x4.h>
#include <QtGui/qvector3d.h>
#include "qvector2darray.h"
#include "qvector3darray.h"

class QGLSimulator
{
public:
    QGLSimulator(QPainter *painter, const QSize& size);
    ~QGLSimulator();

    void setClearColor(const QColor& color) { m_clearColor = color; }
    void clear();

    void setScissor(const QRect& rect);
    void intersectScissor(const QRect& rect);
    void expandScissor(const QRect& rect);
    void clearScissor();

    void setViewport(int x, int y, int width, int height);

    QMatrix4x4 projectionMatrix() const { return m_projectionMatrix; }
    void setProjectionMatrix(const QMatrix4x4& m) { m_projectionMatrix = m; }

    QMatrix4x4 modelViewMatrix() const { return m_modelViewMatrix; }
    void setModelViewMatrix(const QMatrix4x4& m) { m_modelViewMatrix = m; }

    void setColor(const QColor& color);

    void drawTriangles(const QVector2DArray& array);
    void drawTriangles(const QVector3DArray& array);

private:
    QPainter *m_painter;
    QSize m_size;
    QColor m_clearColor;
    QRect m_viewport;
    QMatrix4x4 m_projectionMatrix;
    QMatrix4x4 m_modelViewMatrix;
    QRect m_scissor;

    QPointF project(const QVector3D& vector) const;
};

#endif
