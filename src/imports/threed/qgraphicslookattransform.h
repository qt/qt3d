/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGRAPHICSLOOKATTRANSFORM_H
#define QGRAPHICSLOOKATTRANSFORM_H

#include "qgraphicstransform3d.h"
#include "qdeclarativeitem3d.h"

#include <QtCore/qscopedpointer.h>
#include <QtCore/qpointer.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QGraphicsLookAtTransformPrivate;

class QGraphicsLookAtTransform : public QGraphicsTransform3D
{
    Q_OBJECT
    Q_PROPERTY(bool preserveUpVector READ preserveUpVector WRITE setPreserveUpVector NOTIFY preserveUpVectorChanged)
    Q_PROPERTY(QDeclarativeItem3D* subject READ subject WRITE setSubject NOTIFY subjectChanged )
public:
    QGraphicsLookAtTransform(QObject *parent = 0);
    ~QGraphicsLookAtTransform();

    bool preserveUpVector() const;
    void setPreserveUpVector(bool value);

    QDeclarativeItem3D* subject() const;
    void setSubject(QDeclarativeItem3D* value);

    void applyTo(QMatrix4x4 *matrix) const;
    QGraphicsTransform3D *clone(QObject *parent) const;

private Q_SLOTS:
    void subjectPositionChanged();

Q_SIGNALS:
    void preserveUpVectorChanged();
    void subjectChanged();

private:
    QScopedPointer<QGraphicsLookAtTransformPrivate> d_ptr;

    Q_DISABLE_COPY(QGraphicsLookAtTransform)
    Q_DECLARE_PRIVATE(QGraphicsLookAtTransform)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
