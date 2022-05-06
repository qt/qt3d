/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
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
**
**
**
******************************************************************************/

#ifndef QT3DINPUT_QABSTRACTPHYSICALDEVICE_P_H
#define QT3DINPUT_QABSTRACTPHYSICALDEVICE_P_H

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

#include <Qt3DInput/qt3dinput_global.h>
#include <QtCore/qhash.h>
#include <QtCore/qlist.h>
#include <QtCore/qmutex.h>

#include <Qt3DInput/private/qt3dinput_global_p.h>
#include <Qt3DCore/private/qnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QAxisSetting;

class Q_3DINPUTSHARED_PRIVATE_EXPORT QAbstractPhysicalDevicePrivate : public Qt3DCore::QNodePrivate
{
public:
    QAbstractPhysicalDevicePrivate();
    ~QAbstractPhysicalDevicePrivate();

    Q_DECLARE_PUBLIC(QAbstractPhysicalDevice)
    QList<QAxisSetting *> m_axisSettings;
    QHash<QString, int> m_axesHash;
    QHash<QString, int> m_buttonsHash;

    QList<QPair<int, qreal>> m_pendingAxisEvents;
    QList<QPair<int, qreal>> m_pendingButtonsEvents;

    void postAxisEvent(int axis, qreal value);
    void postButtonEvent(int button, qreal value);
};

}

QT_END_NAMESPACE

#endif // QT3DINPUT_QABSTRACTPHYSICALDEVICE_P_H

