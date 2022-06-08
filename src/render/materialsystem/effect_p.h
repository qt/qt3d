// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_EFFECT_H
#define QT3DRENDER_RENDER_EFFECT_H

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

#include <Qt3DRender/private/backendnode_p.h>
#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DRender/private/parameterpack_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QTechnique;

namespace Render {

class Q_3DRENDERSHARED_PRIVATE_EXPORT Effect : public BackendNode
{
public:
    Effect();
    ~Effect();
    void cleanup();

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;
    void appendRenderTechnique(Qt3DCore::QNodeId t);

    QList<Qt3DCore::QNodeId> techniques() const;
    QList<Qt3DCore::QNodeId> parameters() const;

private:
    QList<Qt3DCore::QNodeId> m_techniques;
    ParameterPack m_parameterPack;
};

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_EFFECT_H
