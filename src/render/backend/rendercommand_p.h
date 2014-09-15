/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3D_RENDER_RENDERCOMMAND_H
#define QT3D_RENDER_RENDERCOMMAND_H

#include <qglobal.h>
#include <Qt3DRenderer/private/quniformvalue_p.h>
#include <Qt3DRenderer/private/qmeshdata_p.h>
#include <Qt3DCore/qhandle.h>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMatrix4x4>

QT_BEGIN_NAMESPACE

class QOpenGLVertexArrayObject;

namespace Qt3D {

namespace Render {

class RenderTarget;
class RenderMaterial;
class RenderMesh;
class RenderShader;
class RenderTechnique;
class RenderStateSet;

typedef QHandle<QOpenGLVertexArrayObject*, 16> HVao;
typedef QHandle<RenderMaterial, 16> HMaterial;
typedef QHandle<RenderMesh, 16> HMesh;
typedef QHandle<RenderShader, 16> HShader;
typedef QHandle<QMeshData, 16> HMeshData;
typedef QHandle<RenderTechnique, 16> HTechnique;

class RenderCommand
{
public:
    RenderCommand();

    HVao m_vao; // VAO used during the submission step to store all states and VBOs
    HMeshData m_meshData;
    HShader m_shader; // Shader for given pass and mesh
    QUniformPack m_uniforms; // Might need to be reworked so as to be able to destroy the
                            // RenderTexture while submission is happening.
    GLint m_instancesCount; // Number of instances of the mesh, if 0 regular draw otherwise glDrawArraysInstanced or glDrawElementsInstanced
    RenderStateSet *m_stateSet;

    // A QAttribute pack might be interesting
    // This is a temporary fix in the meantime, to remove the hacked methods in RenderTechnique
    QHash<QString, QString> m_parameterAttributeToShaderNames;

    float m_depth;
    int m_changeCost;

    union sortingType {
        char sorts[4];
        int  global;
    } m_sortingType;

};

bool operator < (const RenderCommand &r1, const RenderCommand &r2);

} // Render

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_RENDERCOMMAND_H
