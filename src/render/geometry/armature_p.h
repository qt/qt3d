// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_ARMATURE_H
#define QT3DRENDER_RENDER_ARMATURE_H

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
#include <Qt3DRender/private/uniform_p.h>
#include <Qt3DCore/qnodeid.h>
#include <QtGui/qmatrix4x4.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

class Q_3DRENDERSHARED_PRIVATE_EXPORT Armature : public BackendNode
{
public:
    Armature();

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;
    void cleanup();

    Qt3DCore::QNodeId skeletonId() const { return m_skeletonId; }

    // Called from jobs
    UniformValue &skinningPaletteUniform() { return m_skinningPaletteUniform; }
    const UniformValue &skinningPaletteUniform() const { return m_skinningPaletteUniform; }

private:
    Qt3DCore::QNodeId m_skeletonId;
    UniformValue m_skinningPaletteUniform;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_ARMATURE_H
