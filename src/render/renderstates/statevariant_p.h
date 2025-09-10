// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_STATEVARIANT_P_H
#define QT3DRENDER_RENDER_STATEVARIANT_P_H

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

#include <Qt3DRender/private/renderstates_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

struct Q_3DRENDERSHARED_PRIVATE_EXPORT StateVariant
{
    StateMask type;

    union u_Data {
        BlendEquationArguments blendEquationArguments;
        BlendEquation blendEquation;
        AlphaFunc alphaFunc;
        MSAAEnabled msaaEnabled;
        DepthRange depthRange;
        DepthTest depthTest;
        NoDepthMask noDepthMask;
        CullFace cullFace;
        FrontFace frontFace;
        Dithering dithering;
        ScissorTest scissorTest;
        StencilTest stencilTest;
        AlphaCoverage alphaCoverage;
        PointSize pointSize;
        PolygonOffset polygonOffset;
        ColorMask colorMask;
        ClipPlane clipPlane;
        SeamlessCubemap seamlessCubemap;
        StencilOp stencilOp;
        StencilMask stencilMask;
        LineWidth lineWidth;

        u_Data()
        {
            // Assumes the above types don't need to have their ctor called
            memset(static_cast<void *>(this), 0, sizeof(u_Data));
        }

        u_Data(const u_Data &other)
        {
            memcpy(static_cast<void *>(this), static_cast<const void *>(&other), sizeof(u_Data));
        }

        u_Data& operator=(const u_Data &other)
        {
            memcpy(static_cast<void *>(this), static_cast<const void *>(&other), sizeof(u_Data));
            return *this;
        }

        ~u_Data()
        {
            // Assumes the above types don't need to have their dtor called
        }
    } data;

    template<class State, typename ... Args>
    static StateVariant createState(Args... values)
    {
        State state;
        state.set(values...);
        return StateVariant::fromValue(state);
    }

    template<typename GenericState>
    static StateVariant fromValue(const GenericState &state)
    {
        StateVariant v;
        v.type = GenericState::type();
        // all union members start at the same memory address
        // so we can just write into whichever we want
        memcpy(static_cast<void *>(&v.data), static_cast<const void *>(&state), sizeof(state));
        return v;
    }

    RenderStateImpl *state();
    const RenderStateImpl *constState() const;

    bool operator ==(const StateVariant &other) const;
    bool operator !=(const StateVariant &other) const;
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_STATEVARIANT_P_H
