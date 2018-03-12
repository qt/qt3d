/****************************************************************************
**
** Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
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

class ImGuiRenderer : public QObject {
    Q_OBJECT
public:
    ImGuiRenderer(Qt3DRender::Render::OpenGL::Renderer *renderer);

    void processEvent(QEvent *event);
    void renderDebugOverlay(const QVector<Render::OpenGL::RenderView *> &renderViews, const Render::OpenGL::RenderView *renderView, int jobsInLastFrame);

    void setCapabilities(const QString &capabilities);

private:
    bool newFrame(const Render::OpenGL::RenderView *renderView);
    void renderDrawList(ImDrawData *draw_data);
    void onMouseChange(QMouseEvent *event);
    void onWheel(QWheelEvent *event);
    void onKeyPressRelease(QKeyEvent *event);
    void showGLInfo();
    void showRenderDetails(const QVector<Render::OpenGL::RenderView *> &renderViews);

    bool createFontsTexture();
    bool createDeviceObjects();

    double       m_time = 0.;
    bool         m_mousePressed[3] = { false, false, false };
    float        m_mouseWheel;
    float        m_mouseWheelH;
    GLuint       m_fontTexture = 0;
    GLuint       m_shaderHandle = 0, m_vertHandle = 0, m_fragHandle = 0;
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
