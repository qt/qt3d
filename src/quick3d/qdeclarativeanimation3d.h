/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the Qt3D module of the Qt Toolkit.
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEANIMATION3D_H
#define QDECLARATIVEANIMATION3D_H

#include "qt3dquickglobal.h"

#include <qqml.h>
#include <QtCore/qobject.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(QtQuick3D)

class QDeclarativeAnimation3DPrivate;
class QGLSceneAnimation;

class Q_QT3D_QUICK_EXPORT QDeclarativeAnimation3D : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged DESIGNABLE false )
public:
    explicit QDeclarativeAnimation3D(QObject *parent = 0);
    explicit QDeclarativeAnimation3D(QGLSceneAnimation* pAnim, QObject *parent = 0);
    ~QDeclarativeAnimation3D();

    QString name() const;

public Q_SLOTS:

Q_SIGNALS:
    void nameChanged();

private:
    QDeclarativeAnimation3DPrivate *d;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeAnimation3D)

QT_END_HEADER

#endif // QDECLARATIVEANIMATION3D_H
