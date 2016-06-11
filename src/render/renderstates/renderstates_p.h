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

template <class State, class S>
State* getOrCreateRenderStateImpl(const S& m1)
{
    State state;
    state.set(m1);
    return getOrCreateRenderStateEqualTo<State>(state);
}

template <class State, class S, class T>
State* getOrCreateRenderStateImpl(const S& m1, const T& m2)
{
    State state;
    state.set(m1, m2);
    return getOrCreateRenderStateEqualTo<State>(state);
}

template <class State, class S, class T, class U>
State* getOrCreateRenderStateImpl(const S& m1, const T& m2, const U& m3)
{
    State state;
    state.set(m1, m2, m3);
    return getOrCreateRenderStateEqualTo<State>(state);
}

template <class State, class S, class T, class U, class V>
State* getOrCreateRenderStateImpl(const S& m1, const T& m2, const U& m3, const V& m4)
{
    State state;
    state.set(m1, m2, m3, m4);
    return getOrCreateRenderStateEqualTo<State>(state);
}

template <class State, class S, class T, class U, class V, class W, class Z>
State* getOrCreateRenderStateImpl(const S& m1, const T& m2, const U& m3, const V& m4, const W& m5, const Z& m6)
{
    State state;
    state.set(m1, m2, m3, m4, m5, m6);
    return getOrCreateRenderStateEqualTo<State>(state);
}

class Q_AUTOTEST_EXPORT BlendEquationArguments : public GenericState6<BlendEquationArguments, BlendStateMask, GLenum, GLenum, GLenum, GLenum, bool, int>
{
public:
    virtual void apply(GraphicsContext *gc) const Q_DECL_OVERRIDE;
    void updateProperty(const char *name, const QVariant &value) Q_DECL_OVERRIDE;
};

class Q_AUTOTEST_EXPORT BlendEquation : public GenericState1<BlendEquation, BlendStateMask, GLenum>
{
public:
    virtual void apply(GraphicsContext *gc) const Q_DECL_OVERRIDE;
    void updateProperty(const char *name, const QVariant &value) Q_DECL_OVERRIDE;
};


class Q_AUTOTEST_EXPORT AlphaFunc : public GenericState2<AlphaFunc, AlphaTestMask, GLenum, GLclampf>
{
public:
    virtual void apply(GraphicsContext *gc) const Q_DECL_OVERRIDE;
};

class Q_AUTOTEST_EXPORT MSAAEnabled : public GenericState1<MSAAEnabled, MSAAEnabledStateMask, GLboolean>
{
public:
    virtual void apply(GraphicsContext *gc) const Q_DECL_OVERRIDE;
    void updateProperty(const char *name, const QVariant &value) Q_DECL_OVERRIDE;
};

class Q_AUTOTEST_EXPORT DepthTest : public GenericState1<DepthTest, DepthTestStateMask, GLenum>
{
public:
    virtual void apply(GraphicsContext *gc) const Q_DECL_OVERRIDE;
    void updateProperty(const char *name, const QVariant &value) Q_DECL_OVERRIDE;
};

class Q_AUTOTEST_EXPORT NoDepthMask : public GenericState1<NoDepthMask, DepthWriteStateMask, GLboolean>
{
public:
    virtual void apply(GraphicsContext *gc) const Q_DECL_OVERRIDE;
    void updateProperty(const char *name, const QVariant &value) Q_DECL_OVERRIDE;
};

class Q_AUTOTEST_EXPORT CullFace : public GenericState1<CullFace, CullFaceStateMask, GLenum>
{
public:
    virtual void apply(GraphicsContext *gc) const Q_DECL_OVERRIDE;
    void updateProperty(const char *name, const QVariant &value) Q_DECL_OVERRIDE;
};

class Q_AUTOTEST_EXPORT FrontFace : public GenericState1<FrontFace, FrontFaceStateMask, GLenum>
{
public:
    virtual void apply(GraphicsContext *gc) const Q_DECL_OVERRIDE;
    void updateProperty(const char *name, const QVariant &value) Q_DECL_OVERRIDE;
};

class Q_AUTOTEST_EXPORT Dithering : public MaskedRenderState<Dithering, DitheringStateMask>
{
public:
    virtual void apply(GraphicsContext *gc) const Q_DECL_OVERRIDE;
};

class Q_AUTOTEST_EXPORT ScissorTest : public GenericState4<ScissorTest, ScissorStateMask, int, int, int, int>
{
public:
    virtual void apply(GraphicsContext *gc) const Q_DECL_OVERRIDE;
    bool isPooledImpl() const Q_DECL_NOTHROW Q_DECL_OVERRIDE { return false; }
    void updateProperty(const char *name, const QVariant &value) Q_DECL_OVERRIDE;
};

class Q_AUTOTEST_EXPORT StencilTest : public GenericState6<StencilTest, StencilTestStateMask, GLenum, int, uint, GLenum, int, uint>
{
public:
    virtual void apply(GraphicsContext *gc) const Q_DECL_OVERRIDE;
};

class Q_AUTOTEST_EXPORT AlphaCoverage : public GenericState1<AlphaCoverage, AlphaCoverageStateMask, GLboolean>
{
public:
    void apply(GraphicsContext *gc) const Q_DECL_OVERRIDE;
    void updateProperty(const char *name, const QVariant &value) Q_DECL_OVERRIDE;
};

class Q_AUTOTEST_EXPORT PointSize : public GenericState2<PointSize, PointSizeMask, bool, GLfloat>
{
public:
    void apply(GraphicsContext *gc) const Q_DECL_OVERRIDE;
    bool isPooledImpl() const Q_DECL_NOTHROW Q_DECL_OVERRIDE { return false; }
    void updateProperty(const char *name, const QVariant &value) Q_DECL_OVERRIDE;
};

class Q_AUTOTEST_EXPORT PolygonOffset : public GenericState2<PolygonOffset, PolygonOffsetStateMask, GLfloat, GLfloat>
{
public:
    void apply(GraphicsContext *gc) const Q_DECL_OVERRIDE;
    bool isPooledImpl() const Q_DECL_NOTHROW Q_DECL_OVERRIDE { return false; }
    void updateProperty(const char *name, const QVariant &value) Q_DECL_OVERRIDE;
};

class Q_AUTOTEST_EXPORT ColorMask : public GenericState4<ColorMask, ColorStateMask, GLboolean, GLboolean, GLboolean, GLboolean>
{
public:
    void apply(GraphicsContext *gc) const Q_DECL_FINAL;
    void updateProperty(const char *name, const QVariant &value) Q_DECL_OVERRIDE;
};

class Q_AUTOTEST_EXPORT ClipPlane : public GenericState3<ClipPlane, ClipPlaneMask, int, QVector3D, float>
{
public:
    void apply(GraphicsContext *gc) const Q_DECL_FINAL;
    void updateProperty(const char *name, const QVariant &value) Q_DECL_OVERRIDE;
};

class Q_AUTOTEST_EXPORT SeamlessCubemap : public GenericState1<SeamlessCubemap, SeamlessCubemapMask, GLboolean>
{
public:
    virtual void apply(GraphicsContext *gc) const Q_DECL_OVERRIDE;
    void updateProperty(const char *name, const QVariant &value) Q_DECL_OVERRIDE;
};

class Q_AUTOTEST_EXPORT StencilOp : public GenericState6<StencilOp, StencilOpMask, GLenum, GLenum, GLenum, GLenum, GLenum, GLenum>
{
public:
    void apply(GraphicsContext *gc) const Q_DECL_FINAL;
};

class Q_AUTOTEST_EXPORT StencilMask : public GenericState2<StencilMask, StencilWriteStateMask, uint, uint>
{
public:
    void apply(GraphicsContext *gc) const Q_DECL_FINAL;
    void updateProperty(const char *name, const QVariant &value) Q_DECL_OVERRIDE;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RENDERSTATES_H
