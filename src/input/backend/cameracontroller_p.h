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

#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QObject>

#include <QPoint>
#include <QtGui/qvector3d.h>

QT_BEGIN_NAMESPACE

class QMouseEvent;
class QWheelEvent;
class QKeyEvent;
class QTimer;

namespace Qt3DCore {
class QCamera;
class QEntity;
}

namespace Qt3DInput {
namespace Input {

class CameraController : public QObject
{
    Q_OBJECT

    Q_PROPERTY( float linearSpeed READ linearSpeed WRITE setLinearSpeed NOTIFY linearSpeedChanged )
    Q_PROPERTY( float orbitRate READ orbitRate WRITE setOrbitRate NOTIFY orbitRateChanged )
    Q_PROPERTY( float lookRate READ lookRate WRITE setLookRate NOTIFY lookRateChanged )
    Q_PROPERTY( bool multisampleEnabled READ isMultisampleEnabled NOTIFY multisampleEnabledChanged )

    Q_PROPERTY( ControlMode controlMode READ controlMode WRITE setControlMode NOTIFY controlModeChanged )
    Q_PROPERTY( QVector3D firstPersonUpVector READ firstPersonUpVector WRITE setFirstPersonUpVector NOTIFY firstPersonUpVectorChanged )

public:
    explicit CameraController(QObject *parent = 0);

    void setCamera( Qt3DCore::QCamera* cam );
    Qt3DCore::QCamera *camera() const;

    void setLinearSpeed( float speed );
    float linearSpeed() const;

    float orbitRate() const;
    void setOrbitRate( float rate );

    float lookRate() const;
    void setLookRate( float rate );

    void mousePressEvent( QMouseEvent* aEvent );
    void mouseReleaseEvent( QMouseEvent* aEvent );
    void mouseMoveEvent( QMouseEvent* aEvent );

    bool keyPressEvent( QKeyEvent* aEvent );
    bool keyReleaseEvent( QKeyEvent* aEvent );

    void update( double t );

    bool isMultisampleEnabled() const;

    enum ControlMode {
        FreeLook,
        FirstPerson
    };
    Q_ENUM(ControlMode)

    void setControlMode( ControlMode controlMode );
    ControlMode controlMode() const;

    void setFirstPersonUpVector( const QVector3D &up );
    QVector3D firstPersonUpVector() const;

public Q_SLOTS:
    void toggleMSAA();

protected:
    bool eventFilter(QObject *receiver, QEvent *event) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void linearSpeedChanged(float speed);
    void orbitRateChanged(float rate);
    void lookRateChanged(float rate);

    void multisampleEnabledChanged(bool enabled);

    void controlModeChanged(ControlMode controlMode);
    void firstPersonUpVectorChanged(const QVector3D &up);

private Q_SLOTS:
    void onUpdate();

private:
    Qt3DCore::QCamera* m_camera;
    Qt3DCore::QEntity* m_cameraEntity;

    float m_vx;
    float m_vy;
    float m_vz;
    bool m_viewCenterFixed;
    float m_panAngle;
    float m_tiltAngle;

    bool m_leftButtonPressed;
    QPoint m_prevPos;
    QPoint m_pos;
    bool m_orbitMode;

    float m_linearSpeed;
    float m_time;
    float m_orbitRate;
    float m_lookRate;

    bool m_translateFast;
    bool m_multisampleEnabled;

    ControlMode m_controlMode;
    QVector3D m_firstPersonUp;

    QTimer *m_updateTimer;
};

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // of CAMERA_CONTROLLER_H
