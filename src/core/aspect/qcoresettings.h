// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
