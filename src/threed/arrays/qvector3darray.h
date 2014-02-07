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

#ifndef QVECTOR3DARRAY_H
#define QVECTOR3DARRAY_H

#include <Qt3D/qarray.h>

#include <QtGui/qvector3d.h>

QT_BEGIN_NAMESPACE

class QMatrix4x4;

class Q_QT3D_EXPORT QVector3DArray : public QArray<QVector3D>
{
public:
    QVector3DArray();
    QVector3DArray(int fillSize, const QVector3D& fillValue = QVector3D());
    QVector3DArray(const QArray<QVector3D>& other);

    void append(float x, float y, float z);

    void scale(float scale);
    QVector3DArray scaled(float scale) const;

    void translate(const QVector3D& value);
    void translate(float x, float y, float z);

    QArray<QVector3D> translated(const QVector3D& value) const;
    QArray<QVector3D> translated(float x, float y, float z) const;

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

inline QVector3DArray::QVector3DArray(int fillSize, const QVector3D& fillValue)
    : QArray<QVector3D>(fillSize, fillValue) {}

inline QVector3DArray::QVector3DArray(const QArray<QVector3D>& other)
    : QArray<QVector3D>(other) {}

inline void QVector3DArray::append(float x, float y, float z)
    { QArray<QVector3D>::append(QVector3D(x, y, z)); }

inline void QVector3DArray::translate(float x, float y, float z)
    { translate(QVector3D(x, y, z)); }

inline QArray<QVector3D> QVector3DArray::translated(float x, float y, float z) const
    { return translated(QVector3D(x, y, z)); }

QT_END_NAMESPACE

#endif
