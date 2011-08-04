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

#ifndef QVECTOR3DARRAY_H
#define QVECTOR3DARRAY_H

#include "qarray.h"
#include <QtGui/qvector3d.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3D)

class QMatrix4x4;

class Q_QT3D_EXPORT QVector3DArray : public QArray<QVector3D>
{
public:
    QVector3DArray();
    QVector3DArray(int size, const QVector3D& value = QVector3D());
    QVector3DArray(const QArray<QVector3D>& other);

    void append(qreal x, qreal y, qreal z);

    void scale(qreal scale);
    QVector3DArray scaled(qreal scale) const;

    void translate(const QVector3D& value);
    void translate(qreal x, qreal y, qreal z);

    QArray<QVector3D> translated(const QVector3D& value) const;
    QArray<QVector3D> translated(qreal x, qreal y, qreal z) const;

    void transform(const QMatrix4x4& matrix);
    QArray<QVector3D> transformed(const QMatrix4x4& matrix) const;

#if !defined(Q_NO_USING_KEYWORD) || defined(Q_QDOC)
    using QArray<QVector3D>::append;
#else
    inline void append(const QVector3D& value)
        { QArray<QVector3D>::append(value); }
    inline void append(const QVector3D& value1, const QVector3D& value2)
        { QArray<QVector3D>::append(value1, value2); }
    inline void append(const QVector3D& value1, const QVector3D& value2, const QVector3D& value3)
        { QArray<QVector3D>::append(value1, value2, value3); }
    inline void append(const QVector3D& value1, const QVector3D& value2, const QVector3D& value3, const QVector3D& value4)
        { QArray<QVector3D>::append(value1, value2, value3, value4); }
    inline void append(const QVector3D *values, int count)
        { QArray<QVector3D>::append(values, count); }
    inline void append(const QArray<QVector3D>& other)
        { QArray<QVector3D>::append(other); }
#endif
};

inline QVector3DArray::QVector3DArray() {}

inline QVector3DArray::QVector3DArray(int size, const QVector3D& value)
    : QArray<QVector3D>(size, value) {}

inline QVector3DArray::QVector3DArray(const QArray<QVector3D>& other)
    : QArray<QVector3D>(other) {}

inline void QVector3DArray::append(qreal x, qreal y, qreal z)
    { QArray<QVector3D>::append(QVector3D(x, y, z)); }

inline void QVector3DArray::translate(qreal x, qreal y, qreal z)
    { translate(QVector3D(x, y, z)); }

inline QArray<QVector3D> QVector3DArray::translated(qreal x, qreal y, qreal z) const
    { return translated(QVector3D(x, y, z)); }

QT_END_NAMESPACE

QT_END_HEADER

#endif
