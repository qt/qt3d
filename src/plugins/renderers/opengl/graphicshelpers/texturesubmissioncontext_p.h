/****************************************************************************
**
** Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DRENDER_RENDER_OPENGL_TEXTURESUBMISSIONCONTEXT_H
#define QT3DRENDER_RENDER_OPENGL_TEXTURESUBMISSIONCONTEXT_H

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
#include <QVector>

QT_BEGIN_NAMESPACE

class QOpenGLContext;

namespace Qt3DRender {
namespace Render {
namespace OpenGL {

class GraphicsContext;
class GLTexture;

class Q_AUTOTEST_EXPORT TextureSubmissionContext
{
public:
    enum TextureScope
    {
        TextureScopeMaterial = 0,
        TextureScopeTechnique
        // per-pass for deferred rendering?
    };

    TextureSubmissionContext();
    ~TextureSubmissionContext();

    void initialize(GraphicsContext *context);
    void endDrawing();
    int activateTexture(TextureScope scope, QOpenGLContext *gl, GLTexture* tex);
    void deactivateTexture(GLTexture *tex);
    void deactivateTexturesWithScope(TextureScope ts);

private:
    void decayTextureScores();
    int assignUnitForTexture(GLTexture* tex);

    // active textures, indexed by texture unit
    struct ActiveTexture {
        GLTexture *texture = nullptr;
        int score = 0;
        TextureScope scope = TextureScopeMaterial;
        bool pinned = false;
    };
    QVector<ActiveTexture> m_activeTextures;
};

} // namespace OpenGL
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_OPENGL_TEXTURESUBMISSIONCONTEXT_H
