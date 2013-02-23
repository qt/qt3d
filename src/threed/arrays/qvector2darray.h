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

#ifndef QVECTOR2DARRAY_H
#define QVECTOR2DARRAY_H

#include <Qt3D/qarray.h>

#include <QtGui/qvector2d.h>

QT_BEGIN_NAMESPACE

class QMatrix4x4;

class Q_QT3D_EXPORT QVector2DArray : public QArray<QVector2D>
{
public:
    QVector2DArray();
    QVector2DArray(int fillSize, const QVector2D& fillValue = QVector2D());
    QVector2DArray(const QArray<QVector2D>& other);

    void append(float x, float y);
    void append(const QPointF& point);
    void append(const QPoint& point);

    void scale(float scale);
    QVector2DArray scaled(float scale) const;

    void translate(const QVector2D& value);
    void translate(float x, float y);

    QArray<QVector2D> translated(const QVector2D& value) const;
    QArray<QVector2D> translated(float x, float y) const;

    void transform(const QMatrix4x4& matrix);
    QArray<QVector2D> transformed(const QMatrix4x4& matrix) const;

#if !defined(Q_NO_USING_KEYWORD) || defined(Q_QDOC)
    using QArray<QVector2D>::append;
#else
    inline void append(const QVector2D& value)
        { QArray<QVector2D>::append(value); }
    inline void append(const QVector2D& value1, const QVector2D& value2)
        { QArray<QVector2D>::append(value1, value2); }
    inline void append(const QVector2D& value1, const QVector2D& value2, const QVector2D& value3)
        { QArray<QVector2D>::append(value1, value2, value3); }
    inline void append(const QVector2D& value1, const QVector2D& value2, const QVector2D& value3, const QVector2D& value4)
        { QArray<QVector2D>::append(value1, value2, value3, value4); }
    inline void append(const QVector2D *values, int count)
        { QArray<QVector2D>::append(values, count); }
    inline void append(const QArray<QVector2D>& other)
        { QArray<QVector2D>::append(other); }
#endif
};

inline QVector2DArray::QVector2DArray() {}

inline QVector2DArray::QVector2DArray(int fillSize, const QVector2D& fillValue)
    : QArray<QVector2D>(fillSize, fillValue) {}

inline QVector2DArray::QVector2DArray(const QArray<QVector2D>& other)
    : QArray<QVector2D>(other) {}

inline void QVector2DArray::append(float x, float y)
    { QArray<QVector2D>::append(QVector2D(x, y)); }

inline void QVector2DArray::append(const QPointF& point)
    { QArray<QVector2D>::append(QVector2D(point)); }

inline void QVector2DArray::append(const QPoint& point)
    { QArray<QVector2D>::append(QVector2D(point)); }

inline void QVector2DArray::translate(float x, float y)
    { translate(QVector2D(x, y)); }

inline QArray<QVector2D> QVector2DArray::translated(float x, float y) const
    { return translated(QVector2D(x, y)); }

QT_END_NAMESPACE

#endif
