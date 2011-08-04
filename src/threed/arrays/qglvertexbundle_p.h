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

#ifndef QGLVERTEXBUNDLE_P_H
#define QGLVERTEXBUNDLE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qglvertexbundle.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QGLVertexBundleAttribute
{
public:
    QGLVertexBundleAttribute(QGL::VertexAttribute attr) : attribute(attr) {}
    virtual ~QGLVertexBundleAttribute() {}

    virtual void clear() = 0;
    virtual QGLAttributeValue uploadValue() = 0;
    virtual int count() = 0;
    virtual int elementSize() = 0;

    QGL::VertexAttribute attribute;
    QGLAttributeValue value;
};

class QGLVertexBundleFloatAttribute : public QGLVertexBundleAttribute
{
public:
    QGLVertexBundleFloatAttribute
            (QGL::VertexAttribute attr, const QArray<float>& array)
        : QGLVertexBundleAttribute(attr), floatArray(array)
    {
        value = QGLAttributeValue(floatArray);
    }

    void clear() { floatArray.clear(); }
    QGLAttributeValue uploadValue()
        { return QGLAttributeValue(floatArray); }
    int count() { return floatArray.count(); }
    int elementSize() { return sizeof(float); }

    QArray<float> floatArray;
};

class QGLVertexBundleVector2DAttribute : public QGLVertexBundleAttribute
{
public:
    QGLVertexBundleVector2DAttribute
            (QGL::VertexAttribute attr, const QArray<QVector2D>& array)
        : QGLVertexBundleAttribute(attr), vector2DArray(array)
    {
        value = QGLAttributeValue(vector2DArray);
    }

    void clear() { vector2DArray.clear(); }
    QGLAttributeValue uploadValue()
        { return QGLAttributeValue(vector2DArray); }
    int count() { return vector2DArray.count(); }
    int elementSize() { return sizeof(QVector2D); }

    QArray<QVector2D> vector2DArray;
};

class QGLVertexBundleVector3DAttribute : public QGLVertexBundleAttribute
{
public:
    QGLVertexBundleVector3DAttribute
            (QGL::VertexAttribute attr, const QArray<QVector3D>& array)
        : QGLVertexBundleAttribute(attr), vector3DArray(array)
    {
        value = QGLAttributeValue(vector3DArray);
    }

    void clear() { vector3DArray.clear(); }
    QGLAttributeValue uploadValue()
        { return QGLAttributeValue(vector3DArray); }
    int count() { return vector3DArray.count(); }
    int elementSize() { return sizeof(QVector3D); }

    QArray<QVector3D> vector3DArray;
};

class QGLVertexBundleVector4DAttribute : public QGLVertexBundleAttribute
{
public:
    QGLVertexBundleVector4DAttribute
            (QGL::VertexAttribute attr, const QArray<QVector4D>& array)
        : QGLVertexBundleAttribute(attr), vector4DArray(array)
    {
        value = QGLAttributeValue(vector4DArray);
    }

    void clear() { vector4DArray.clear(); }
    QGLAttributeValue uploadValue()
        { return QGLAttributeValue(vector4DArray); }
    int count() { return vector4DArray.count(); }
    int elementSize() { return sizeof(QVector4D); }

    QArray<QVector4D> vector4DArray;
};

class QGLVertexBundleColorAttribute : public QGLVertexBundleAttribute
{
public:
    QGLVertexBundleColorAttribute
            (QGL::VertexAttribute attr, const QArray<QColor4ub>& array)
        : QGLVertexBundleAttribute(attr), colorArray(array)
    {
        value = QGLAttributeValue(colorArray);
    }

    void clear() { colorArray.clear(); }
    QGLAttributeValue uploadValue()
        { return QGLAttributeValue(colorArray); }
    int count() { return colorArray.count(); }
    int elementSize() { return sizeof(QColor4ub); }

    QArray<QColor4ub> colorArray;
};

class QGLVertexBundleCustomAttribute : public QGLVertexBundleAttribute
{
public:
    QGLVertexBundleCustomAttribute
            (QGL::VertexAttribute attr, const QCustomDataArray& array)
        : QGLVertexBundleAttribute(attr), customArray(array)
    {
        value = QGLAttributeValue(customArray);
    }

    void clear() { customArray.clear(); }
    QGLAttributeValue uploadValue()
        { return QGLAttributeValue(customArray); }
    int count() { return customArray.count(); }
    int elementSize() { return customArray.elementSize(); }

    QCustomDataArray customArray;
};

class QGLVertexBundlePrivate
{
public:
    QGLVertexBundlePrivate()
        : buffer(QGLBuffer::VertexBuffer),
          vertexCount(0)
    {
        ref = 1;
    }
    ~QGLVertexBundlePrivate()
    {
        qDeleteAll(attributes);
    }

    QBasicAtomicInt ref;
    QGLBuffer buffer;
    QList<QGLVertexBundleAttribute *> attributes;
    int vertexCount;
    QGLAttributeSet attributeSet;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
