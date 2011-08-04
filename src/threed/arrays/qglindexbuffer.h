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

#ifndef QGLINDEXBUFFER_H
#define QGLINDEXBUFFER_H

#include <QtOpenGL/qgl.h>
#include <QtOpenGL/qglbuffer.h>
#include "qglnamespace.h"
#include "qarray.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3D)

class QGLIndexBufferPrivate;
class QGLPainter;

class Q_QT3D_EXPORT QGLIndexBuffer
{
public:
    QGLIndexBuffer();
    QGLIndexBuffer(const QGLIndexBuffer& other);
    ~QGLIndexBuffer();

    QGLIndexBuffer& operator=(const QGLIndexBuffer& other);

    QGLBuffer::UsagePattern usagePattern() const;
    void setUsagePattern(QGLBuffer::UsagePattern value);

    QArray<ushort> indexesUShort() const;
    QArray<uint> indexesUInt() const;

    void setIndexes(const QArray<ushort>& values);
    void setIndexes(const QArray<uint>& values);

    void replaceIndexes(int index, const QArray<ushort>& values);
    void replaceIndexes(int index, const QArray<uint>& values);

    GLenum elementType() const;

    int indexCount() const;
    bool isEmpty() const { return indexCount() == 0; }

    bool upload();
    bool isUploaded() const;

    QGLBuffer buffer() const;

    bool bind();
    void release();

    void append(const QGLIndexBuffer &buffer, uint offset);
    void append(const QGLIndexBuffer &buffer, uint offset, QGL::DrawingMode combineMode);

private:
    QGLIndexBufferPrivate *d_ptr;

    Q_DECLARE_PRIVATE(QGLIndexBuffer)

    friend class QGLPainter;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
