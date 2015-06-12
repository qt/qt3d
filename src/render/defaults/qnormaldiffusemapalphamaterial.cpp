/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qnormaldiffusemapalphamaterial.h"
#include "qnormaldiffusemapalphamaterial_p.h"

#include <Qt3DRenderer/qeffect.h>
#include <Qt3DRenderer/qtexture.h>
#include <Qt3DRenderer/qtechnique.h>
#include <Qt3DRenderer/qparameter.h>
#include <Qt3DRenderer/qshaderprogram.h>
#include <Qt3DRenderer/qrenderpass.h>
#include <Qt3DRenderer/qopenglfilter.h>
#include <Qt3DRenderer/qalphacoverage.h>
#include <Qt3DRenderer/qdepthtest.h>

#include <QUrl>
#include <QVector3D>
#include <QVector4D>

QT_BEGIN_NAMESPACE

namespace Qt3D {


/*!
    \class Qt3D::QNormalDiffuseMapAlphaMaterialPrivate
    \internal
*/
QNormalDiffuseMapAlphaMaterialPrivate::QNormalDiffuseMapAlphaMaterialPrivate()
    : QNormalDiffuseMapMaterialPrivate()
    , m_alphaCoverage(new QAlphaCoverage())
    , m_depthTest(new QDepthTest())
{
}

void QNormalDiffuseMapAlphaMaterialPrivate::init()
{
    m_normalDiffuseGL3Shader->setVertexShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/gl3/normaldiffusemap.vert"))));
    m_normalDiffuseGL3Shader->setFragmentShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/gl3/normaldiffusemapalpha.frag"))));
    m_normalDiffuseGL2ES2Shader->setVertexShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/es2/normaldiffusemap.vert"))));
    m_normalDiffuseGL2ES2Shader->setFragmentShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/es2/normaldiffusemapalpha.frag"))));

    m_normalDiffuseGL3Technique->openGLFilter()->setApi(QOpenGLFilter::Desktop);
    m_normalDiffuseGL3Technique->openGLFilter()->setMajorVersion(3);
    m_normalDiffuseGL3Technique->openGLFilter()->setMinorVersion(1);
    m_normalDiffuseGL3Technique->openGLFilter()->setProfile(QOpenGLFilter::Core);

    m_normalDiffuseGL2Technique->openGLFilter()->setApi(QOpenGLFilter::Desktop);
    m_normalDiffuseGL2Technique->openGLFilter()->setMajorVersion(2);
    m_normalDiffuseGL2Technique->openGLFilter()->setMinorVersion(0);
    m_normalDiffuseGL2Technique->openGLFilter()->setProfile(QOpenGLFilter::None);

    m_normalDiffuseES2Technique->openGLFilter()->setApi(QOpenGLFilter::ES);
    m_normalDiffuseES2Technique->openGLFilter()->setMajorVersion(2);
    m_normalDiffuseES2Technique->openGLFilter()->setMinorVersion(0);
    m_normalDiffuseES2Technique->openGLFilter()->setProfile(QOpenGLFilter::None);

    m_depthTest->setFunc(QDepthTest::Less);

    m_normalDiffuseGL3RenderPass->setShaderProgram(m_normalDiffuseGL3Shader);
    m_normalDiffuseGL3RenderPass->addRenderState(m_alphaCoverage);
    m_normalDiffuseGL3RenderPass->addRenderState(m_depthTest);

    m_normalDiffuseGL2RenderPass->setShaderProgram(m_normalDiffuseGL2ES2Shader);
    m_normalDiffuseGL2RenderPass->addRenderState(m_alphaCoverage);
    m_normalDiffuseGL2RenderPass->addRenderState(m_depthTest);

    m_normalDiffuseES2RenderPass->setShaderProgram(m_normalDiffuseGL2ES2Shader);
    m_normalDiffuseES2RenderPass->addRenderState(m_alphaCoverage);
    m_normalDiffuseES2RenderPass->addRenderState(m_depthTest);

    m_normalDiffuseGL3Technique->addPass(m_normalDiffuseGL3RenderPass);
    m_normalDiffuseGL2Technique->addPass(m_normalDiffuseGL2RenderPass);
    m_normalDiffuseES2Technique->addPass(m_normalDiffuseES2RenderPass);

    m_normalDiffuseEffect->addTechnique(m_normalDiffuseGL3Technique);
    m_normalDiffuseEffect->addTechnique(m_normalDiffuseGL2Technique);
    m_normalDiffuseEffect->addTechnique(m_normalDiffuseES2Technique);

    m_normalDiffuseEffect->addParameter(m_ambientParameter);
    m_normalDiffuseEffect->addParameter(m_diffuseParameter);
    m_normalDiffuseEffect->addParameter(m_normalParameter);
    m_normalDiffuseEffect->addParameter(m_specularParameter);
    m_normalDiffuseEffect->addParameter(m_shininessParameter);
    m_normalDiffuseEffect->addParameter(m_lightPositionParameter);
    m_normalDiffuseEffect->addParameter(m_lightIntensityParameter);
    m_normalDiffuseEffect->addParameter(m_textureScaleParameter);

    q_func()->setEffect(m_normalDiffuseEffect);
}


/*!
    \class Qt3D::QNormalDiffuseMapAlphaMaterial
    \brief The QNormalDiffuseMapAlphaMaterial provides a default implementation of the phong lighting and bump effect where the diffuse light component
    is read from a texture map and the normals of the mesh being rendered from a normal texture map. In addition, it defines an alpha to coverage and
    a depth test to be performed in the rendering pass.
    \inmodule Qt3DRenderer
    \since 5.5

    The specular lighting effect is based on the combination of 3 lighting components ambient, diffuse and specular.
    The relative strengths of these components is controlled by means of their reflectivity coefficients which are modelled as RGB triplets:

    \list
    \li Ambient is the color that is emitted by an object without any other light source.
    \li Diffuse is the color that is emitted for rought surface reflections with the lights.
    \li Specular is the color emitted for shiny surface reflections with the lights.
    \li The shininess of a surface is controlled by a float property.
    \endlist

    This material uses an effect with a single render pass approach and performs per fragment lighting.
    Techniques are provided for OpenGL 2, OpenGL 3 or above as well as OpenGL ES 2.
*/

/*!
    Constructs a new Qt3D::QNormalDiffuseMapAlphaMaterial instance with parent object \a parent.
*/
QNormalDiffuseMapAlphaMaterial::QNormalDiffuseMapAlphaMaterial(QNode *parent)
    : QNormalDiffuseMapMaterial(*new QNormalDiffuseMapAlphaMaterialPrivate, parent)
{
}

/*!
    Destroys the QNormalDiffuseMapAlphaMaterial instance.
*/
QNormalDiffuseMapAlphaMaterial::~QNormalDiffuseMapAlphaMaterial()
{
}

} // Qt3D

QT_END_NAMESPACE
