// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_QMOUSEDEVICE_H
#define QT3DINPUT_QMOUSEDEVICE_H

#include <Qt3DInput/qabstractphysicaldevice.h>
#include <Qt3DInput/qt3dinput_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QMouseDevicePrivate;
class QMouseInput;

class Q_3DINPUTSHARED_EXPORT QMouseDevice : public Qt3DInput::QAbstractPhysicalDevice
{
    Q_OBJECT
    Q_PROPERTY(float sensitivity READ sensitivity WRITE setSensitivity NOTIFY sensitivityChanged)
    Q_PROPERTY(bool updateAxesContinuously READ updateAxesContinuously WRITE setUpdateAxesContinuously NOTIFY updateAxesContinuouslyChanged REVISION 15)
public:
    explicit QMouseDevice(Qt3DCore::QNode *parent = nullptr);
    ~QMouseDevice();

    enum Axis {
        X,
        Y,
        WheelX,
        WheelY
    };
    Q_ENUM(Axis) // LCOV_EXCL_LINE

    int axisCount() const final;
    int buttonCount() const final;
    QStringList axisNames() const final;
    QStringList buttonNames() const final;
    int axisIdentifier(const QString &name) const final;
    int buttonIdentifier(const QString &name) const final;

    float sensitivity() const;
    bool updateAxesContinuously() const;

public Q_SLOTS:
    void setSensitivity(float value);
    void setUpdateAxesContinuously(bool updateAxesContinuously);

Q_SIGNALS:
    void sensitivityChanged(float value);
    void updateAxesContinuouslyChanged(bool updateAxesContinuously);

private:
    Q_DECLARE_PRIVATE(QMouseDevice)
};

} // namespace Qt3DInput

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DInput::QMouseDevice*) // LCOV_EXCL_LINE

#endif // QT3DINPUT_QMOUSEDEVICE_H
