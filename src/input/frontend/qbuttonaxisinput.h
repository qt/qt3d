/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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
    Q_PROPERTY(QVector<int> buttons READ buttons WRITE setButtons NOTIFY buttonsChanged)
    Q_PROPERTY(float acceleration READ acceleration WRITE setAcceleration NOTIFY accelerationChanged)
    Q_PROPERTY(float deceleration READ deceleration WRITE setDeceleration NOTIFY decelerationChanged)

public:
    explicit QButtonAxisInput(Qt3DCore::QNode *parent = nullptr);
    ~QButtonAxisInput();

    float scale() const;
    QVector<int> buttons() const;

    float acceleration() const;
    float deceleration() const;

public Q_SLOTS:
    void setScale(float scale);
    void setButtons(const QVector<int> &buttons);

    void setAcceleration(float acceleration);
    void setDeceleration(float deceleration);

Q_SIGNALS:
    void scaleChanged(float scale);
    void buttonsChanged(const QVector<int> &buttons);

    void accelerationChanged(float acceleration);
    void decelerationChanged(float deceleration);

private:
    Q_DECLARE_PRIVATE(QButtonAxisInput)
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
};

} // Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QBUTTONAXISINPUT_H
