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
******************************************************************************/

#ifndef QT3DRENDER_RENDER_OPENGL_IMAGESUBMISSIONCONTEXT_P_H
#define QT3DRENDER_RENDER_OPENGL_IMAGESUBMISSIONCONTEXT_P_H

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

#include <Qt3DCore/QNodeId>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

class ShaderImage;

namespace OpenGL {

class GraphicsContext;
class GLTexture;

class Q_AUTOTEST_EXPORT ImageSubmissionContext
{
public:
    ImageSubmissionContext();

    void initialize(GraphicsContext *context);
    void endDrawing();
    int activateImage(ShaderImage *image, GLTexture *tex);
    void deactivateImages();

private:
    void decayImageScores();
    int assignUnitForImage(Qt3DCore::QNodeId shaderImageId);

    struct ActiveImage
    {
        Qt3DCore::QNodeId shaderImageId;
        GLTexture *texture = nullptr;
        int score = 0;
        bool pinned = false;
    };
    std::vector<ActiveImage> m_activeImages;
    GraphicsContext *m_ctx;
};

} // namespace OpenGL
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_OPENGL_IMAGESUBMISSIONCONTEXT_P_H
