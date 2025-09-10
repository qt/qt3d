// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3D_RENDER_GL_IMGUIRENDER_H_
#define QT3D_RENDER_GL_IMGUIRENDER_H_

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

#include <QObject>
#include <QOpenGLFunctions>
#include <QPoint>
#include <memory>

struct ImDrawData;

QT_BEGIN_NAMESPACE

class QMouseEvent;
class QWheelEvent;
class QKeyEvent;
class QOpenGLExtraFunctions;
class QOpenGLShaderProgram;

namespace Qt3DCore {
class QServiceLocator;
}

namespace Qt3DRender {
namespace Render {
namespace OpenGL {
class RenderView;
class Renderer;
}

namespace Debug {

#define IMGUI_PERF_LOG_SIZE 30

class Q_AUTOTEST_EXPORT ImGuiRenderer : public QObject {
public:
    ImGuiRenderer(Qt3DRender::Render::OpenGL::Renderer *renderer);
    ~ImGuiRenderer();

    void processEvent(QEvent *event);
    void renderDebugOverlay(const std::vector<Render::OpenGL::RenderView *> &renderViews, const Render::OpenGL::RenderView *renderView, int jobsInLastFrame);

    void setCapabilities(const QString &capabilities);

private:
    bool newFrame(const Render::OpenGL::RenderView *renderView);
    void renderDrawList(ImDrawData *draw_data);
    void onMouseChange(QMouseEvent *event);
    void onWheel(QWheelEvent *event);
    void onKeyPressRelease(QKeyEvent *event);
    void showGLInfo();
    void showRenderDetails(const std::vector<Render::OpenGL::RenderView *> &renderViews);

    bool createFontsTexture();
    bool createDeviceObjects();

    double       m_time = 0.;
    bool         m_mousePressed[3] = { false, false, false };
    float        m_mouseWheel;
    float        m_mouseWheelH;
    GLuint       m_fontTexture = 0;
    GLuint       m_shaderHandle = 0;
    int          m_attribLocationTex = 0, m_attribLocationProjMtx = 0;
    int          m_attribLocationPosition = 0, m_attribLocationUV = 0, m_attribLocationColor = 0;
    unsigned int m_vboHandle = 0, m_vaoHandle = 0, m_elementsHandle = 0;

    Render::OpenGL::Renderer *m_renderer;
    QOpenGLExtraFunctions *m_funcs = nullptr;
    QOpenGLShaderProgram *m_shader = nullptr;

    bool         m_showGLInfoWindow = false;
    bool         m_showRenderDetailsWindow = false;

    float        m_fpsLog[IMGUI_PERF_LOG_SIZE];
    float        m_jobsLog[IMGUI_PERF_LOG_SIZE];
    std::pair<float, float> m_fpsRange;
    std::pair<float, float> m_jobsRange;

    QByteArray   m_capabilities;
};

} // namespace Debug
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3D_RENDER_GL_IMGUIRENDER_H_
