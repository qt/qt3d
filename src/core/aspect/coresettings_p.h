// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_RENDERSETTINGS_H
#define QT3DCORE_RENDERSETTINGS_H

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

#include <Qt3DCore/qbackendnode.h>
#include <Qt3DCore/qcoresettings.h>
#include <Qt3DCore/private/qt3dcore_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QCoreAspect;

class Q_3DCORE_PRIVATE_EXPORT CoreSettings : public QBackendNode
{
public:
    CoreSettings();

    void setAspect(QCoreAspect *aspect) { m_aspect = aspect; }
    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

private:
    QCoreAspect *m_aspect;
};

class CoreSettingsFunctor : public Qt3DCore::QBackendNodeMapper
{
public:
    explicit CoreSettingsFunctor(QCoreAspect *aspect);
    Qt3DCore::QBackendNode *create(Qt3DCore::QNodeId id) const override;
    Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const override;
    void destroy(Qt3DCore::QNodeId id) const override;

private:
    QCoreAspect *m_aspect;
    mutable CoreSettings *m_settings;
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_RENDERSETTINGS_H
