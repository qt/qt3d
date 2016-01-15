/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DINPUT_QFIRSTPERSONCAMERACONTROLLER_H
#define QT3DINPUT_QFIRSTPERSONCAMERACONTROLLER_H

#include <Qt3DCore/QEntity>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
class QCamera;
}

namespace Qt3DInput {

class QFirstPersonCameraControllerPrivate;

class QFirstPersonCameraController : public Qt3DCore::QEntity
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QCamera *camera READ camera WRITE setCamera NOTIFY cameraChanged)
    Q_PROPERTY(float linearSpeed READ linearSpeed WRITE setLinearSpeed NOTIFY linearSpeedChanged)
    Q_PROPERTY(float lookSpeed READ lookSpeed WRITE setLookSpeed NOTIFY lookSpeedChanged)

public:
    explicit QFirstPersonCameraController(Qt3DCore::QNode *parent = Q_NULLPTR);
    ~QFirstPersonCameraController();

    Qt3DRender::QCamera *camera() const;
    float linearSpeed() const;
    float lookSpeed() const;

    void setCamera(Qt3DRender::QCamera *camera);
    void setLinearSpeed(float linearSpeed);
    void setLookSpeed(float lookSpeed);

Q_SIGNALS:
    void cameraChanged();
    void linearSpeedChanged();
    void lookSpeedChanged();

private:
    Q_DECLARE_PRIVATE(QFirstPersonCameraController)
    Q_PRIVATE_SLOT(d_func(),  void _q_onFrameUpdate(float))
    Q_PRIVATE_SLOT(d_func(),  void _q_onAxisValueChanged(QString, float))
    Q_PRIVATE_SLOT(d_func(),  void _q_onActionStarted(QString))
    Q_PRIVATE_SLOT(d_func(),  void _q_onActionFinished(QString))
};

} // Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QFIRSTPERSONCAMERACONTROLLER_H
