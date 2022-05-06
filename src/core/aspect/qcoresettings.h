/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DCORE_QCORESETTINGS_H
#define QT3DCORE_QCORESETTINGS_H

#include <Qt3DCore/qcomponent.h>
#include <Qt3DCore/qt3dcore_global.h>
#include <QtCore/QSharedPointer>


QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QCoreSettingsPrivate;

class Q_3DCORESHARED_EXPORT QCoreSettings : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(bool boundingVolumesEnabled WRITE setBoundingVolumesEnabled READ boundingVolumesEnabled NOTIFY boundingVolumesEnabledChanged)
public:
    explicit QCoreSettings(Qt3DCore::QNode *parent = nullptr);
    ~QCoreSettings();

    bool boundingVolumesEnabled() const;

public Q_SLOTS:
    void setBoundingVolumesEnabled(bool boundingVolumesEnabled);

Q_SIGNALS:
    void boundingVolumesEnabledChanged(bool boundingVolumesEnabled);

private:
    Q_DECLARE_PRIVATE(QCoreSettings)
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QCORESETTINGS_H
