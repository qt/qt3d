// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_QINPUTASPECT_H
#define QT3DINPUT_QINPUTASPECT_H

#include <Qt3DInput/qt3dinput_global.h>
#include <Qt3DCore/qabstractaspect.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QAbstractPhysicalDevice;
class QInputAspectPrivate;
class QInputDeviceIntegration;

class Q_3DINPUTSHARED_EXPORT QInputAspect : public Qt3DCore::QAbstractAspect
{
    Q_OBJECT
public:
    explicit QInputAspect(QObject *parent = nullptr);
    ~QInputAspect();

    QAbstractPhysicalDevice *createPhysicalDevice(const QString &name);
    QStringList availablePhysicalDevices() const;

private:
    std::vector<Qt3DCore::QAspectJobPtr> jobsToExecute(qint64 time) override;

    void onRegistered() override;
    void onUnregistered() override;
    void onEngineStartup() override;
    void jobsDone() override;

    Q_DECLARE_PRIVATE(QInputAspect)
    explicit QInputAspect(QInputAspectPrivate &dd, QObject *parent);
    friend class QInputDeviceIntegration;
};

} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QINPUTASPECT_H
