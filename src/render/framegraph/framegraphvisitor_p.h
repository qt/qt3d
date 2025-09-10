// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_FRAMEGRAPHVISITOR_H
#define QT3DRENDER_RENDER_FRAMEGRAPHVISITOR_H

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

#include <qglobal.h>

#include <Qt3DCore/qaspectjob.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

class FrameGraphNode;
class Renderer;
class FrameGraphManager;

class Q_3DRENDERSHARED_PRIVATE_EXPORT FrameGraphVisitor
{
public:
    explicit FrameGraphVisitor(const FrameGraphManager *nodeManager);

    std::vector<FrameGraphNode *> &&traverse(FrameGraphNode *root);
    std::vector<FrameGraphNode *> &&takeEnablersToDisable();

private:
    void visit(Render::FrameGraphNode *node);

    const FrameGraphManager *m_manager;
    std::vector<FrameGraphNode *> m_leaves;
    std::vector<FrameGraphNode *> m_enablersToDisable;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_FRAMEGRAPHVISITOR_H
