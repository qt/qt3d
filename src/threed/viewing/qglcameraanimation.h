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

#ifndef QGLCAMERAANIMATION_H
#define QGLCAMERAANIMATION_H

#include <QtCore/qabstractanimation.h>
#include <QtCore/qeasingcurve.h>
#include <QtGui/qvector3d.h>
#include "qt3dglobal.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3D)

class QGLCamera;
class QGLCameraAnimationPrivate;

class Q_QT3D_EXPORT QGLCameraAnimation : public QAbstractAnimation
{
    Q_OBJECT
    Q_PROPERTY(QGLCamera *camera READ camera WRITE setCamera)
    Q_PROPERTY(QVector3D startEye READ startEye WRITE setStartEye)
    Q_PROPERTY(QVector3D startUpVector READ startUpVector WRITE setStartUpVector)
    Q_PROPERTY(QVector3D startCenter READ startCenter WRITE setStartCenter)
    Q_PROPERTY(QVector3D endEye READ endEye WRITE setEndEye)
    Q_PROPERTY(QVector3D endUpVector READ endUpVector WRITE setEndUpVector)
    Q_PROPERTY(QVector3D endCenter READ endCenter WRITE setEndCenter)
    Q_PROPERTY(int duration READ duration WRITE setDuration)
    Q_PROPERTY(QEasingCurve easingCurve READ easingCurve WRITE setEasingCurve)
public:
    explicit QGLCameraAnimation(QObject *parent = 0);
    ~QGLCameraAnimation();

    QGLCamera *camera() const;
    void setCamera(QGLCamera *camera);

    QVector3D startEye() const;
    void setStartEye(const QVector3D &eye);

    QVector3D startUpVector() const;
    void setStartUpVector(const QVector3D &upVector);

    QVector3D startCenter() const;
    void setStartCenter(const QVector3D &center);

    QVector3D endEye() const;
    void setEndEye(const QVector3D &eye);

    QVector3D endUpVector() const;
    void setEndUpVector(const QVector3D &upVector);

    QVector3D endCenter() const;
    void setEndCenter(const QVector3D &center);

    int duration() const;
    void setDuration(int duration);

    QEasingCurve easingCurve() const;
    void setEasingCurve(const QEasingCurve &easing);

protected:
    void updateCurrentTime(int currentTime);

private:
    QScopedPointer<QGLCameraAnimationPrivate> d_ptr;

    Q_DISABLE_COPY(QGLCameraAnimation)
    Q_DECLARE_PRIVATE(QGLCameraAnimation)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
