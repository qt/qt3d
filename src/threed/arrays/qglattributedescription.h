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

#ifndef QGLATTRIBUTEDESCRIPTION_H
#define QGLATTRIBUTEDESCRIPTION_H

#include <Qt3D/qt3dglobal.h>
#include <Qt3D/qglnamespace.h>

#include <QOpenGLBuffer>

QT_BEGIN_NAMESPACE

class Q_QT3D_EXPORT QGLAttributeDescription
{
public:
    QGLAttributeDescription();
    QGLAttributeDescription(QGL::VertexAttribute attribute,
                            int tupleSize, GLenum type, int stride);

    bool isNull() const;

    QGL::VertexAttribute attribute() const;
    void setAttribute(QGL::VertexAttribute attribute);

    GLenum type() const;
    void setType(GLenum type);

    int sizeOfType() const;

    int tupleSize() const;
    void setTupleSize(int tupleSize);

    int stride() const;
    void setStride(int stride);

private:
    QGL::VertexAttribute m_attribute;
    GLenum m_type;
    int m_tupleSize;
    int m_stride;
};

inline QGLAttributeDescription::QGLAttributeDescription()
    : m_attribute(QGL::Position), m_type(GL_FLOAT),
      m_tupleSize(0), m_stride(0)
{
}

inline QGLAttributeDescription::QGLAttributeDescription
        (QGL::VertexAttribute attribute_, int tupleSize_, GLenum type_, int stride_)
    : m_attribute(attribute_), m_type(type_),
      m_tupleSize(tupleSize_), m_stride(stride_)
{
    Q_ASSERT(tupleSize_ >= 1 && tupleSize_ <= 4);
}

inline bool QGLAttributeDescription::isNull() const
{
    return m_tupleSize == 0;
}

inline QGL::VertexAttribute QGLAttributeDescription::attribute() const
{
    return m_attribute;
}

inline void QGLAttributeDescription::setAttribute(QGL::VertexAttribute attribute_)
{
    m_attribute = attribute_;
}

inline GLenum QGLAttributeDescription::type() const
{
    return m_type;
}

inline void QGLAttributeDescription::setType(GLenum type_)
{
    m_type = type_;
}

inline int QGLAttributeDescription::tupleSize() const
{
    return m_tupleSize;
}

inline void QGLAttributeDescription::setTupleSize(int tupleSize_)
{
    Q_ASSERT(tupleSize_ >= 1 && tupleSize_ <= 4);
    m_tupleSize = tupleSize_;
}

inline int QGLAttributeDescription::stride() const
{
    return m_stride;
}

inline void QGLAttributeDescription::setStride(int stride_)
{
    m_stride = stride_;
}

QT_END_NAMESPACE

#endif
