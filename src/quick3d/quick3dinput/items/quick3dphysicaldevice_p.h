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
****************************************************************************/

#ifndef QT3DINPUT_INPUT_QUICK_QUICK3DPHYSICALDEVICE_H
#define QT3DINPUT_INPUT_QUICK_QUICK3DPHYSICALDEVICE_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DInput/qabstractphysicaldevice.h>
#include <Qt3DInput/qaxissetting.h>
#include <QtCore/QObject>
#include <QtQml/QQmlListProperty>

#include <Qt3DQuickInput/private/qt3dquickinput_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {
namespace Quick {

class Q_3DQUICKINPUTSHARED_PRIVATE_EXPORT Quick3DPhysicalDevice : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Qt3DInput::QAxisSetting> axisSettings READ axisSettings CONSTANT)
public:
    explicit Quick3DPhysicalDevice(QObject *parent = 0);

    inline QAbstractPhysicalDevice *parentPhysicalDevice() const { return qobject_cast<QAbstractPhysicalDevice *>(parent()); }
    QQmlListProperty<Qt3DInput::QAxisSetting> axisSettings();

private:
    static void appendAxisSetting(QQmlListProperty<QAxisSetting> *list, QAxisSetting *axes);
    static QAxisSetting *axisSettingAt(QQmlListProperty<QAxisSetting> *list, int index);
    static int axisSettingsCount(QQmlListProperty<QAxisSetting> *list);
    static void clearAxisSettings(QQmlListProperty<QAxisSetting> *list);
};

} // namespace Quick
} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_INPUT_QUICK_QUICK3DPHYSICALDEVICE_H
