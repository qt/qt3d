// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
