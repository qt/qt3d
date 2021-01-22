/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

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
