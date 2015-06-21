/****************************************************************************
**
** Copyright (C) 2015 Lorenz Esch (TU Ilmenau).
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

#include "qpervertexcolormaterial.h"
#include "qpervertexcolormaterial_p.h"
#include <Qt3DRenderer/qmaterial.h>
#include <Qt3DRenderer/qeffect.h>
#include <Qt3DRenderer/qtechnique.h>
#include <Qt3DRenderer/qshaderprogram.h>
#include <Qt3DRenderer/qparameter.h>
#include <Qt3DRenderer/qrenderpass.h>
#include <Qt3DRenderer/qopenglfilter.h>
#include <QUrl>
#include <QVector3D>
#include <QVector4D>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/*!
    \class Qt3D::QPerVertexColorMaterialPrivate
    \internal
*/
QPerVertexColorMaterialPrivate::QPerVertexColorMaterialPrivate()
    : QMaterialPrivate()
    , m_vertexEffect(new QEffect())
    , m_lightPositionParameter(new QParameter(QStringLiteral("lightPosition"), QVector4D(1.0f, 1.0f, 0.0f, 1.0f)))
    , m_lightIntensityParameter(new QParameter(QStringLiteral("lightIntensity"), QVector3D(1.0f, 1.0f, 1.0f)))
    , m_vertexGL3Technique(new QTechnique())
    , m_vertexGL2Technique(new QTechnique())
    , m_vertexES2Technique(new QTechnique())
    , m_vertexGL3RenderPass(new QRenderPass())
    , m_vertexGL2RenderPass(new QRenderPass())
    , m_vertexES2RenderPass(new QRenderPass())
    , m_vertexGL3Shader(new QShaderProgram())
    , m_vertexGL2ES2Shader(new QShaderProgram())
{
}

/*!
    \class Qt3D::QPerVertexColorMaterial
    \brief The QPerVertexColorMaterial class provides a default implementation for rendering the color properties set for each vertex.
    \inmodule Qt3DRenderer
    \since 5.5

    This lighting effect is based on the combination of 2 lighting components ambient and diffuse. Ambient is set by the vertex color.
    Diffuse takes in account the normal distribution of each vertex.

    \list
    \li Ambient is the color that is emitted by an object without any other light source.
    \li Diffuse is the color that is emitted for rough surface reflections with the lights
    \endlist

    This material uses an effect with a single render pass approach and forms  fragment lighting.
    Techniques are provided for OpenGL 2, OpenGL 3 or above as well as OpenGL ES 2.
*/

/*!
    \fn Qt3D::QPerVertexColorMaterial::QPerVertexColorMaterial(QNode *parent)

    Constructs a new QPerVertexColorMaterial instance with parent object \a parent.
*/
QPerVertexColorMaterial::QPerVertexColorMaterial(QNode *parent)
    : QMaterial(*new QPerVertexColorMaterialPrivate, parent)
{
    Q_D(QPerVertexColorMaterial);
    d->init();
}

/*!
   \fn Qt3D::QPerVertexColorMaterial::~QPerVertexColorMaterial()

   Destroys the QPerVertexColorMaterial
*/
QPerVertexColorMaterial::~QPerVertexColorMaterial()
{
}

// TODO: Define how lights are proties are set in the shaders. Ideally using a QShaderData
void QPerVertexColorMaterialPrivate::init()
{
    m_vertexGL3Shader->setVertexShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/gl3/pervertexcolor.vert"))));
    m_vertexGL3Shader->setFragmentShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/gl3/pervertexcolor.frag"))));
    m_vertexGL2ES2Shader->setVertexShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/es2/pervertexcolor.vert"))));
    m_vertexGL2ES2Shader->setFragmentShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/es2/pervertexcolor.frag"))));

    m_vertexGL3Technique->openGLFilter()->setApi(QOpenGLFilter::Desktop);
    m_vertexGL3Technique->openGLFilter()->setMajorVersion(3);
    m_vertexGL3Technique->openGLFilter()->setMinorVersion(1);
    m_vertexGL3Technique->openGLFilter()->setProfile(QOpenGLFilter::Core);

    m_vertexGL2Technique->openGLFilter()->setApi(QOpenGLFilter::Desktop);
    m_vertexGL2Technique->openGLFilter()->setMajorVersion(2);
    m_vertexGL2Technique->openGLFilter()->setMinorVersion(0);
    m_vertexGL2Technique->openGLFilter()->setProfile(QOpenGLFilter::None);

    m_vertexES2Technique->openGLFilter()->setApi(QOpenGLFilter::ES);
    m_vertexES2Technique->openGLFilter()->setMajorVersion(2);
    m_vertexES2Technique->openGLFilter()->setMinorVersion(0);
    m_vertexES2Technique->openGLFilter()->setProfile(QOpenGLFilter::None);

    m_vertexGL3RenderPass->setShaderProgram(m_vertexGL3Shader);
    m_vertexGL2RenderPass->setShaderProgram(m_vertexGL2ES2Shader);
    m_vertexES2RenderPass->setShaderProgram(m_vertexGL2ES2Shader);

    m_vertexGL3Technique->addPass(m_vertexGL3RenderPass);
    m_vertexGL2Technique->addPass(m_vertexGL2RenderPass);
    m_vertexES2Technique->addPass(m_vertexES2RenderPass);

    m_vertexEffect->addTechnique(m_vertexGL3Technique);
    m_vertexEffect->addTechnique(m_vertexGL2Technique);
    m_vertexEffect->addTechnique(m_vertexES2Technique);

    m_vertexEffect->addParameter(m_lightPositionParameter);
    m_vertexEffect->addParameter(m_lightIntensityParameter);

    q_func()->setEffect(m_vertexEffect);
}

} // Qt3D

QT_END_NAMESPACE
