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

#ifndef QT3DINPUT_QINPUTDEVICEPLUGIN_P_H
#define QT3DINPUT_QINPUTDEVICEPLUGIN_P_H

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

#include <QtCore/QObject>
#include <QtCore/QFactoryInterface>
#include <QtCore/QtPlugin>

#include <Qt3DInput/private/qt3dinput_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QInputDeviceIntegration;

class Q_3DINPUTSHARED_PRIVATE_EXPORT QInputDevicePlugin : public QObject
{
    Q_OBJECT
public:
    explicit QInputDevicePlugin(QObject *parent = nullptr);
    ~QInputDevicePlugin();

    virtual QInputDeviceIntegration *create(const QString &key, const QStringList &paramList);
};

} // Qt3DInput

#define QInputDevicePlugin_iid "org.qt-project.Qt3DInput.QInputDevicePlugin 5.6"

QT_END_NAMESPACE

#endif // QT3DINPUT_QINPUTDEVICEPLUGIN_P_H
