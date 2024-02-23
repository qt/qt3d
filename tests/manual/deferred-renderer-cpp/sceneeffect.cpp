// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include "sceneeffect.h"

#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QShaderProgram>
#include <QUrl>

QT_BEGIN_NAMESPACE

SceneEffect::SceneEffect(Qt3DCore::QNode *parent)
    : Qt3DRender::QEffect(parent)
    , m_gl3Technique(new Qt3DRender::QTechnique())
    , m_gl2Technique(new Qt3DRender::QTechnique())
    , m_gl2Pass(new Qt3DRender::QRenderPass())
    , m_gl3Pass(new Qt3DRender::QRenderPass())
    , m_passCriterion(new Qt3DRender::QFilterKey(this))
{

    m_gl3Technique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);
    m_gl3Technique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
    m_gl3Technique->graphicsApiFilter()->setMajorVersion(3);
    m_gl3Technique->graphicsApiFilter()->setMinorVersion(1);

    m_gl2Technique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
    m_gl2Technique->graphicsApiFilter()->setMajorVersion(2);
    m_gl2Technique->graphicsApiFilter()->setMinorVersion(0);
    m_gl2Technique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::NoProfile);


    m_passCriterion->setName(QStringLiteral("pass"));
    m_passCriterion->setValue(QStringLiteral("geometry"));

    Qt3DRender::QShaderProgram *gl3Shader = new Qt3DRender::QShaderProgram();
    gl3Shader->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/geometry_gl3.vert"))));
    gl3Shader->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/geometry_gl3.frag"))));

    m_gl3Pass->addFilterKey(m_passCriterion);
    m_gl3Pass->setShaderProgram(gl3Shader);
    m_gl3Technique->addRenderPass(m_gl3Pass);

    Qt3DRender::QShaderProgram *gl2Shader = new Qt3DRender::QShaderProgram();
    gl2Shader->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/geometry_gl2.vert"))));
    gl2Shader->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/geometry_gl2.frag"))));

    m_gl2Pass->addFilterKey(m_passCriterion);
    m_gl2Pass->setShaderProgram(gl2Shader);
    m_gl2Technique->addRenderPass(m_gl2Pass);

    addTechnique(m_gl3Technique);
    addTechnique(m_gl2Technique);
}

QList<Qt3DRender::QFilterKey *> SceneEffect::passCriteria() const
{
    return { m_passCriterion };
}

QT_END_NAMESPACE
