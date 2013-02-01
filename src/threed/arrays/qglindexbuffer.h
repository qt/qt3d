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

#ifndef QGLINDEXBUFFER_H
#define QGLINDEXBUFFER_H

#include <Qt3D/qglnamespace.h>
#include <Qt3D/qarray.h>

#include <QOpenGLBuffer>

QT_BEGIN_NAMESPACE

class QGLIndexBufferPrivate;
class QGLPainter;

class Q_QT3D_EXPORT QGLIndexBuffer
{
public:
    QGLIndexBuffer();
    QGLIndexBuffer(const QGLIndexBuffer& other);
    ~QGLIndexBuffer();

    QGLIndexBuffer& operator=(const QGLIndexBuffer& other);

    QOpenGLBuffer::UsagePattern usagePattern() const;
    void setUsagePattern(QOpenGLBuffer::UsagePattern value);

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

    QOpenGLBuffer buffer() const;

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

#endif
