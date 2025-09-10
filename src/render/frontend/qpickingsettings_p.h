// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QPICKINGSETTINGS_P_H
#define QT3DRENDER_QPICKINGSETTINGS_P_H

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
#include <Qt3DRender/qpickingsettings.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class Q_3DRENDERSHARED_PRIVATE_EXPORT QPickingSettingsPrivate : public Qt3DCore::QNodePrivate
{
public:
    QPickingSettingsPrivate();

    QPickingSettings::PickMethod m_pickMethod;
    QPickingSettings::PickResultMode m_pickResultMode;
    QPickingSettings::FaceOrientationPickingMode m_faceOrientationPickingMode;
    float m_worldSpaceTolerance;
};

} // namespace Qt3Drender

QT_END_NAMESPACE

#endif // QT3DRENDER_QPICKINGSETTINGS_P_H
