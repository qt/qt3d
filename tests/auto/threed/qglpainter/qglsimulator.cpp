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

#include "qglsimulator.h"

QGLSimulator::QGLSimulator(QPainter *painter, const QSize& size)
    : m_painter(painter),
      m_size(size),
      m_clearColor(Qt::black),
      m_viewport(0, 0, size.width(), size.height())
{
}

QGLSimulator::~QGLSimulator()
{
}

void QGLSimulator::clear()
{
    m_painter->fillRect(0, 0, m_size.width(), m_size.height(), m_clearColor);
}

void QGLSimulator::setScissor(const QRect& rect)
{
    m_scissor = rect;
    m_painter->setClipRect(rect);
}

void QGLSimulator::intersectScissor(const QRect& rect)
{
    m_scissor = rect.intersected(m_scissor);
    m_painter->setClipRect(m_scissor);
}

void QGLSimulator::expandScissor(const QRect& rect)
{
    m_scissor = rect.united(m_scissor);
    m_painter->setClipRect(m_scissor);
}

void QGLSimulator::clearScissor()
{
    m_painter->setClipping(false);
}

void QGLSimulator::setViewport(int x, int y, int width, int height)
{
    m_viewport = QRect(x, y, width, height);
}

// Set the flat color for drawing triangles.
void QGLSimulator::setColor(const QColor& color)
{
    m_painter->setBrush(QBrush(color));
    m_painter->setPen(QPen());
}

// Draw a set of triangles from a vertex array.
void QGLSimulator::drawTriangles(const QVector2DArray& array)
{
    QVector<QPointF> points;

    for (int index = 0; index < array.count(); ++index) {
        QVector3D v = array[index];
        points.append(project(v));
    }

    m_painter->drawPolygon(points);
}

void QGLSimulator::drawTriangles(const QVector3DArray& array)
{
    QVector<QPointF> points;

    for (int index = 0; index < array.count(); ++index) {
        QVector3D v = array[index];
        points.append(project(v));
    }

    m_painter->drawPolygon(points);
}

// Project a 3D point to 2D and convert to left-handed QPainter co-ordinates.
QPointF QGLSimulator::project(const QVector3D& vector) const
{
    QVector3D pt = (m_projectionMatrix * m_modelViewMatrix) * vector;
    float x = (pt.x() + 1.0f) * m_viewport.width() * 0.5f + m_viewport.x();
    float y = (pt.y() + 1.0f) * m_viewport.height() * 0.5f + m_viewport.y();
    y = (m_viewport.height() - 1) - y;
    return QPointF(x, y);
}
