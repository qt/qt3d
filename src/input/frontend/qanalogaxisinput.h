// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_QANALOGAXISINPUT_H
#define QT3DINPUT_QANALOGAXISINPUT_H

#include <Qt3DInput/qt3dinput_global.h>
#include <Qt3DInput/QAbstractAxisInput>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QAnalogAxisInputPrivate;

class Q_3DINPUTSHARED_EXPORT QAnalogAxisInput : public QAbstractAxisInput
{
    Q_OBJECT
    Q_PROPERTY(int axis READ axis WRITE setAxis NOTIFY axisChanged)

public:
    explicit QAnalogAxisInput(Qt3DCore::QNode *parent = nullptr);
    ~QAnalogAxisInput();

    int axis() const;

public Q_SLOTS:
    void setAxis(int axis);

Q_SIGNALS:
    void axisChanged(int axis);

private:
    Q_DECLARE_PRIVATE(QAnalogAxisInput)
};

} // Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QANALOGAXISINPUT_H
