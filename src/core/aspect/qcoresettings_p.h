// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QCORESETTINGS_P_H
#define QT3DCORE_QCORESETTINGS_P_H

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

#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/qcoresettings.h>
#include <Qt3DCore/qt3dcore_global.h>
#include <Qt3DCore/private/qcomponent_p.h>
#include <QByteArray>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class Q_3DCORESHARED_EXPORT QCoreSettingsPrivate : public Qt3DCore::QComponentPrivate
{
public:
    Q_DECLARE_PUBLIC(QCoreSettings)

    QCoreSettingsPrivate();

    static QCoreSettingsPrivate *get(QCoreSettings *q);

    bool m_boundingVolumesEnabled;
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QBUFFER_P_H
