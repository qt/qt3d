/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DRENDER_RENDER_OPENGL_GLLIGHTS_P_H
#define QT3DRENDER_RENDER_OPENGL_GLLIGHTS_P_H

#include <QString>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace OpenGL {

#define MAX_LIGHTS 8

static_assert (MAX_LIGHTS < 10, "GL_Lights can't use the QChar trick anymore");

struct GLLights
{
    static int LIGHT_COUNT_NAME_ID;

    static QString LIGHT_STRUCT_NAMES[MAX_LIGHTS];
    static int LIGHT_POSITION_NAMES[MAX_LIGHTS];
    static int LIGHT_TYPE_NAMES[MAX_LIGHTS];
    static int LIGHT_COLOR_NAMES[MAX_LIGHTS];
    static int LIGHT_INTENSITY_NAMES[MAX_LIGHTS];
    static int LIGHT_DIRECTION_NAMES[MAX_LIGHTS];
    static int LIGHT_LINEAR_ATTENUATION_NAMES[MAX_LIGHTS];
    static int LIGHT_QUADRATIC_ATTENUATION_NAMES[MAX_LIGHTS];
    static int LIGHT_CONSTANT_ATTENUATION_NAMES[MAX_LIGHTS];
    static int LIGHT_CUT_OFF_ANGLE_NAMES[MAX_LIGHTS];

    static QString LIGHT_STRUCT_UNROLL_NAMES[MAX_LIGHTS];
    static int LIGHT_POSITION_UNROLL_NAMES[MAX_LIGHTS];
    static int LIGHT_TYPE_UNROLL_NAMES[MAX_LIGHTS];
    static int LIGHT_COLOR_UNROLL_NAMES[MAX_LIGHTS];
    static int LIGHT_INTENSITY_UNROLL_NAMES[MAX_LIGHTS];
    static int LIGHT_DIRECTION_UNROLL_NAMES[MAX_LIGHTS];
    static int LIGHT_LINEAR_ATTENUATION_UNROLL_NAMES[MAX_LIGHTS];
    static int LIGHT_QUADRATIC_ATTENUATION_UNROLL_NAMES[MAX_LIGHTS];
    static int LIGHT_CONSTANT_ATTENUATION_UNROLL_NAMES[MAX_LIGHTS];
    static int LIGHT_CUT_OFF_ANGLE_UNROLL_NAMES[MAX_LIGHTS];
};

} // namespace OpenGL
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_OPENGL_GLLIGHTS_P_H
