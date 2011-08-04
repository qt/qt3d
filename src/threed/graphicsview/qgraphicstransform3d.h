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

#ifndef QGRAPHICSTRANSFORM3D_H
#define QGRAPHICSTRANSFORM3D_H

#include <QtCore/qobject.h>
#include <QtGui/qmatrix4x4.h>
#include "qt3dglobal.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class Q_QT3D_EXPORT QGraphicsTransform3D : public QObject
{
    Q_OBJECT
public:
    QGraphicsTransform3D(QObject *parent = 0) : QObject(parent) {}
    ~QGraphicsTransform3D() {}

    virtual void applyTo(QMatrix4x4 *matrix) const = 0;
    virtual QGraphicsTransform3D *clone(QObject *parent = 0) const = 0;

Q_SIGNALS:
    void transformChanged();
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
