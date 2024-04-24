// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qtext2dmaterial_p.h"
#include "qtext2dmaterial_p_p.h"
#include <Qt3DRender/qfilterkey.h>
#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qeffect.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/qshaderprogram.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qgraphicsapifilter.h>
#include <Qt3DRender/qblendequation.h>
#include <Qt3DRender/qblendequationarguments.h>
#include <Qt3DRender/qdepthtest.h>
#include <Qt3DRender/qabstracttexture.h>
#include <QUrl>
#include <QVector3D>
#include <QVector4D>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

QText2DMaterialPrivate::QText2DMaterialPrivate()
    : QMaterialPrivate()
    , m_effect(new Qt3DRender::QEffect())
    , m_distanceFieldTexture(nullptr)
    , m_textureParameter(new Qt3DRender::QParameter(QStringLiteral("distanceFieldTexture"), QVariant(0)))
    , m_textureSizeParameter(new Qt3DRender::QParameter(QStringLiteral("textureWidth"), QVariant(256.f)))
    , m_colorParameter(new Qt3DRender::QParameter(QStringLiteral("color"), QVariant(QColor(255, 255, 255, 255))))
    , m_gl3Technique(new Qt3DRender::QTechnique())
    , m_gl2Technique(new Qt3DRender::QTechnique())
    , m_es2Technique(new Qt3DRender::QTechnique())
    , m_rhiTechnique(new Qt3DRender::QTechnique())
    , m_gl3RenderPass(new Qt3DRender::QRenderPass())
    , m_gl2RenderPass(new Qt3DRender::QRenderPass())
    , m_es2RenderPass(new Qt3DRender::QRenderPass())
    , m_rhiRenderPass(new Qt3DRender::QRenderPass())
    , m_rhiShaderProgram(new Qt3DRender::QShaderProgram())
    , m_gl3ShaderProgram(new Qt3DRender::QShaderProgram())
    , m_gl2es2ShaderProgram(new Qt3DRender::QShaderProgram())
    , m_blend(new Qt3DRender::QBlendEquation())
    , m_blendArgs(new Qt3DRender::QBlendEquationArguments())
    , m_depthTest(new Qt3DRender::QDepthTest())
{
}

void QText2DMaterialPrivate::init()
{
    m_rhiShaderProgram->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/rhi/distancefieldtext.vert"))));
    m_rhiShaderProgram->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/rhi/distancefieldtext.frag"))));

    m_gl3ShaderProgram->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/gl3/distancefieldtext.vert"))));
    m_gl3ShaderProgram->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/gl3/distancefieldtext.frag"))));

    m_gl2es2ShaderProgram->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/es2/distancefieldtext.vert"))));
    m_gl2es2ShaderProgram->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/es2/distancefieldtext.frag"))));

    m_blend->setBlendFunction(Qt3DRender::QBlendEquation::Add);
    m_blendArgs->setSourceRgba(Qt3DRender::QBlendEquationArguments::SourceAlpha);
    m_blendArgs->setDestinationRgba(Qt3DRender::QBlendEquationArguments::OneMinusSourceAlpha);
    m_depthTest->setDepthFunction(Qt3DRender::QDepthTest::LessOrEqual);

    m_gl3RenderPass->setShaderProgram(m_gl3ShaderProgram);
    m_gl3RenderPass->addRenderState(m_blend);
    m_gl3RenderPass->addRenderState(m_blendArgs);
    m_gl3RenderPass->addRenderState(m_depthTest);

    m_gl2RenderPass->setShaderProgram(m_gl2es2ShaderProgram);
    m_gl2RenderPass->addRenderState(m_blend);
    m_gl2RenderPass->addRenderState(m_blendArgs);
    m_gl2RenderPass->addRenderState(m_depthTest);

    m_es2RenderPass->setShaderProgram(m_gl2es2ShaderProgram);
    m_es2RenderPass->addRenderState(m_blend);
    m_es2RenderPass->addRenderState(m_blendArgs);
    m_es2RenderPass->addRenderState(m_depthTest);

    m_rhiRenderPass->setShaderProgram(m_rhiShaderProgram);
    m_rhiRenderPass->addRenderState(m_blend);
    m_rhiRenderPass->addRenderState(m_blendArgs);
    m_rhiRenderPass->addRenderState(m_depthTest);

    m_gl3Technique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
    m_gl3Technique->graphicsApiFilter()->setMajorVersion(3);
    m_gl3Technique->graphicsApiFilter()->setMinorVersion(1);
    m_gl3Technique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);
    m_gl3Technique->addRenderPass(m_gl3RenderPass);

    m_gl2Technique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
    m_gl2Technique->graphicsApiFilter()->setMajorVersion(2);
    m_gl2Technique->graphicsApiFilter()->setMinorVersion(0);
    m_gl2Technique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::NoProfile);
    m_gl2Technique->addRenderPass(m_gl2RenderPass);

    m_es2Technique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGLES);
    m_es2Technique->graphicsApiFilter()->setMajorVersion(2);
    m_es2Technique->graphicsApiFilter()->setMinorVersion(0);
    m_es2Technique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::NoProfile);
    m_es2Technique->addRenderPass(m_es2RenderPass);

    m_rhiTechnique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::RHI);
    m_rhiTechnique->graphicsApiFilter()->setMajorVersion(1);
    m_rhiTechnique->graphicsApiFilter()->setMinorVersion(0);
    m_rhiTechnique->addRenderPass(m_rhiRenderPass);

    Qt3DRender::QFilterKey *filterKey = new Qt3DRender::QFilterKey(q_func());
    filterKey->setName(QStringLiteral("renderingStyle"));
    filterKey->setValue(QStringLiteral("forward"));
    m_gl3Technique->addFilterKey(filterKey);
    m_gl2Technique->addFilterKey(filterKey);
    m_es2Technique->addFilterKey(filterKey);
    m_rhiTechnique->addFilterKey(filterKey);

    m_effect->addTechnique(m_gl3Technique);
    m_effect->addTechnique(m_gl2Technique);
    m_effect->addTechnique(m_es2Technique);
    m_effect->addTechnique(m_rhiTechnique);
    m_effect->addParameter(m_textureParameter);
    m_effect->addParameter(m_textureSizeParameter);
    m_effect->addParameter(m_colorParameter);

    q_func()->setEffect(m_effect);
}

QText2DMaterial::QText2DMaterial(Qt3DCore::QNode *parent)
    : QMaterial(*new QText2DMaterialPrivate(), parent)
{
    Q_D(QText2DMaterial);
    d->init();
}

QText2DMaterial::~QText2DMaterial()
{
}

void QText2DMaterial::setColor(const QColor &color)
{
    Q_D(QText2DMaterial);
    d->m_colorParameter->setValue(QVariant::fromValue(color));
}

void QText2DMaterial::setDistanceFieldTexture(Qt3DRender::QAbstractTexture *tex)
{
    Q_D(QText2DMaterial);
    d->m_distanceFieldTexture = tex;

    if (tex) {
        d->m_textureParameter->setValue(QVariant::fromValue(tex));
        d->m_textureSizeParameter->setValue(QVariant::fromValue(static_cast<float>(tex->width())));
    } else {
        d->m_textureParameter->setValue(0);
        d->m_textureSizeParameter->setValue(QVariant::fromValue(1.f));
    }
}

} // namespace Qt3DExtras

QT_END_NAMESPACE

#include "moc_qtext2dmaterial_p.cpp"
