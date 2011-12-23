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

#ifndef QATLAS_H
#define QATLAS_H

#include <QSize>

#include "qarray.h"
#include "qgeometrydata.h"

QT_BEGIN_NAMESPACE
class QAreaAllocator;
class QGLTexture2D;
class QGLMaterial;
class QGeometryData;
class QGLFramebufferObject;
class QGLFramebufferObjectSurface;
QT_END_NAMESPACE

struct QAtlasEntry
{
    QAtlasEntry(QImage i, QRect r) : image(i), rect(r) {}
    QImage image;
    QRect rect;
};

class QAtlas
{
public:
    QAtlas();
    ~QAtlas();

    QAreaAllocator *allocator() const { return m_allocator; }
    void setAllocator(QAreaAllocator *allocator) { m_allocator = allocator; }

    QRect allocate(const QSize &size, const QImage &image, const QGL::IndexArray &indices);

    void initialize(QGLPainter *painter);
    void paint(QGLPainter *painter);

    void release(QRect frame);

    void setGeometry(QGeometryData geometry) { m_geometry = geometry; }
    QGeometryData geometry() { return m_geometry; }

    QGLMaterial *material() { return m_material; }

    QList<QAtlasEntry> allocationQueue() const { return m_allocationQueue; }

    static QAtlas *instance();

private:
    QSize m_size;
    QGLFramebufferObject *m_data;
    QGLFramebufferObjectSurface *m_renderTarget;
    QAreaAllocator *m_allocator;
    QGLTexture2D *m_tex;
    QGLMaterial *m_material;
    QGeometryData m_geometry;
    bool m_initialized;
    QList<QAtlasEntry> m_allocationQueue;
    int m_count;
};

#endif // QATLAS_H
