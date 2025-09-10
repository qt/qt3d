// Copyright (C) 2016 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_LIGHTGATHERER_P_H
#define QT3DRENDER_RENDER_LIGHTGATHERER_P_H

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

#include <Qt3DCore/qaspectjob.h>
#include <Qt3DRender/private/lightsource_p.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class EntityManager;
class EnvironmentLight;

class Q_3DRENDERSHARED_PRIVATE_EXPORT LightGatherer : public Qt3DCore::QAspectJob
{
public:
    LightGatherer();

    inline void setManager(EntityManager *manager) noexcept { m_manager = manager; }
    inline std::vector<LightSource> &lights() { return m_lights; }
    inline EnvironmentLight *environmentLight() const { return m_environmentLight; }

    void run() override;

private:
    EntityManager *m_manager;
    std::vector<LightSource> m_lights;
    EnvironmentLight *m_environmentLight;
};

typedef QSharedPointer<LightGatherer> LightGathererPtr;

} // Render

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_LIGHTGATHERER_P_H
