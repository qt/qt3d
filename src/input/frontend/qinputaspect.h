/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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
****************************************************************************/

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
    QVector<Qt3DCore::QAspectJobPtr> jobsToExecute(qint64 time) override;

    void onRegistered() override;
    void onUnregistered() override;

    Q_DECLARE_PRIVATE(QInputAspect)
    explicit QInputAspect(QInputAspectPrivate &dd, QObject *parent);
    friend class QInputDeviceIntegration;
};

} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QINPUTASPECT_H
