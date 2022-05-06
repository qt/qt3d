/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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
******************************************************************************/

#ifndef QT3DINPUT_QAXISSETTING_H
#define QT3DINPUT_QAXISSETTING_H

#include <Qt3DInput/qt3dinput_global.h>
#include <Qt3DCore/qnode.h>
#include <QtCore/qlist.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QAxisSettingPrivate;

class Q_3DINPUTSHARED_EXPORT QAxisSetting : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(float deadZoneRadius READ deadZoneRadius WRITE setDeadZoneRadius NOTIFY deadZoneRadiusChanged)
    Q_PROPERTY(QList<int> axes READ axes WRITE setAxes NOTIFY axesChanged)
    Q_PROPERTY(bool smooth READ isSmoothEnabled WRITE setSmoothEnabled NOTIFY smoothChanged)

public:
    explicit QAxisSetting(Qt3DCore::QNode *parent = nullptr);
    ~QAxisSetting();

    float deadZoneRadius() const;
    QList<int> axes() const;
    bool isSmoothEnabled() const;

public Q_SLOTS:
    void setDeadZoneRadius(float deadZoneRadius);
    void setAxes(const QList<int> &axes);
    void setSmoothEnabled(bool enabled);

Q_SIGNALS:
    void deadZoneRadiusChanged(float deadZoneRadius);
    void axesChanged(const QList<int> &axes);
    void smoothChanged(bool smooth);

private:
    Q_DECLARE_PRIVATE(QAxisSetting)
};

} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QAXISSETTING_H
