// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_QBUTTONAXISINPUT_H
#define QT3DINPUT_QBUTTONAXISINPUT_H

#include <Qt3DInput/qt3dinput_global.h>
#include <Qt3DInput/QAbstractAxisInput>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QButtonAxisInputPrivate;

class Q_3DINPUTSHARED_EXPORT QButtonAxisInput : public QAbstractAxisInput
{
    Q_OBJECT
    Q_PROPERTY(float scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(QList<int> buttons READ buttons WRITE setButtons NOTIFY buttonsChanged)
    Q_PROPERTY(float acceleration READ acceleration WRITE setAcceleration NOTIFY accelerationChanged)
    Q_PROPERTY(float deceleration READ deceleration WRITE setDeceleration NOTIFY decelerationChanged)

public:
    explicit QButtonAxisInput(Qt3DCore::QNode *parent = nullptr);
    ~QButtonAxisInput();

    float scale() const;
    QList<int> buttons() const;

    float acceleration() const;
    float deceleration() const;

public Q_SLOTS:
    void setScale(float scale);
    void setButtons(const QList<int> &buttons);

    void setAcceleration(float acceleration);
    void setDeceleration(float deceleration);

Q_SIGNALS:
    void scaleChanged(float scale);
    void buttonsChanged(const QList<int> &buttons);

    void accelerationChanged(float acceleration);
    void decelerationChanged(float deceleration);

private:
    Q_DECLARE_PRIVATE(QButtonAxisInput)
};

} // Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QBUTTONAXISINPUT_H
