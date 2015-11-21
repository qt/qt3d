/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2015 The Qt Company Ltd and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DRENDER_RENDER_RENDERCOMMAND_H
#define QT3DRENDER_RENDER_RENDERCOMMAND_H

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
#include <Qt3DRender/private/quniformvalue_p.h>
#include <Qt3DRender/private/handle_types_p.h>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMatrix4x4>

QT_BEGIN_NAMESPACE

class QOpenGLVertexArrayObject;

namespace Qt3DRender {

namespace Render {

class RenderStateSet;

class RenderCommand
{
public:
    RenderCommand();

    HVao m_vao; // VAO used during the submission step to store all states and VBOs
    HShader m_shader; // Shader for given pass and mesh
    QUniformPack m_uniforms; // Might need to be reworked so as to be able to destroy the
                            // Texture while submission is happening.
    GLint m_instancesCount; // Number of instances of the mesh, if 0 regular draw otherwise glDrawArraysInstanced or glDrawElementsInstanced
    RenderStateSet *m_stateSet;

    HGeometry m_geometry;
    HGeometryRenderer m_geometryRenderer;

    // A QAttribute pack might be interesting
    // This is a temporary fix in the meantime, to remove the hacked methods in Technique
    QHash<QString, QString> m_parameterAttributeToShaderNames;

    float m_depth;
    int m_changeCost;
    uint m_shaderDna;

    union sortingType {
        char sorts[4];
        int  global;
    } m_sortingType;

    bool m_sortBackToFront;
};

bool compareCommands(RenderCommand *r1, RenderCommand *r2);

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_RENDERCOMMAND_H
