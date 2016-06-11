/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
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

#include "renderstates_p.h"

#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DRender/qrenderstate.h>
#include <Qt3DRender/qcullface.h>

#include <Qt3DRender/private/graphicscontext_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

void RenderStateImpl::updateProperty(const char *name, const QVariant &value)
{
    Q_UNUSED(name);
    Q_UNUSED(value);
}

bool RenderStateImpl::isPooledImpl() const Q_DECL_NOTHROW
{
    return true;
}

void BlendEquationArguments::apply(GraphicsContext* gc) const
{
    // Un-indexed BlendEquationArguments -> Use normal GL1.0 functions
    if (m_6 < 0) {
        if (m_5) {
            gc->openGLContext()->functions()->glEnable(GL_BLEND);
            gc->openGLContext()->functions()->glBlendFuncSeparate(m_1, m_2, m_3, m_4);
        } else {
            gc->openGLContext()->functions()->glDisable(GL_BLEND);
        }
    }
    // BlendEquationArguments for a particular Draw Buffer. Different behaviours for
    //  (1) 3.0-3.3: only enablei/disablei supported.
    //  (2) 4.0+: all operations supported.
    // We just ignore blend func parameter for (1), so no warnings get
    // printed.
    else {
        if (m_5) {
            gc->enablei(GL_BLEND, m_6);
            if (gc->supportsDrawBuffersBlend()) {
                gc->blendFuncSeparatei(m_6, m_1, m_2, m_3, m_4);
            }
        } else {
            gc->disablei(GL_BLEND, m_6);
        }
    }
}

void BlendEquationArguments::updateProperty(const char *name, const QVariant &value)
{
         if (name == QByteArrayLiteral("sourceRgb")) m_1 = value.toInt();
    else if (name == QByteArrayLiteral("destinationRgb")) m_2 = value.toInt();
    else if (name == QByteArrayLiteral("sourceAlpha")) m_3 = value.toInt();
    else if (name == QByteArrayLiteral("destinationAlpha")) m_4 = value.toInt();
    else if (name == QByteArrayLiteral("enabled")) m_5 = value.toBool();
    else if (name == QByteArrayLiteral("bufferIndex")) m_6 = value.toInt();
}

void BlendEquation::apply(GraphicsContext *gc) const
{
    gc->blendEquation(m_1);
}

void BlendEquation::updateProperty(const char *name, const QVariant &value)
{
    if (name == QByteArrayLiteral("mode")) m_1 = value.toInt();
}

void AlphaFunc::apply(GraphicsContext* gc) const
{
    gc->alphaTest(m_1, m_2);
}

void MSAAEnabled::apply(GraphicsContext *gc) const
{
    gc->setMSAAEnabled(m_1);
}

void MSAAEnabled::updateProperty(const char *name, const QVariant &value)
{
    if (name == QByteArrayLiteral("enabled"))
        m_1 = value.toBool();
}

void DepthTest::apply(GraphicsContext *gc) const
{
    gc->depthTest(m_1);
}

void DepthTest::updateProperty(const char *name, const QVariant &value)
{
    if (name == QByteArrayLiteral("func")) m_1 = value.toInt();
}

void CullFace::apply(GraphicsContext *gc) const
{
    if (m_1 == QCullFace::NoCulling) {
        gc->openGLContext()->functions()->glDisable(GL_CULL_FACE);
    } else {
        gc->openGLContext()->functions()->glEnable(GL_CULL_FACE);
        gc->openGLContext()->functions()->glCullFace(m_1);
    }
}

void CullFace::updateProperty(const char *name, const QVariant &value)
{
    if (name == QByteArrayLiteral("mode")) m_1 = value.toInt();
}

void FrontFace::apply(GraphicsContext *gc) const
{
    gc->frontFace(m_1);
}

void FrontFace::updateProperty(const char *name, const QVariant &value)
{
    if (name == QByteArrayLiteral("direction")) m_1 = value.toInt();
}

void NoDepthMask::apply(GraphicsContext *gc) const
{
   gc->depthMask(m_1);
}

void NoDepthMask::updateProperty(const char *name, const QVariant &value)
{
    if (name == QByteArrayLiteral("mask")) m_1 = value.toBool();
}

void Dithering::apply(GraphicsContext *gc) const
{
    gc->openGLContext()->functions()->glEnable(GL_DITHER);
}

void ScissorTest::apply(GraphicsContext *gc) const
{
    gc->openGLContext()->functions()->glEnable(GL_SCISSOR_TEST);
    gc->openGLContext()->functions()->glScissor(m_1, m_2, m_3, m_4);
}

void ScissorTest::updateProperty(const char *name, const QVariant &value)
{
         if (name == QByteArrayLiteral("left")) m_1 = value.toInt();
    else if (name == QByteArrayLiteral("bottom")) m_2 = value.toInt();
    else if (name == QByteArrayLiteral("width")) m_3 = value.toInt();
    else if (name == QByteArrayLiteral("height")) m_4 = value.toInt();
}

void StencilTest::apply(GraphicsContext *gc) const
{
    gc->openGLContext()->functions()->glEnable(GL_STENCIL_TEST);
    gc->openGLContext()->functions()->glStencilFuncSeparate(GL_FRONT, m_1, m_2, m_3);
    gc->openGLContext()->functions()->glStencilFuncSeparate(GL_BACK, m_4, m_5, m_6);
}

void AlphaCoverage::apply(GraphicsContext *gc) const
{
    gc->setAlphaCoverageEnabled(m_1);
}

void AlphaCoverage::updateProperty(const char *name, const QVariant &value)
{
    if (name == QByteArrayLiteral("enabled"))
        m_1 = value.toBool();
}

void PointSize::apply(GraphicsContext *gc) const
{
    gc->pointSize(m_1, m_2);
}

void PointSize::updateProperty(const char *name, const QVariant &value)
{
         if (name == QByteArrayLiteral("specification")) m_1 = value.toBool();
    else if (name == QByteArrayLiteral("value")) m_2 = value.toFloat();
}

void PolygonOffset::apply(GraphicsContext *gc) const
{
    gc->openGLContext()->functions()->glEnable(GL_POLYGON_OFFSET_FILL);
    gc->openGLContext()->functions()->glPolygonOffset(m_1, m_2);
}

void PolygonOffset::updateProperty(const char *name, const QVariant &value)
{
         if (name == QByteArrayLiteral("factor")) m_1 = value.toFloat();
    else if (name == QByteArrayLiteral("units")) m_2 = value.toFloat();
}

void ColorMask::apply(GraphicsContext *gc) const
{
    gc->openGLContext()->functions()->glColorMask(m_1, m_2, m_3, m_4);
}

void ColorMask::updateProperty(const char *name, const QVariant &value)
{
         if (name == QByteArrayLiteral("red")) m_1 = value.toBool();
    else if (name == QByteArrayLiteral("green")) m_2 = value.toBool();
    else if (name == QByteArrayLiteral("blue")) m_3 = value.toBool();
    else if (name == QByteArrayLiteral("alpha")) m_4 = value.toBool();
}

void ClipPlane::apply(GraphicsContext *gc) const
{
    gc->enableClipPlane(m_1);
    gc->setClipPlane(m_1, m_2, m_3);
}

void ClipPlane::updateProperty(const char *name, const QVariant &value)
{
    if (name == QByteArrayLiteral("planeIndex")) m_1 = value.toInt();
    else if (name == QByteArrayLiteral("normal")) m_2 = value.value<QVector3D>();
    else if (name == QByteArrayLiteral("distance")) m_3 = value.toFloat();
}

void SeamlessCubemap::apply(GraphicsContext *gc) const
{
    gc->setSeamlessCubemap(m_1);
}

void SeamlessCubemap::updateProperty(const char *name, const QVariant &value)
{
    if (name == QByteArrayLiteral("enabled")) m_1 = value.toBool();
}

void StencilOp::apply(GraphicsContext *gc) const
{
    gc->openGLContext()->functions()->glStencilOpSeparate(GL_FRONT, m_1, m_2, m_3);
    gc->openGLContext()->functions()->glStencilOpSeparate(GL_BACK, m_4, m_5, m_6);
}

void StencilMask::apply(GraphicsContext *gc) const
{
    gc->openGLContext()->functions()->glStencilMaskSeparate(GL_FRONT, m_1);
    gc->openGLContext()->functions()->glStencilMaskSeparate(GL_BACK, m_2);
}

void StencilMask::updateProperty(const char *name, const QVariant &value)
{
         if (name == QByteArrayLiteral("frontMask")) m_1 = value.toInt();
    else if (name == QByteArrayLiteral("backMask")) m_2 = value.toInt();
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
