// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_QLOGICALDEVICE_H
#define QT3DINPUT_QLOGICALDEVICE_H

#include <Qt3DInput/qt3dinput_global.h>
#include <Qt3DCore/qcomponent.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QLogicalDevicePrivate;
class QAction;
class QAxis;

class Q_3DINPUTSHARED_EXPORT QLogicalDevice : public Qt3DCore::QComponent
{
    Q_OBJECT
public:
    explicit QLogicalDevice(Qt3DCore::QNode *parent = nullptr);
    ~QLogicalDevice();

    void addAction(QAction *action);
    void removeAction(QAction *action);
    QList<QAction *> actions() const;

    void addAxis(QAxis *axis);
    void removeAxis(QAxis *axis);
    QList<QAxis *> axes() const;

private:
    Q_DECLARE_PRIVATE(QLogicalDevice)
};

} // Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QLOGICALDEVICE_H
