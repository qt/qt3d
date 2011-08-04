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

#ifndef QVECTOR4DARRAY_H
#define QVECTOR4DARRAY_H

#include "qarray.h"
#include <QtGui/qvector4d.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3D)

class QMatrix4x4;

class Q_QT3D_EXPORT QVector4DArray : public QArray<QVector4D>
{
public:
    QVector4DArray();
    QVector4DArray(int size, const QVector4D& value = QVector4D());
    QVector4DArray(const QArray<QVector4D>& other);

    void append(qreal x, qreal y, qreal z, qreal w);

    void scale(qreal scale);
    QVector4DArray scaled(qreal scale) const;

    void translate(const QVector4D& value);
    void translate(qreal x, qreal y, qreal z, qreal w);

    QArray<QVector4D> translated(const QVector4D& value) const;
    QArray<QVector4D> translated
        (qreal x, qreal y, qreal z, qreal w) const;

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

inline QVector4DArray::QVector4DArray(int size, const QVector4D& value)
    : QArray<QVector4D>(size, value) {}

inline QVector4DArray::QVector4DArray(const QArray<QVector4D>& other)
    : QArray<QVector4D>(other) {}

inline void QVector4DArray::append(qreal x, qreal y, qreal z, qreal w)
    { QArray<QVector4D>::append(QVector4D(x, y, z, w)); }

inline void QVector4DArray::translate(qreal x, qreal y, qreal z, qreal w)
    { translate(QVector4D(x, y, z, w)); }

inline QArray<QVector4D> QVector4DArray::translated
        (qreal x, qreal y, qreal z, qreal w) const
    { return translated(QVector4D(x, y, z, w)); }

QT_END_NAMESPACE

QT_END_HEADER

#endif
