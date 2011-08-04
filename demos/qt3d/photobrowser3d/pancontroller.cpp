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

#include "pancontroller.h"
#include "qglview.h"
#include "qglcamera.h"
#include "photobrowser3dview.h"

#include <qmath.h>

#include <QtGlobal>
#include <QTime>

class PanControllerPrivate
{
public:
    PanControllerPrivate()
        : speed(0.0f)
        , angle(0.0f)
        , arrowDirection(Qt::NoArrow)
        , view(0)
        , maxSpeed(4.0f)
        , defaultDistance(0.0f)
        , panDistance(0.0f)
        , panViewAngle(M_PI / 4.0f)  // 45 degrees in radians
        , animating(false)
        , elapsed(0.0f)
    {
        panTime.start();
    }

    void calculateValues();

    qreal speed;
    qreal angle;
    Qt::ArrowType arrowDirection;
    QGLView *view;
    QTime panTime;
    qreal maxSpeed;
    qreal defaultDistance;
    qreal panDistance;
    qreal panViewAngle;
    bool animating;
    qreal elapsed;
};

PanController::PanController(QObject *parent)
    : QObject(parent)
    , d(new PanControllerPrivate)
{
    QGLView *v = qobject_cast<QGLView*>(parent);
    if (v)
        d->view = v;
}

PanController::~PanController()
{
    delete d;
}

qreal PanController::defaultDistance() const
{
    return d->defaultDistance;
}

void PanController::setDefaultDistance(qreal dist)
{
    d->defaultDistance = dist;
}

qreal PanController::panDistance() const
{
    return d->panDistance;
}

void PanController::setPanDistance(qreal dist)
{
    d->panDistance = dist;
}

qreal PanController::panViewAngle() const
{
    return d->panViewAngle;
}

void PanController::setPanViewAngle(qreal angle)
{
    d->panViewAngle = angle;
}

void PanController::setMaxSpeed(qreal maxSpeed)
{
    d->maxSpeed = maxSpeed;
}

qreal PanController::maxSpeed() const
{
    return d->maxSpeed;
}

void PanController::setSpeed(qreal speed)
{
    qreal t = d->panTime.restart();
    if (d->speed != speed)
    {
        d->speed = speed;
        d->angle = speed * d->panViewAngle;
        if (!qIsNull(d->speed))
            d->animating = true;
        d->elapsed += t;
        d->calculateValues();
        emit speedChanged();
    }
}

qreal PanController::speed() const
{
    return d->speed;
}

void PanController::pan()
{
    if (d->animating)
    {
        qreal t = d->panTime.restart();
        d->elapsed += t;
        // dont recalculate every single time
        // 30ms frame time == 33fps - more than enough
        if (d->elapsed > 30)
            d->calculateValues();

        PhotoBrowser3DView *view = qobject_cast<PhotoBrowser3DView*>(parent());
        Q_ASSERT(view);
        view->update();
    }
}

void PanControllerPrivate::calculateValues()
{
    if (view && animating)
    {
        QGLCamera *cam = view->camera();
        Q_ASSERT(cam);

        QVector3D c = cam->center();
        QVector3D e = cam->eye();

        if (qFuzzyIsNull(speed))
        {
            c.setX(e.x());
            e.setZ(defaultDistance);
        }
        else
        {
            // as speed goes from 0 -> 1, eye moves closer to z=0 plane
            e.setZ(defaultDistance - (speed * (defaultDistance - panDistance)));

            // the view angle is a direct function of the speed see setSpeed() above
            // and as view angle increases we look further along the x-axis
            qreal opp = (e.z() - c.z()) * qTan(angle);

            // velocity along the x axis is controlled by speed (a value from 0 -> 1
            // which is a modifier for the maxSpeed, a constant).  the velocity gives
            // us the incremental change in x for this unit time
            qreal dx = (speed * maxSpeed * elapsed);

            if (arrowDirection == Qt::LeftArrow)
            {
                e.setX(e.x() - dx);
                c.setX(e.x() - opp);
            }
            else if (arrowDirection == Qt::RightArrow)
            {
                e.setX(e.x() + dx);
                c.setX(e.x() + opp);
            }
        }
        cam->setEye(e);
        cam->setCenter(c);
    }
    elapsed = 0;
    if (qIsNull(speed))
        animating = false;
}

Qt::ArrowType PanController::direction() const
{
    return d->arrowDirection;
}

void PanController::setDirection(Qt::ArrowType arrow)
{
    Q_ASSERT(arrow == Qt::LeftArrow || arrow == Qt::RightArrow);
    d->arrowDirection = arrow;
}

QGLView *PanController::view() const
{
    return d->view;
}

void PanController::setView(QGLView *view)
{
    d->view = view;
}

