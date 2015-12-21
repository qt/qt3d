/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "cameracontroller_p.h"

#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>

#include <qcamera.h>
#include <qcameralens.h>
#include <qentity.h>

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DInput {
namespace Input {

CameraController::CameraController(QObject *parent) :
    QObject(parent),
    m_camera( 0 ),
    m_vx( 0.0f ),
    m_vy( 0.0f ),
    m_vz( 0.0f ),
    m_viewCenterFixed( false ),
    m_panAngle( 0.0f ),
    m_tiltAngle( 0.0f ),
    m_leftButtonPressed( false ),
    m_orbitMode( false ),
    m_linearSpeed( 40.0f ),
    m_time( 0.0f ),
    m_orbitRate( -0.3f ),
    m_lookRate( 0.1f ),
    m_translateFast( false ),
    m_multisampleEnabled( true ),
    m_controlMode( FirstPerson ),
    m_firstPersonUp( QVector3D( 0.0f, 1.0f, 0.0f ) ),
    m_updateTimer(new QTimer(this))
{
    m_updateTimer->setInterval(16);
    connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(onUpdate()));
}

/*!
 * CameraController expects to find a Camera entity.
 * That means if you have built you Camera yourself using
 * an Entity, a CameraLens and a Transform instead of using the
 * ready made Camera element, it won't work.
 */
void CameraController::setCamera( Qt3DCore::QCamera* cam )
{
    m_camera = cam;
    m_cameraEntity = cam;

    if (m_camera)
        m_updateTimer->start();
    else
        m_updateTimer->stop();
}

Qt3DCore::QCamera *CameraController::camera() const
{
    return m_camera;
}

void CameraController::setLinearSpeed( float speed )
{
    if ( qFuzzyCompare( m_linearSpeed, speed ) )
        return;
    m_linearSpeed = speed;
    emit linearSpeedChanged(speed);
}

float CameraController::linearSpeed() const
{
    return m_linearSpeed;
}

float CameraController::orbitRate() const
{
    return m_orbitRate;
}

void CameraController::setOrbitRate( float rate )
{
    if ( qFuzzyCompare( m_orbitRate, rate ) )
        return;
    m_orbitRate = rate;
    emit orbitRateChanged(rate);
}

float CameraController::lookRate() const
{
    return m_lookRate;
}

void CameraController::setLookRate( float rate )
{
    if ( qFuzzyCompare( m_lookRate, rate ) )
        return;
    m_lookRate = rate;
    emit lookRateChanged(rate);
}

void CameraController::update(double dt)
{
    if ( !m_camera )
        return;

    if (m_translateFast)
        dt *= 10;

    // Update the camera position and orientation
    QCamera::CameraTranslationOption option = m_viewCenterFixed
                                               ? QCamera::DontTranslateViewCenter
                                               : QCamera::TranslateViewCenter;
    m_camera->translate(dt * QVector3D(m_vx, m_vy, m_vz), option);

    if (!qFuzzyIsNull(m_panAngle)) {
        m_camera->pan(m_panAngle);
        m_panAngle = 0.0f;
    }

    if (!qFuzzyIsNull(m_tiltAngle)) {
        m_camera->tilt(m_tiltAngle);
        m_tiltAngle = 0.0f;
    }
}

bool CameraController::keyPressEvent( QT_PREPEND_NAMESPACE(QKeyEvent*) e )
{
    m_translateFast = e->modifiers().testFlag(Qt::AltModifier);
    m_viewCenterFixed = e->modifiers().testFlag(Qt::ShiftModifier);

    switch ( e->key() )
    {
        case Qt::Key_Right:
            m_vx = m_linearSpeed;
            break;

        case Qt::Key_Left:
            m_vx = -m_linearSpeed;
            break;

        case Qt::Key_Up:
            m_vz = m_linearSpeed;
            break;

        case Qt::Key_Down:
            m_vz = -m_linearSpeed;
            break;

        case Qt::Key_PageUp:
            m_vy = m_linearSpeed;
            break;

        case Qt::Key_PageDown:
            m_vy = -m_linearSpeed;
            break;

        case Qt::Key_A:
            toggleMSAA();
            break;

        default:
            return false;
    }

    return true;
}

bool CameraController::keyReleaseEvent( QT_PREPEND_NAMESPACE(QKeyEvent*) e )
{
    switch ( e->key() )
    {
        case Qt::Key_Right:
        case Qt::Key_Left:
            m_vx = 0.0;
            break;

        case Qt::Key_Up:
        case Qt::Key_Down:
            m_vz = 0.0;
            break;

        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
            m_vy = 0.0;
            break;

        default:
            return false;
    }

    return true;
}

void CameraController::mousePressEvent( QT_PREPEND_NAMESPACE(QMouseEvent*) e )
{
    if ( e->button() == Qt::LeftButton )
    {
        m_leftButtonPressed = true;
        m_pos = m_prevPos = e->pos();
    }
    else if ( e->button() == Qt::RightButton )
    {
        m_orbitMode = true;
        m_pos = m_prevPos = e->pos();
    }
}

void CameraController::mouseReleaseEvent( QT_PREPEND_NAMESPACE(QMouseEvent*) e )
{
    if ( e->button() == Qt::LeftButton )
    {
        m_leftButtonPressed = false;
    }
    else if ( e->button() == Qt::RightButton )
    {
        m_orbitMode = false;
    }
}

void CameraController::mouseMoveEvent( QT_PREPEND_NAMESPACE(QMouseEvent*) e )
{
    if (!m_camera )
      return;

    if (!m_leftButtonPressed && !m_orbitMode)
        return;

    m_pos = e->pos();
    float dx = m_pos.x() - m_prevPos.x();
    float dy = -(m_pos.y() - m_prevPos.y());
    m_prevPos = m_pos;

    if (m_leftButtonPressed) {
        switch (m_controlMode) {
        case FreeLook:
            m_camera->pan(dx * m_lookRate);
            break;

        case FirstPerson:
            m_camera->pan(dx * m_lookRate, m_firstPersonUp);
            break;
        }

        m_camera->tilt(dy * m_lookRate);
    } else if (m_orbitMode) {
        switch (m_controlMode) {
        case FreeLook:
            m_camera->panAboutViewCenter(dx * m_orbitRate);
            break;

        case FirstPerson:
            m_camera->panAboutViewCenter(dx * m_orbitRate, m_firstPersonUp);
            break;
        }

        m_camera->tiltAboutViewCenter(dy * m_orbitRate);
    }
}

bool CameraController::isMultisampleEnabled() const
{
    return m_multisampleEnabled;
}

void CameraController::toggleMSAA()
{
    m_multisampleEnabled = !m_multisampleEnabled;
    emit multisampleEnabledChanged(m_multisampleEnabled);
}

void CameraController::setControlMode(ControlMode controlMode)
{
    if (controlMode != m_controlMode) {
        m_controlMode = controlMode;
        emit controlModeChanged(controlMode);
    }
}

CameraController::ControlMode CameraController::controlMode() const
{
    return m_controlMode;
}

void CameraController::setFirstPersonUpVector(const QVector3D &up)
{
    if (m_firstPersonUp != up) {
        m_firstPersonUp = up;
        emit firstPersonUpVectorChanged(up);
    }
}

QVector3D CameraController::firstPersonUpVector() const
{
    return m_firstPersonUp;
}

bool CameraController::eventFilter(QObject *receiver, QEvent *event)
{
    switch (event->type()) {
    case QEvent::MouseButtonPress:
        mousePressEvent(static_cast<QT_PREPEND_NAMESPACE(QMouseEvent*)>(event));
        return true;
    case QEvent::MouseButtonRelease:
        mouseReleaseEvent(static_cast<QT_PREPEND_NAMESPACE(QMouseEvent*)>(event));
        return true;
    case QEvent::MouseMove:
        mouseMoveEvent(static_cast<QT_PREPEND_NAMESPACE(QMouseEvent*)>(event));
        return true;
    case QEvent::KeyPress:
        return keyPressEvent(static_cast<QT_PREPEND_NAMESPACE(QKeyEvent)*>(event));
    case QEvent::KeyRelease:
        return keyReleaseEvent(static_cast<QT_PREPEND_NAMESPACE(QKeyEvent)*>(event));
    default:
        return QObject::eventFilter(receiver, event);
    }
}

void CameraController::onUpdate()
{
    update(1.0 / 60.0);
}

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE
