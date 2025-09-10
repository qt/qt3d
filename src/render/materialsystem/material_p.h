// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_MATERIAL_H
#define QT3DRENDER_RENDER_MATERIAL_H

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

#include <QVariant>

#include <Qt3DRender/private/backendnode_p.h>
#include <Qt3DRender/private/parameterpack_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QMaterial;
class QParameter;

namespace Render {

class Renderer;
class GraphicsContext;
class Technique;
class Effect;
class MaterialManager;

class Q_3DRENDERSHARED_PRIVATE_EXPORT Material : public BackendNode
{
public:
    Material();
    ~Material();
    void cleanup();

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

    QList<Qt3DCore::QNodeId> parameters() const;
    Qt3DCore::QNodeId effect() const;

private:
    ParameterPack m_parameterPack;
    Qt3DCore::QNodeId m_effectUuid;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_MATERIAL_H
