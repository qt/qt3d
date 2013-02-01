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

#ifndef QGLCAMERAANIMATION_H
#define QGLCAMERAANIMATION_H

#include <Qt3D/qt3dglobal.h>

#include <QtCore/qabstractanimation.h>
#include <QtCore/qeasingcurve.h>
#include <QtGui/qvector3d.h>

QT_BEGIN_NAMESPACE

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

#endif
