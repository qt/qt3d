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

/*
 * Based on https://github.com/seanchas116/qtimgui/
 *
 * MIT License https://github.com/seanchas116/qtimgui/blob/master/LICENSE
 *
 */

#include "imguirenderer_p.h"
#include <renderview_p.h>
#include <renderer_p.h>
#include <submissioncontext_p.h>
#include <Qt3DRender/private/geometryrenderermanager_p.h>

#include <QDateTime>
#include <QGuiApplication>
#include <QMouseEvent>
#include <QClipboard>
#include <QCursor>
#include <QOpenGLExtraFunctions>

#include "imgui.h"

#ifndef GL_VERTEX_ARRAY_BINDING
// just for building on some platforms, won't run anyway as this requires GL/ES > 2
#define GL_VERTEX_ARRAY_BINDING           0x85B5
#endif

QT_BEGIN_NAMESPACE

using namespace Qt3DRender;
using namespace Render;
using namespace Render::Debug;
using namespace Render::OpenGL;

namespace {

    const QHash<int, ImGuiKey> keyMap = {
        { Qt::Key_Tab, ImGuiKey_Tab },
        { Qt::Key_Left, ImGuiKey_LeftArrow },
        { Qt::Key_Right, ImGuiKey_RightArrow },
        { Qt::Key_Up, ImGuiKey_UpArrow },
        { Qt::Key_Down, ImGuiKey_DownArrow },
        { Qt::Key_PageUp, ImGuiKey_PageUp },
        { Qt::Key_PageDown, ImGuiKey_PageDown },
        { Qt::Key_Home, ImGuiKey_Home },
        { Qt::Key_End, ImGuiKey_End },
        { Qt::Key_Delete, ImGuiKey_Delete },
        { Qt::Key_Backspace, ImGuiKey_Backspace },
        { Qt::Key_Enter, ImGuiKey_Enter },
        { Qt::Key_Escape, ImGuiKey_Escape },
        { Qt::Key_A, ImGuiKey_A },
        { Qt::Key_C, ImGuiKey_C },
        { Qt::Key_V, ImGuiKey_V },
        { Qt::Key_X, ImGuiKey_X },
        { Qt::Key_Y, ImGuiKey_Y },
        { Qt::Key_Z, ImGuiKey_Z },
        };

    QByteArray g_currentClipboardText;

    // doesn't handle primitive restart when using indexes
    int vertexToPrimitiveCount(Qt3DRender::QGeometryRenderer::PrimitiveType primitiveType, int numVertices) {
        int nPrimitives = 0;
        switch (primitiveType) {
        case QGeometryRenderer::Points:
        case QGeometryRenderer::LineLoop: nPrimitives += numVertices; break;
        case QGeometryRenderer::Triangles: nPrimitives += numVertices / 3; break;
        case QGeometryRenderer::Lines: nPrimitives += numVertices / 2; break;
        case QGeometryRenderer::TriangleFan:
        case QGeometryRenderer::TriangleStrip:
        case QGeometryRenderer::LineStrip: nPrimitives += numVertices - 1; break;
        case QGeometryRenderer::TrianglesAdjacency: nPrimitives += numVertices / 6; break;
        case QGeometryRenderer::TriangleStripAdjacency:
        case QGeometryRenderer::LineStripAdjacency: nPrimitives += numVertices / 2 - 1; break;
        case QGeometryRenderer::LinesAdjacency: nPrimitives += numVertices / 4; break;
        case QGeometryRenderer::Patches: nPrimitives += 1;
        }
        return nPrimitives;
    }

    const char *primitiveTypeName(Qt3DRender::QGeometryRenderer::PrimitiveType primitiveType) {
        switch (primitiveType) {
        case QGeometryRenderer::Points: return "Points";
        case QGeometryRenderer::LineLoop: return "LineLoop";
        case QGeometryRenderer::Triangles: return "Triangles";
        case QGeometryRenderer::TrianglesAdjacency: return "TriangleAdjacency";
        case QGeometryRenderer::TriangleFan: return "TriangleFan";
        case QGeometryRenderer::TriangleStrip: return "TriangleStrip";
        case QGeometryRenderer::TriangleStripAdjacency: return "TriangleStringAdjacency";
        case QGeometryRenderer::LineStrip: return "LineStrip";
        case QGeometryRenderer::LineStripAdjacency: return "LineStripAdjacency";
        case QGeometryRenderer::Lines: return "Lines";
        case QGeometryRenderer::LinesAdjacency: return "LinesAdjacency";
        case QGeometryRenderer::Patches: return "Patches";
        }
        return "";
    }
}

ImGuiRenderer::ImGuiRenderer(Qt3DRender::Render::OpenGL::Renderer *renderer)
    : m_renderer(renderer)
{
    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    for (ImGuiKey key : keyMap.values())
        io.KeyMap[key] = key;

#ifndef QT_NO_CLIPBOARD
    io.SetClipboardTextFn = [](void *user_data, const char *text) {
        Q_UNUSED(user_data)
        QGuiApplication::clipboard()->setText(QString::fromLatin1(text));
    };
    io.GetClipboardTextFn = [](void *user_data) {
        Q_UNUSED(user_data)
        g_currentClipboardText = QGuiApplication::clipboard()->text().toUtf8();
        return static_cast<const char *>(g_currentClipboardText.data());
    };
#endif

    std::fill(std::begin(m_fpsLog), std::end(m_fpsLog), 0.f);
    std::fill(std::begin(m_jobsLog), std::end(m_jobsLog), 0.f);
    m_fpsRange.first = m_fpsRange.second = 0.f;
    m_jobsRange.first = m_jobsRange.second = 0.f;
}

void ImGuiRenderer::renderDebugOverlay(const QVector<RenderView *> &renderViews, const RenderView *renderView, int jobsInLastFrame)
{
    if (!newFrame(renderView))
        return;

    const int renderViewsCount = renderViews.size();

    int logIndex = qMin(IMGUI_PERF_LOG_SIZE - 1, ImGui::GetFrameCount());
    if (logIndex == IMGUI_PERF_LOG_SIZE - 1) {
        std::rotate(std::begin(m_fpsLog), std::begin(m_fpsLog) + 1, std::end(m_fpsLog));
        std::rotate(std::begin(m_jobsLog), std::begin(m_jobsLog) + 1, std::end(m_jobsLog));
    }
    m_fpsLog[logIndex] = ImGui::GetIO().Framerate;
    m_fpsRange.first = m_fpsRange.second = 0.f;
    for (float v: m_fpsLog) {
        m_fpsRange.first = qMin(m_fpsRange.first, qMax(v - 5.f, 0.f));
        m_fpsRange.second = qMax(m_fpsRange.second, v + 2.f);
    }
    m_jobsLog[logIndex] = jobsInLastFrame;
    m_jobsRange.first = m_jobsRange.second = 0.f;
    for (float v: m_jobsLog) {
        m_jobsRange.first = qMin(m_jobsRange.first, qMax(v - 5.f, 0.f));
        m_jobsRange.second = qMax(m_jobsRange.second, v + 2.f);
    }

    {
        ImGui::Begin("Qt3D Profiling");
        char caption[50];
        sprintf(caption, "Avg %.3f ms/frame (%.1f FPS)", static_cast<double>(1000.0f / ImGui::GetIO().Framerate), static_cast<double>(ImGui::GetIO().Framerate));
        ImGui::PlotLines("FPS", m_fpsLog, logIndex + 1, 0, caption, m_fpsRange.first, m_fpsRange.second, ImVec2(0, 80));
        ImGui::PlotHistogram("Jobs", m_jobsLog, logIndex + 1, 0, nullptr, m_jobsRange.first, m_jobsRange.second, ImVec2(0, 80));

        int nCommands = 0;
        int nVertices = 0;
        int nPrimitives = 0;
        QSet<HGeometryRenderer> inUseGeometries;
        QSet<Qt3DCore::QNodeId> inUseTextures;
        for (int j=0; j<renderViewsCount; j++) {
            RenderView *rv = renderViews.at(j);
            nCommands += rv->commandCount();
            rv->forEachCommand([&] (const RenderCommand &command) {
                if (command.m_type != RenderCommand::Draw)
                    return;
                nVertices += command.m_primitiveCount;
                nPrimitives += vertexToPrimitiveCount(command.m_primitiveType, command.m_primitiveCount);
                inUseGeometries.insert(command.m_geometryRenderer);
                const auto &textures = command.m_parameterPack.textures();
                for (const auto &ns: textures)
                    inUseTextures.insert(ns.nodeId);
            });
        }

        auto columnNumber = [](int i) {
            if (i == 0)
                ImGui::Text("--");
            else
                ImGui::Text("  %d", i);
            ImGui::NextColumn();
        };
        auto column2Numbers = [](int i, int of) {
            if (of == 0)
                ImGui::Text("--");
            else
                ImGui::Text("  %d of %d", i, of);
            ImGui::NextColumn();
        };

        ImGui::Columns(5);
        ImGui::Separator();
        for (auto s: {"Jobs", "RV", "Cmds", "Verts", "Prims"}) {
            ImGui::Text("#%s", s);
            ImGui::NextColumn();
        }
        for (auto s: {jobsInLastFrame, renderViewsCount, nCommands, nVertices, nPrimitives})
            columnNumber(s);

        ImGui::Columns(3);
        ImGui::Separator();
        for (auto s: {"Entities", "Geometries", "Textures"}) {
            ImGui::Text("#%s", s);
            ImGui::NextColumn();
        }
        columnNumber(m_renderer->nodeManagers()->renderNodesManager()->count());
        column2Numbers(inUseGeometries.size(), m_renderer->nodeManagers()->geometryRendererManager()->count());
        column2Numbers(inUseTextures.size(), m_renderer->nodeManagers()->textureManager()->count());

        ImGui::Columns(1);
        ImGui::Separator();

        bool pj = m_renderer->services()->systemInformation()->isTraceEnabled();
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Profiling: ");
        ImGui::SameLine();
        if (ImGui::Checkbox("Jobs", &pj))
            QMetaObject::invokeMethod(m_renderer->services()->systemInformation(), "setTraceEnabled", Qt::QueuedConnection, Q_ARG(bool, pj));
        ImGui::SameLine();
        if (ImGui::Button("Reveal"))
            QMetaObject::invokeMethod(m_renderer->services()->systemInformation(), "revealLogFolder", Qt::QueuedConnection);

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Show:");
        ImGui::SameLine();
        if (ImGui::Button("GL Info"))
            m_showGLInfoWindow = !m_showGLInfoWindow;
        ImGui::SameLine();
        if (ImGui::Button("Render Views"))
            m_showRenderDetailsWindow = !m_showRenderDetailsWindow;

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Dump:");
        ImGui::SameLine();
        if (ImGui::Button("SceneGraph##1"))
            QMetaObject::invokeMethod(m_renderer->services()->systemInformation(), "dumpCommand",
                                      Qt::QueuedConnection, Q_ARG(QString, QLatin1String("render scenegraph")));
        ImGui::SameLine();
        if (ImGui::Button("FrameGraph##1"))
            QMetaObject::invokeMethod(m_renderer->services()->systemInformation(), "dumpCommand",
                                      Qt::QueuedConnection, Q_ARG(QString, QLatin1String("render framegraph")));
        ImGui::SameLine();
        if (ImGui::Button("Render Views##1"))
            QMetaObject::invokeMethod(m_renderer->services()->systemInformation(), "dumpCommand",
                                      Qt::QueuedConnection, Q_ARG(QString, QLatin1String("render framepaths")));

        ImGui::AlignTextToFramePadding();
        ImGui::Text("     ");
        ImGui::SameLine();
        if (ImGui::Button("Filter State##1"))
            QMetaObject::invokeMethod(m_renderer->services()->systemInformation(), "dumpCommand",
                                      Qt::QueuedConnection, Q_ARG(QString, QLatin1String("render filterstates")));
        ImGui::SameLine();
        if (ImGui::Button("JobsGraph##1"))
            QMetaObject::invokeMethod(m_renderer->services()->systemInformation(), "dumpCommand",
                                      Qt::QueuedConnection, Q_ARG(QString, QLatin1String("dump jobs")));

        ImGui::End();

        if (m_showGLInfoWindow)
           showGLInfo();
        if (m_showRenderDetailsWindow)
            showRenderDetails(renderViews);
    }

    ImGui::Render();
    renderDrawList(ImGui::GetDrawData());
}

void ImGuiRenderer::setCapabilities(const QString &capabilities)
{
    m_capabilities = capabilities.toLatin1();
}

void ImGuiRenderer::showGLInfo()
{
    ImGui::Begin("Open GL Details", &m_showGLInfoWindow);
    ImGui::Text("%s", m_capabilities.data());
    ImGui::End();
}

void ImGuiRenderer::showRenderDetails(const QVector<RenderView *> &renderViews)
{
    ImGui::Begin("Render Views", &m_showRenderDetailsWindow);

    int i = 1;
    for (const RenderView *view: renderViews) {
        QString label(QLatin1String("View ") + QString::number(i++));
        if (ImGui::TreeNode(label.toLatin1().data())) {
            ImGui::Text("Viewport: (%.1f, %.1f, %.1f, %.1f)", view->viewport().x(), view->viewport().y(),
                        view->viewport().width(), view->viewport().height());
            ImGui::Text("Surface Size: (%d, %d)", view->surfaceSize().width(), view->surfaceSize().height());
            ImGui::Text("Pixel Ratio: %.1f", view->devicePixelRatio());
            ImGui::Text("No Draw: %s", view->noDraw() ? "TRUE" : "FALSE");
            ImGui::Text("Frustum Culling: %s", view->frustumCulling() ? "TRUE" : "FALSE");
            ImGui::Text("Compute: %s", view->isCompute() ? "TRUE" : "FALSE");
            ImGui::Text("Clear Depth Value: %f", static_cast<double>(view->clearDepthValue()));
            ImGui::Text("Clear Stencil Value: %d", view->clearStencilValue());
            int j = 1;

            view->forEachCommand([&] (const RenderCommand &command) {
                GeometryRenderer *rGeometryRenderer = m_renderer->nodeManagers()->data<GeometryRenderer, GeometryRendererManager>(command.m_geometryRenderer);
                QString label = QString(QLatin1String("Command %1 {%2}")).arg(QString::number(j++), QString::number(rGeometryRenderer->peerId().id()));
                if (ImGui::TreeNode(label.toLatin1().data())) {
                    ImGui::Text("Primitive Type: %s %s", primitiveTypeName(command.m_primitiveType),
                                command.m_drawIndexed ? "(indexed)" : "");
                    ImGui::Text("# Vertices: %d", command.m_primitiveCount);
                    ImGui::Text("# Primitives: %d", vertexToPrimitiveCount(command.m_primitiveType, command.m_primitiveCount));
                    ImGui::Text("# Instances: %d", command.m_instanceCount);
                    ImGui::TreePop();
                }
            });
            ImGui::TreePop();
            ImGui::Separator();
        }
    }

    if (ImGui::Button("Dump"))
        QMetaObject::invokeMethod(m_renderer->services()->systemInformation(), "dumpCommand",
                                  Qt::QueuedConnection, Q_ARG(QString, QLatin1String("render rendercommands")));
    ImGui::End();
}

void ImGuiRenderer::renderDrawList(ImDrawData *draw_data)
{
    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    ImGuiIO& io = ImGui::GetIO();
    int fb_width = int(io.DisplaySize.x * io.DisplayFramebufferScale.x);
    int fb_height = int(io.DisplaySize.y * io.DisplayFramebufferScale.y);
    if (fb_width == 0 || fb_height == 0)
        return;
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    // Backup GL state
    GLint last_active_texture; m_funcs->glGetIntegerv(GL_ACTIVE_TEXTURE, &last_active_texture);
    m_funcs->glActiveTexture(GL_TEXTURE0);
    GLint last_program; m_funcs->glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
    GLint last_texture; m_funcs->glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    GLint last_array_buffer; m_funcs->glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    GLint last_element_array_buffer; m_funcs->glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
    GLint last_vertex_array; m_funcs->glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
    GLint last_blend_src_rgb; m_funcs->glGetIntegerv(GL_BLEND_SRC_RGB, &last_blend_src_rgb);
    GLint last_blend_dst_rgb; m_funcs->glGetIntegerv(GL_BLEND_DST_RGB, &last_blend_dst_rgb);
    GLint last_blend_src_alpha; m_funcs->glGetIntegerv(GL_BLEND_SRC_ALPHA, &last_blend_src_alpha);
    GLint last_blend_dst_alpha; m_funcs->glGetIntegerv(GL_BLEND_DST_ALPHA, &last_blend_dst_alpha);
    GLint last_blend_equation_rgb; m_funcs->glGetIntegerv(GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
    GLint last_blend_equation_alpha; m_funcs->glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
    GLint last_viewport[4]; m_funcs->glGetIntegerv(GL_VIEWPORT, last_viewport);
    GLint last_scissor_box[4]; m_funcs->glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
    GLboolean last_enable_blend = m_funcs->glIsEnabled(GL_BLEND);
    GLboolean last_enable_cull_face = m_funcs->glIsEnabled(GL_CULL_FACE);
    GLboolean last_enable_depth_test = m_funcs->glIsEnabled(GL_DEPTH_TEST);
    GLboolean last_enable_scissor_test = m_funcs->glIsEnabled(GL_SCISSOR_TEST);

    // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
    m_funcs->glEnable(GL_BLEND);
    m_funcs->glBlendEquation(GL_FUNC_ADD);
    m_funcs->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_funcs->glDisable(GL_CULL_FACE);
    m_funcs->glDisable(GL_DEPTH_TEST);
    m_funcs->glEnable(GL_SCISSOR_TEST);

    // Setup viewport, orthographic projection matrix
    m_funcs->glViewport(0, 0, static_cast<GLsizei>(fb_width), static_cast<GLsizei>(fb_height));
    const float ortho_projection[4][4] = {
            { 2.0f/io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
            { 0.0f,                  2.0f/-io.DisplaySize.y, 0.0f, 0.0f },
            { 0.0f,                  0.0f,                  -1.0f, 0.0f },
            {-1.0f,                  1.0f,                   0.0f, 1.0f },
    };
    m_funcs->glUseProgram(m_shaderHandle);
    m_funcs->glUniform1i(m_attribLocationTex, 0);
    m_funcs->glUniformMatrix4fv(m_attribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
    m_funcs->glBindVertexArray(m_vaoHandle);

    for (int n = 0; n < draw_data->CmdListsCount; n++) {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawIdx* idx_buffer_offset = nullptr;

        m_funcs->glBindBuffer(GL_ARRAY_BUFFER, m_vboHandle);
        m_funcs->glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(cmd_list->VtxBuffer.Size) * sizeof(ImDrawVert), static_cast<const GLvoid*>(cmd_list->VtxBuffer.Data), GL_STREAM_DRAW);

        m_funcs->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementsHandle);
        m_funcs->glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(cmd_list->IdxBuffer.Size) * sizeof(ImDrawIdx), static_cast<const GLvoid*>(cmd_list->IdxBuffer.Data), GL_STREAM_DRAW);

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback) {
                pcmd->UserCallback(cmd_list, pcmd);
            } else {
                m_funcs->glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
                m_funcs->glScissor(static_cast<int>(pcmd->ClipRect.x), static_cast<int>(fb_height - pcmd->ClipRect.w),
                                   static_cast<int>(pcmd->ClipRect.z - pcmd->ClipRect.x), static_cast<int>(pcmd->ClipRect.w - pcmd->ClipRect.y));
                m_funcs->glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(pcmd->ElemCount), sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
            }
            idx_buffer_offset += pcmd->ElemCount;
        }
    }

    // Restore modified GL state
    m_funcs->glUseProgram(last_program);
    m_funcs->glBindTexture(GL_TEXTURE_2D, last_texture);
    m_funcs->glActiveTexture(last_active_texture);
    m_funcs->glBindVertexArray(last_vertex_array);
    m_funcs->glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
    m_funcs->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
    m_funcs->glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
    m_funcs->glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
    if (last_enable_blend)
        m_funcs->glEnable(GL_BLEND); else m_funcs->glDisable(GL_BLEND);
    if (last_enable_cull_face)
        m_funcs->glEnable(GL_CULL_FACE); else m_funcs->glDisable(GL_CULL_FACE);
    if (last_enable_depth_test)
        m_funcs->glEnable(GL_DEPTH_TEST); else m_funcs->glDisable(GL_DEPTH_TEST);
    if (last_enable_scissor_test)
        m_funcs->glEnable(GL_SCISSOR_TEST); else m_funcs->glDisable(GL_SCISSOR_TEST);
    m_funcs->glViewport(last_viewport[0], last_viewport[1], static_cast<GLsizei>(last_viewport[2]), static_cast<GLsizei>(last_viewport[3]));
    m_funcs->glScissor(last_scissor_box[0], last_scissor_box[1], static_cast<GLsizei>(last_scissor_box[2]), static_cast<GLsizei>(last_scissor_box[3]));
}

bool ImGuiRenderer::createFontsTexture()
{
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.

    // Upload texture to graphics system
    GLint last_texture;
    m_funcs->glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    m_funcs->glGenTextures(1, &m_fontTexture);
    m_funcs->glBindTexture(GL_TEXTURE_2D, m_fontTexture);
    m_funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    m_funcs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    m_funcs->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // Store our identifier
    io.Fonts->TexID = (void *)(intptr_t)m_fontTexture;

    // Restore state
    m_funcs->glBindTexture(GL_TEXTURE_2D, last_texture);

    return true;
}

bool ImGuiRenderer::createDeviceObjects()
{
    auto *glContext = m_renderer->submissionContext()->openGLContext();
    if (glContext->format().majorVersion() < 3) {
        qWarning() << "Qt3D Profiling overlay requires GL or GL ES >= 3";
        return false;
    }

    // Backup GL state
    GLint last_texture, last_array_buffer, last_vertex_array;
    m_funcs->glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    m_funcs->glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    m_funcs->glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

    const GLchar *vertex_shader =
        "#version 330\n"
        "uniform mat4 ProjMtx;\n"
        "in vec2 Position;\n"
        "in vec2 UV;\n"
        "in vec4 Color;\n"
        "out vec2 Frag_UV;\n"
        "out vec4 Frag_Color;\n"
        "void main()\n"
        "{\n"
        "  Frag_UV = UV;\n"
        "  Frag_Color = Color;\n"
        "  gl_Position = ProjMtx * vec4(Position.xy, 0, 1);\n"
        "}\n";

    const GLchar* fragment_shader =
        "#version 330\n"
        "uniform sampler2D Texture;\n"
        "in vec2 Frag_UV;\n"
        "in vec4 Frag_Color;\n"
        "out vec4 Out_Color;\n"
        "void main()\n"
        "{\n"
        "  Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
        "}\n";

    const GLchar *vertex_shader_es3 =
        "#version 300 es\n"
        "uniform mat4 ProjMtx;\n"
        "in vec2 Position;\n"
        "in vec2 UV;\n"
        "in vec4 Color;\n"
        "out vec2 Frag_UV;\n"
        "out vec4 Frag_Color;\n"
        "void main()\n"
        "{\n"
        "  Frag_UV = UV;\n"
        "  Frag_Color = Color;\n"
        "  gl_Position = ProjMtx * vec4(Position.xy, 0, 1);\n"
        "}\n";

    const GLchar* fragment_shader_es3 =
        "#version 300 es\n"
        "precision highp float;\n"
        "uniform sampler2D Texture;\n"
        "in vec2 Frag_UV;\n"
        "in vec4 Frag_Color;\n"
        "out vec4 Out_Color;\n"
        "void main()\n"
        "{\n"
        "  Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
        "}\n";

//    m_shaderHandle = m_funcs->glCreateProgram();
//    m_vertHandle = m_funcs->glCreateShader(GL_VERTEX_SHADER);
//    m_fragHandle = m_funcs->glCreateShader(GL_FRAGMENT_SHADER);
//    auto *glContext = m_renderer->submissionContext()->openGLContext();
//    m_funcs->glShaderSource(m_vertHandle, 1, &vertex_shader, nullptr);
//    m_funcs->glShaderSource(m_fragHandle, 1, &fragment_shader, nullptr);
//    m_funcs->glCompileShader(m_vertHandle);
//    m_funcs->glCompileShader(m_fragHandle);
//    m_funcs->glAttachShader(m_shaderHandle, m_vertHandle);
//    m_funcs->glAttachShader(m_shaderHandle, m_fragHandle);
//    m_funcs->glLinkProgram(m_shaderHandle);

    QString logs;
    m_shader = new QOpenGLShaderProgram(this);
    if (glContext->isOpenGLES()) {
        if (!m_shader->addShaderFromSourceCode(QOpenGLShader::Vertex, vertex_shader_es3))
            logs += m_shader->log();
        if (!m_shader->addShaderFromSourceCode(QOpenGLShader::Fragment, fragment_shader_es3))
            logs += m_shader->log();
    } else {
        if (!m_shader->addShaderFromSourceCode(QOpenGLShader::Vertex, vertex_shader))
            logs += m_shader->log();
        if (!m_shader->addShaderFromSourceCode(QOpenGLShader::Fragment, fragment_shader))
            logs += m_shader->log();
    }
    m_shader->link();
    logs += m_shader->log();
    if (!logs.isEmpty())
        qWarning() << logs;
    m_shaderHandle = m_shader->programId();

    m_attribLocationTex = m_funcs->glGetUniformLocation(m_shaderHandle, "Texture");
    m_attribLocationProjMtx = m_funcs->glGetUniformLocation(m_shaderHandle, "ProjMtx");
    m_attribLocationPosition = m_funcs->glGetAttribLocation(m_shaderHandle, "Position");
    m_attribLocationUV = m_funcs->glGetAttribLocation(m_shaderHandle, "UV");
    m_attribLocationColor = m_funcs->glGetAttribLocation(m_shaderHandle, "Color");

    m_funcs->glGenBuffers(1, &m_vboHandle);
    m_funcs->glGenBuffers(1, &m_elementsHandle);

    m_funcs->glGenVertexArrays(1, &m_vaoHandle);
    m_funcs->glBindVertexArray(m_vaoHandle);
    m_funcs->glBindBuffer(GL_ARRAY_BUFFER, m_vboHandle);
    m_funcs->glEnableVertexAttribArray(m_attribLocationPosition);
    m_funcs->glEnableVertexAttribArray(m_attribLocationUV);
    m_funcs->glEnableVertexAttribArray(m_attribLocationColor);

#define OFFSETOF(TYPE, ELEMENT) (reinterpret_cast<size_t>(&((static_cast<TYPE *>(nullptr))->ELEMENT)))
    m_funcs->glVertexAttribPointer(m_attribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), reinterpret_cast<GLvoid*>(OFFSETOF(ImDrawVert, pos)));
    m_funcs->glVertexAttribPointer(m_attribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), reinterpret_cast<GLvoid*>(OFFSETOF(ImDrawVert, uv)));
    m_funcs->glVertexAttribPointer(m_attribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), reinterpret_cast<GLvoid*>(OFFSETOF(ImDrawVert, col)));
#undef OFFSETOF

    createFontsTexture();

    // Restore modified GL state
    m_funcs->glBindTexture(GL_TEXTURE_2D, last_texture);
    m_funcs->glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
    m_funcs->glBindVertexArray(last_vertex_array);

    return true;
}

bool ImGuiRenderer::newFrame(const RenderView *renderView)
{
    if (!m_funcs)
        m_funcs = m_renderer->submissionContext()->openGLContext()->extraFunctions();
    if (!m_fontTexture)
        createDeviceObjects();
    if (!m_shader)
        return false;

    ImGuiIO& io = ImGui::GetIO();

    // Setup display size (every frame to accommodate for window resizing)
    io.DisplaySize = ImVec2(renderView->surfaceSize().width() / renderView->devicePixelRatio(), renderView->surfaceSize().height() / renderView->devicePixelRatio());
    io.DisplayFramebufferScale = ImVec2(renderView->devicePixelRatio(), renderView->devicePixelRatio());

    // Setup time step
    double current_time = QDateTime::currentMSecsSinceEpoch() / 1000.;
    io.DeltaTime = m_time > 0.0 ? static_cast<float>(current_time - m_time) : 1.0f / 60.0f;
    if (io.DeltaTime == 0.f)
        io.DeltaTime = 1.0f / 60.0f;
    m_time = current_time;

    // Setup inputs
    for (int i = 0; i < 3; i++)
        io.MouseDown[i] = m_mousePressed[i];

    io.MouseWheelH = m_mouseWheelH;
    io.MouseWheel = m_mouseWheel;
    m_mouseWheelH = 0;
    m_mouseWheel = 0;

    // Start the frame
    ImGui::NewFrame();
    return true;
}

void ImGuiRenderer::onMouseChange(QMouseEvent *event)
{
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2(event->pos().x(), event->pos().y());
    m_mousePressed[0] = event->buttons() & Qt::LeftButton;
    m_mousePressed[1] = event->buttons() & Qt::RightButton;
    m_mousePressed[2] = event->buttons() & Qt::MiddleButton;
}

void ImGuiRenderer::onWheel(QWheelEvent *event)
{
    // 5 lines per unit
    m_mouseWheelH += event->pixelDelta().x() / (ImGui::GetTextLineHeight());
    m_mouseWheel += event->pixelDelta().y() / (5.f * ImGui::GetTextLineHeight());
}

void ImGuiRenderer::onKeyPressRelease(QKeyEvent *event)
{
    ImGuiIO& io = ImGui::GetIO();
    if (keyMap.contains(event->key()))
        io.KeysDown[keyMap[event->key()]] = event->type() == QEvent::KeyPress;

    if (event->type() == QEvent::KeyPress) {
        QString text = event->text();
        if (text.size() == 1)
            io.AddInputCharacter(static_cast<ImWchar>(text.at(0).unicode()));
    }

#ifdef Q_OS_DARWIN
    io.KeyCtrl = event->modifiers() & Qt::MetaModifier;
    io.KeyShift = event->modifiers() & Qt::ShiftModifier;
    io.KeyAlt = event->modifiers() & Qt::AltModifier;
    io.KeySuper = event->modifiers() & Qt::ControlModifier; // Command key
#else
    io.KeyCtrl = event->modifiers() & Qt::ControlModifier;
    io.KeyShift = event->modifiers() & Qt::ShiftModifier;
    io.KeyAlt = event->modifiers() & Qt::AltModifier;
    io.KeySuper = event->modifiers() & Qt::MetaModifier;
#endif
}

void ImGuiRenderer::processEvent(QEvent *event)
{
    switch (event->type()) {
    case QEvent::MouseMove:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
        this->onMouseChange(static_cast<QMouseEvent *>(event));
        break;
    case QEvent::Wheel:
        this->onWheel(static_cast<QWheelEvent *>(event));
        break;
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
        this->onKeyPressRelease(static_cast<QKeyEvent *>(event));
        break;
    default:
        break;
    }
}

QT_END_NAMESPACE
