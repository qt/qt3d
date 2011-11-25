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

#ifndef SCALE3D_H
#define SCALE3D_H

#include "qgraphicsscale3d.h"

#include <QtCore/qvariant.h>
#include <QtDeclarative/qdeclarative.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class Scale3D : public QGraphicsScale3D
{
    Q_OBJECT
    Q_PROPERTY(QVariant scale READ scale WRITE setScale NOTIFY variantScaleChanged)
public:
    Scale3D(QObject *parent = 0) : QGraphicsScale3D(parent) {}
    ~Scale3D() {}

    QVariant scale() const { return QGraphicsScale3D::scale(); }
    void setScale(const QVariant &value);

    QGraphicsTransform3D *clone(QObject *parent) const;

Q_SIGNALS:
    void variantScaleChanged();

private:
    Q_DISABLE_COPY(Scale3D)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(Scale3D)

QT_END_HEADER

#endif
