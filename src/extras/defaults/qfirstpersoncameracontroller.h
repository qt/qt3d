/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#ifndef QT3DEXTRAS_QFIRSTPERSONCAMERACONTROLLER_H
#define QT3DEXTRAS_QFIRSTPERSONCAMERACONTROLLER_H

#include <Qt3DExtras/qabstractcameracontroller.h>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

class Q_3DEXTRASSHARED_EXPORT QFirstPersonCameraController : public QAbstractCameraController
{
    Q_OBJECT

public:
    explicit QFirstPersonCameraController(Qt3DCore::QNode *parent = nullptr);
    ~QFirstPersonCameraController();

private:
    void moveCamera(const QAbstractCameraController::InputState &state, float dt) override;
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QFIRSTPERSONCAMERACONTROLLER_H
