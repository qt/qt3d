// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QT3DINPUT_QAXIS_H
#define QT3DINPUT_QAXIS_H

#include <Qt3DInput/qt3dinput_global.h>
#include <Qt3DCore/qnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QAxisPrivate;
class QAbstractAxisInput;

class Q_3DINPUTSHARED_EXPORT QAxis : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(float value READ value NOTIFY valueChanged)
public:
    explicit QAxis(Qt3DCore::QNode *parent = nullptr);
    ~QAxis();

    void addInput(QAbstractAxisInput *input);
    void removeInput(QAbstractAxisInput *input);
    QList<QAbstractAxisInput *> inputs() const;

    float value() const;

Q_SIGNALS:
    void valueChanged(float value);

private:
    Q_DECLARE_PRIVATE(QAxis)
};

} // Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QAXIS_H
