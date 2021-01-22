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

#ifndef GENERICDEVICE_P_H
#define GENERICDEVICE_P_H

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

#include <Qt3DInput/private/qt3dinput_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class Q_3DINPUTSHARED_PRIVATE_EXPORT QGenericInputDevice : public QAbstractPhysicalDevice
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap axesMap READ axesMap WRITE setAxesMap NOTIFY axesMapChanged)
    Q_PROPERTY(QVariantMap buttonsMap READ buttonsMap WRITE setButtonsMap NOTIFY buttonsMapChanged)
public:
    QGenericInputDevice(Qt3DCore::QNode *parent = nullptr);

    QVariantMap axesMap() const;
    void setAxesMap(const QVariantMap &axesMap);
    QVariantMap buttonsMap() const;
    void setButtonsMap(const QVariantMap &buttonsMap);

Q_SIGNALS:
    void axesMapChanged();
    void buttonsMapChanged();
};

} // Qt3DInput

QT_END_NAMESPACE

#endif // GENERICDEVICE_P_H
