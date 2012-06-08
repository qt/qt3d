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

#include "qglabstractscene.h"
#include "qglsceneanimation.h"

QT_BEGIN_NAMESPACE

// ------------------------------------------------------------------------------------------------------------------------------

class QGLSceneAnimationPrivate
{
public:
    QGLSceneAnimationPrivate();
    ~QGLSceneAnimationPrivate();

    QString m_name;
};

QGLSceneAnimationPrivate::QGLSceneAnimationPrivate() :
    m_name(QLatin1String("unnamed"))
{
}

QGLSceneAnimationPrivate::~QGLSceneAnimationPrivate()
{
}

// ------------------------------------------------------------------------------------------------------------------------------

QGLSceneAnimation::QGLSceneAnimation(QObject *parent) :
    QObject(parent)
    ,d_ptr(new QGLSceneAnimationPrivate())
{
}

QGLSceneAnimation::QGLSceneAnimation(QObject *parent, const QString &name) :
    QObject(parent)
    ,d_ptr(new QGLSceneAnimationPrivate())
{
    Q_D(QGLSceneAnimation);
    d->m_name = name;
}

QGLSceneAnimation::~QGLSceneAnimation()
{
}

QString QGLSceneAnimation::name() const
{
    Q_D(const QGLSceneAnimation);
    return d->m_name;
}

qreal QGLSceneAnimation::duration() const
{
    return 0.0;
}

qreal QGLSceneAnimation::position() const
{
    return 0.0;
}

void QGLSceneAnimation::setPosition(qreal)
{
}

bool QGLSceneAnimation::loop() const
{
    return false;
}

void QGLSceneAnimation::setLoop(bool)
{
}

QList<QString> QGLSceneAnimation::affectedNodes() const
{
    return QList<QString>();
}

QList<QGLSceneAnimation::NodeTransform> QGLSceneAnimation::transformations() const
{
    return QList<QGLSceneAnimation::NodeTransform>();
}

void QGLSceneAnimation::play()
{
    qDebug("QGLSceneAnimation::play()");
}

void QGLSceneAnimation::stop()
{
    qDebug("QGLSceneAnimation::stop()");
}

void QGLSceneAnimation::pause(bool val)
{
    qDebug("QGLSceneAnimation::pause( %d )", val);
}

QT_END_NAMESPACE
