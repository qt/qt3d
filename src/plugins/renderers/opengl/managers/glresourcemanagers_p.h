/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DRENDER_RENDER_OPENGL_GLRESOURCEMANAGERS_P_H
#define QT3DRENDER_RENDER_OPENGL_GLRESOURCEMANAGERS_P_H

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

#include <Qt3DRender/private/qt3drender_global_p.h>
#include <Qt3DCore/private/qresourcemanager_p.h>
#include <gltexture_p.h>
#include <glbuffer_p.h>
#include <glfence_p.h>
#include <openglvertexarrayobject_p.h>
#include <glshader_p.h>
#include <Qt3DRender/private/apishadermanager_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

namespace OpenGL {

class Q_AUTOTEST_EXPORT GLFenceManager : public QHash<Qt3DCore::QNodeId, GLFence>
{
};

class Q_AUTOTEST_EXPORT VAOManager : public Qt3DCore::QResourceManager<
        OpenGLVertexArrayObject,
        VAOIdentifier,
        Qt3DCore::NonLockingPolicy>
{
public:
    VAOManager() {}
};

class Q_AUTOTEST_EXPORT GLBufferManager : public Qt3DCore::QResourceManager<
        GLBuffer,
        Qt3DCore::QNodeId,
        Qt3DCore::NonLockingPolicy>
{
};

class Q_AUTOTEST_EXPORT GLTextureManager : public Qt3DCore::QResourceManager<
        GLTexture,
        Qt3DCore::QNodeId,
        Qt3DCore::NonLockingPolicy>
{
public:
    QHash<GLTexture *, Qt3DCore::QNodeId> texNodeIdForGLTexture;
};

class Q_AUTOTEST_EXPORT GLShaderManager : public APIShaderManager<GLShader>
{
public:
    explicit GLShaderManager()
        : APIShaderManager<GLShader>()
    {}
};


class Q_AUTOTEST_EXPORT GLResourceManagers
{
public:
    GLResourceManagers();
    ~GLResourceManagers();

    inline VAOManager *vaoManager() const noexcept { return m_vaoManager; }
    inline GLShaderManager *glShaderManager() const noexcept { return m_glShaderManager; }
    inline GLTextureManager *glTextureManager() const noexcept { return m_glTextureManager; }
    inline GLBufferManager *glBufferManager() const noexcept { return m_glBufferManager; }
    inline GLFenceManager *glFenceManager() const noexcept { return m_glFenceManager; }

private:
    GLBufferManager *m_glBufferManager;
    GLShaderManager *m_glShaderManager;
    GLTextureManager *m_glTextureManager;
    GLFenceManager *m_glFenceManager;
    VAOManager *m_vaoManager;
};

} // OpenGL

} // Render

} // Qt3DRender

Q_DECLARE_RESOURCE_INFO(Qt3DRender::Render::OpenGL::OpenGLVertexArrayObject, Q_REQUIRES_CLEANUP)

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_OPENGL_GLRESOURCEMANAGERS_P_H
