// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_QABSTRACTPHYSICALDEVICE
#define QT3DINPUT_QABSTRACTPHYSICALDEVICE

#include <Qt3DInput/qt3dinput_global.h>
#include <Qt3DCore/qnode.h>
#include <QtCore/qlist.h>
#include <QtCore/qobject.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QAxisSetting;
class QInputAspect;
class QAbstractPhysicalDevicePrivate;

class Q_3DINPUTSHARED_EXPORT QAbstractPhysicalDevice : public Qt3DCore::QNode
{
    Q_OBJECT
public:
    explicit QAbstractPhysicalDevice(Qt3DCore::QNode *parent = nullptr);
    ~QAbstractPhysicalDevice();

    virtual int axisCount() const;
    virtual int buttonCount() const;
    virtual QStringList axisNames() const;
    virtual QStringList buttonNames() const;

    virtual int axisIdentifier(const QString &name) const;
    virtual int buttonIdentifier(const QString &name) const;

    void addAxisSetting(QAxisSetting *axisSetting);
    void removeAxisSetting(QAxisSetting *axisSetting);
    QList<QAxisSetting *> axisSettings() const;

protected:
    QAbstractPhysicalDevice(QAbstractPhysicalDevicePrivate &dd, Qt3DCore::QNode *parent = nullptr);
    Q_DECLARE_PRIVATE(QAbstractPhysicalDevice)
};

} // Qt3DInput

QT_END_NAMESPACE


#endif // QT3DINPUT_QABSTRACTPHYSICALDEVICE

