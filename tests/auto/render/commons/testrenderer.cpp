// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "testrenderer.h"

QT_BEGIN_NAMESPACE

TestRenderer::TestRenderer() = default;

TestRenderer::~TestRenderer()
{
}

void TestRenderer::markDirty(Qt3DRender::Render::AbstractRenderer::BackendNodeDirtySet changes, Qt3DRender::Render::BackendNode *node)
{
    Q_UNUSED(node);
    m_changes |= changes;
}

Qt3DRender::Render::AbstractRenderer::BackendNodeDirtySet TestRenderer::dirtyBits()
{
    return m_changes;
}

#if defined(QT_BUILD_INTERNAL)
void TestRenderer::clearDirtyBits(Qt3DRender::Render::AbstractRenderer::BackendNodeDirtySet changes)
{
    m_changes &= ~changes;
}
#endif

void TestRenderer::resetDirty()
{
    m_changes = {};
}

QVariant TestRenderer::executeCommand(const QStringList &args)
{
    Q_UNUSED(args);
    return QVariant();
}

void TestRenderer::setOffscreenSurfaceHelper(Qt3DRender::Render::OffscreenSurfaceHelper *helper)
{
    Q_UNUSED(helper);
}

QSurfaceFormat TestRenderer::format()
{
    return QSurfaceFormat();
}

QOpenGLContext *TestRenderer::shareContext() const
{
    return nullptr;
}

QT_END_NAMESPACE
