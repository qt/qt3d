// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_QAXISACCUMULATOR_H
#define QT3DINPUT_QAXISACCUMULATOR_H

#include <Qt3DInput/qt3dinput_global.h>
#include <Qt3DInput/qaxis.h>
#include <Qt3DCore/qcomponent.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QAxisAccumulatorPrivate;

class Q_3DINPUTSHARED_EXPORT QAxisAccumulator : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(Qt3DInput::QAxis *sourceAxis READ sourceAxis WRITE setSourceAxis NOTIFY sourceAxisChanged)
    Q_PROPERTY(SourceAxisType sourceAxisType READ sourceAxisType WRITE setSourceAxisType NOTIFY sourceAxisTypeChanged)
    Q_PROPERTY(float scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(float value READ value NOTIFY valueChanged)
    Q_PROPERTY(float velocity READ velocity NOTIFY velocityChanged)

public:
    enum SourceAxisType {
        Velocity,
        Acceleration
    };
    Q_ENUM(SourceAxisType) // LCOV_EXCL_LINE

    QAxisAccumulator(Qt3DCore::QNode *parent = nullptr);
    ~QAxisAccumulator();

    Qt3DInput::QAxis *sourceAxis() const;
    SourceAxisType sourceAxisType() const;
    float value() const;
    float velocity() const;
    float scale() const;

public Q_SLOTS:
    void setSourceAxis(Qt3DInput::QAxis *sourceAxis);
    void setSourceAxisType(QAxisAccumulator::SourceAxisType sourceAxisType);
    void setScale(float scale);

Q_SIGNALS:
    void sourceAxisChanged(Qt3DInput::QAxis *sourceAxis);
    void sourceAxisTypeChanged(QAxisAccumulator::SourceAxisType sourceAxisType);
    void valueChanged(float value);
    void velocityChanged(float value);
    void scaleChanged(float scale);

private:
    Q_DECLARE_PRIVATE(QAxisAccumulator)
};

} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QAXISACCUMULATOR_H
