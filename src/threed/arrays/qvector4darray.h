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

#ifndef QVECTOR4DARRAY_H
#define QVECTOR4DARRAY_H

#include <Qt3D/qarray.h>
#include <QtGui/qvector4d.h>

QT_BEGIN_NAMESPACE

class QMatrix4x4;

class Q_QT3D_EXPORT QVector4DArray : public QArray<QVector4D>
{
public:
    QVector4DArray();
    QVector4DArray(int fillSize, const QVector4D& fillValue = QVector4D());
    QVector4DArray(const QArray<QVector4D>& other);

    void append(float x, float y, float z, float w);

    void scale(float scale);
    QVector4DArray scaled(float scale) const;

    void translate(const QVector4D& value);
    void translate(float x, float y, float z, float w);

    QArray<QVector4D> translated(const QVector4D& value) const;
    QArray<QVector4D> translated
        (float x, float y, float z, float w) const;

    void transform(const QMatrix4x4& matrix);
    QArray<QVector4D> transformed(const QMatrix4x4& matrix) const;

#if !defined(Q_NO_USING_KEYWORD) || defined(Q_QDOC)
    using QArray<QVector4D>::append;
#else
    inline void append(const QVector4D& value)
        { QArray<QVector4D>::append(value); }
    inline void append(const QVector4D& value1, const QVector4D& value2)
        { QArray<QVector4D>::append(value1, value2); }
    inline void append(const QVector4D& value1, const QVector4D& value2, const QVector4D& value3)
        { QArray<QVector4D>::append(value1, value2, value3); }
    inline void append(const QVector4D& value1, const QVector4D& value2, const QVector4D& value3, const QVector4D& value4)
        { QArray<QVector4D>::append(value1, value2, value3, value4); }
    inline void append(const QVector4D *values, int count)
        { QArray<QVector4D>::append(values, count); }
    inline void append(const QArray<QVector4D>& other)
        { QArray<QVector4D>::append(other); }
#endif
};

inline QVector4DArray::QVector4DArray() {}

inline QVector4DArray::QVector4DArray(int fillSize, const QVector4D& fillValue)
    : QArray<QVector4D>(fillSize, fillValue) {}

inline QVector4DArray::QVector4DArray(const QArray<QVector4D>& other)
    : QArray<QVector4D>(other) {}

inline void QVector4DArray::append(float x, float y, float z, float w)
    { QArray<QVector4D>::append(QVector4D(x, y, z, w)); }

inline void QVector4DArray::translate(float x, float y, float z, float w)
    { translate(QVector4D(x, y, z, w)); }

inline QArray<QVector4D> QVector4DArray::translated
        (float x, float y, float z, float w) const
    { return translated(QVector4D(x, y, z, w)); }

QT_END_NAMESPACE

#endif
