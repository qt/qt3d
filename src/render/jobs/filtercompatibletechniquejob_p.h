// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_FILTERCOMPATIBLETECHNIQUEJOB_H
#define QT3DRENDER_RENDER_FILTERCOMPATIBLETECHNIQUEJOB_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DCore/qaspectjob.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

#include <QSharedPointer>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

class TechniqueManager;
class AbstractRenderer;

class Q_3DRENDERSHARED_PRIVATE_EXPORT FilterCompatibleTechniqueJob : public Qt3DCore::QAspectJob
{
public:
    FilterCompatibleTechniqueJob();

    void setManager(TechniqueManager *managers);
    TechniqueManager *manager() const;

    void setRenderer(AbstractRenderer *renderer);
    AbstractRenderer *renderer() const;

    void run() override;

private:
    TechniqueManager *m_manager;
    AbstractRenderer *m_renderer;
};

typedef QSharedPointer<FilterCompatibleTechniqueJob> FilterCompatibleTechniqueJobPtr;

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_FILTERCOMPATIBLETECHNIQUEJOB_H
