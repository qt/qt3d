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

#ifndef QMATRIX4X4STACK_H
#define QMATRIX4X4STACK_H

#include <Qt3D/qt3dglobal.h>
#include <QtGui/qmatrix4x4.h>
#include <QtCore/qscopedpointer.h>

QT_BEGIN_NAMESPACE

class QMatrix4x4StackPrivate;

class Q_QT3D_EXPORT QMatrix4x4Stack
{
public:
    QMatrix4x4Stack();
    ~QMatrix4x4Stack();

    const QMatrix4x4 &top() const;

    void push();
    void pop();

    void setToIdentity();

    void translate(float x, float y, float z);
    void translate(const QVector3D& vector);
    void scale(float x, float y, float z);
    void scale(float factor);
    void scale(const QVector3D& vector);
    void rotate(float angle, float x, float y, float z);
    void rotate(float angle, const QVector3D& vector);
    void rotate(const QQuaternion &quaternion);

    QMatrix4x4Stack& operator=(const QMatrix4x4& matrix);
    QMatrix4x4Stack& operator*=(const QMatrix4x4& matrix);

    operator const QMatrix4x4 &() const;

    bool isDirty() const;
    void setDirty(bool dirty);

private:
    Q_DISABLE_COPY(QMatrix4x4Stack)
    Q_DECLARE_PRIVATE(QMatrix4x4Stack)

    QScopedPointer<QMatrix4x4StackPrivate> d_ptr;

    friend class QGLPainter;
};

inline QMatrix4x4Stack::operator const QMatrix4x4 &() const
{
    return top();
}

QT_END_NAMESPACE

#endif
