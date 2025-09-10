// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_RENDERSTATES_H
#define QT3DRENDER_RENDER_RENDERSTATES_H

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

#include <Qt3DRender/private/genericstate_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

class Q_3DRENDERSHARED_PRIVATE_EXPORT BlendEquationArguments : public GenericState<BlendEquationArguments, BlendEquationArgumentsMask, GLenum, GLenum, GLenum, GLenum, bool, int>
{
public:
    void updateProperties(const QRenderState *node) override;
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT BlendEquation : public GenericState<BlendEquation, BlendStateMask, GLenum>
{
public:
    void updateProperties(const QRenderState *node) override;
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT AlphaFunc : public GenericState<AlphaFunc, AlphaTestMask, GLenum, GLclampf>
{
public:
    void updateProperties(const QRenderState *node) override;
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT MSAAEnabled : public GenericState<MSAAEnabled, MSAAEnabledStateMask, GLboolean>
{
public:
    void updateProperties(const QRenderState *node) override;
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT DepthRange : public GenericState<DepthRange, DepthRangeMask, double, double>
{
public:
    void updateProperties(const QRenderState *node) override;
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT DepthTest : public GenericState<DepthTest, DepthTestStateMask, GLenum>
{
public:
    void updateProperties(const QRenderState *node) override;
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT RasterMode : public GenericState<RasterMode, RasterModeMask, GLenum, GLenum>
{
public:
    void updateProperties(const QRenderState *node) override;
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT NoDepthMask : public GenericState<NoDepthMask, DepthWriteStateMask, GLboolean>
{
public:
    void updateProperties(const QRenderState *node) override;
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT CullFace : public GenericState<CullFace, CullFaceStateMask, GLenum>
{
public:
    void updateProperties(const QRenderState *node) override;
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT FrontFace : public GenericState<FrontFace, FrontFaceStateMask, GLenum>
{
public:
    void updateProperties(const QRenderState *node) override;
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT Dithering : public GenericState<Dithering, DitheringStateMask>
{
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT ScissorTest : public GenericState<ScissorTest, ScissorStateMask, int, int, int, int>
{
public:
    void updateProperties(const QRenderState *node) override;
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT StencilTest : public GenericState<StencilTest, StencilTestStateMask, GLenum, int, uint, GLenum, int, uint>
{
public:
    void updateProperties(const QRenderState *node) override;
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT AlphaCoverage : public GenericState<AlphaCoverage, AlphaCoverageStateMask>
{
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT PointSize : public GenericState<PointSize, PointSizeMask, bool, GLfloat>
{
public:
    void updateProperties(const QRenderState *node) override;
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT PolygonOffset : public GenericState<PolygonOffset, PolygonOffsetStateMask, GLfloat, GLfloat>
{
public:

    void updateProperties(const QRenderState *node) override;
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT ColorMask : public GenericState<ColorMask, ColorStateMask, GLboolean, GLboolean, GLboolean, GLboolean>
{
public:
    void updateProperties(const QRenderState *node) override;
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT ClipPlane : public GenericState<ClipPlane, ClipPlaneMask, int, QVector3D, float>
{
public:
    void updateProperties(const QRenderState *node) override;
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT SeamlessCubemap : public GenericState<SeamlessCubemap, SeamlessCubemapMask>
{
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT StencilOp : public GenericState<StencilOp, StencilOpMask, GLenum, GLenum, GLenum, GLenum, GLenum, GLenum>
{
public:
    void updateProperties(const QRenderState *node) override;
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT StencilMask : public GenericState<StencilMask, StencilWriteStateMask, uint, uint>
{
public:
    void updateProperties(const QRenderState *node) override;
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT LineWidth : public GenericState<LineWidth, LineWidthMask, GLfloat, bool>
{
public:
    void updateProperties(const QRenderState *node) override;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RENDERSTATES_H
