// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "glresourcemanagers_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace OpenGL {

GLResourceManagers::GLResourceManagers()
    : m_glBufferManager(new GLBufferManager())
    , m_glShaderManager(new GLShaderManager())
    , m_glTextureManager(new GLTextureManager())
    , m_glFenceManager(new GLFenceManager())
    , m_vaoManager(new VAOManager())
{
}

GLResourceManagers::~GLResourceManagers()
{
    delete m_vaoManager;
    delete m_glFenceManager;
    delete m_glTextureManager;
    delete m_glShaderManager;
    delete m_glBufferManager;
}

} // OpenGL

} // Render

} // Qt3DRender

QT_END_NAMESPACE
