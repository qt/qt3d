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

#ifndef QGRAPHICSLOOKATTRANSFORM_H
#define QGRAPHICSLOOKATTRANSFORM_H

#include "qgraphicstransform3d.h"
#include "qquickitem3d.h"

#include <QtCore/qscopedpointer.h>
#include <QtCore/qpointer.h>

QT_BEGIN_NAMESPACE

class QGraphicsLookAtTransformPrivate;

class QGraphicsLookAtTransform : public QQuickQGraphicsTransform3D
{
    Q_OBJECT
    Q_PROPERTY(bool preserveUpVector READ preserveUpVector WRITE setPreserveUpVector NOTIFY preserveUpVectorChanged)
    Q_PROPERTY(QQuickItem3D* subject READ subject WRITE setSubject NOTIFY subjectChanged )
public:
    QGraphicsLookAtTransform(QObject *parent = 0);
    ~QGraphicsLookAtTransform();

    bool preserveUpVector() const;
    void setPreserveUpVector(bool value);

    QQuickItem3D* subject() const;
    void setSubject(QQuickItem3D* value);

    void applyTo(QMatrix4x4 *matrix) const;
    QQuickQGraphicsTransform3D *clone(QObject *parent) const;

private Q_SLOTS:
    void subjectPositionChanged();
    void ancestryChanged();

Q_SIGNALS:
    void preserveUpVectorChanged();
    void subjectChanged();

private:
    QScopedPointer<QGraphicsLookAtTransformPrivate> d_ptr;

    Q_DISABLE_COPY(QGraphicsLookAtTransform)
    Q_DECLARE_PRIVATE(QGraphicsLookAtTransform)
};

QT_END_NAMESPACE

#endif
