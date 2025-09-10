// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QORBITCAMERACONTROLLER_P_H
#define QT3DEXTRAS_QORBITCAMERACONTROLLER_P_H

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

#include <Qt3DExtras/private/qabstractcameracontroller_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

class QOrbitCameraControllerPrivate : public QAbstractCameraControllerPrivate
{
    Q_DECLARE_PUBLIC(QOrbitCameraController)

public:
    QOrbitCameraControllerPrivate();

    void init();

    float m_zoomInLimit;
    QVector3D m_upVector;
    bool m_inverseXTranslate, m_inverseYTranslate;
    bool m_inversePan, m_inverseTilt;
    bool m_zoomTranslateViewCenter;

private:
    QOrbitCameraController *q_ptr;
};

} // namespace Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QORBITCAMERACONTROLLER_P_H
