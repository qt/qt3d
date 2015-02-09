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

#include "finaleffect.h"

#include <Qt3DRenderer/QOpenGLFilter>
#include <Qt3DRenderer/QShaderProgram>
#include <QUrl>

FinalEffect::FinalEffect(Qt3D::QNode *parent)
    : Qt3D::QEffect(parent)
    , m_gl3Technique(new Qt3D::QTechnique())
    , m_gl2Technique(new Qt3D::QTechnique())
    , m_gl2Pass(new Qt3D::QRenderPass())
    , m_gl3Pass(new Qt3D::QRenderPass())
    , m_passCriterion(new Qt3D::QAnnotation(this))
{
    m_gl3Technique->openGLFilter()->setApi(Qt3D::QOpenGLFilter::Desktop);
    m_gl3Technique->openGLFilter()->setMajorVersion(3);
    m_gl3Technique->openGLFilter()->setMinorVersion(1);
    m_gl3Technique->openGLFilter()->setProfile(Qt3D::QOpenGLFilter::Core);

    m_gl2Technique->openGLFilter()->setApi(Qt3D::QOpenGLFilter::Desktop);
    m_gl2Technique->openGLFilter()->setMajorVersion(2);
    m_gl2Technique->openGLFilter()->setMinorVersion(0);
    m_gl2Technique->openGLFilter()->setProfile(Qt3D::QOpenGLFilter::None);

    m_passCriterion->setName(QStringLiteral("pass"));
    m_passCriterion->setValue(QStringLiteral("final"));

    Qt3D::QShaderProgram *gl3Shader = new Qt3D::QShaderProgram();
    gl3Shader->setVertexShaderCode(gl3Shader->loadSource(QUrl(QStringLiteral("qrc:/final_gl3.vert"))));
    gl3Shader->setFragmentShaderCode(gl3Shader->loadSource(QUrl(QStringLiteral("qrc:/final_gl3.frag"))));

    m_gl3Pass->addAnnotation(m_passCriterion);
    m_gl3Pass->setShaderProgram(gl3Shader);
    m_gl3Technique->addPass(m_gl3Pass);

    Qt3D::QShaderProgram *gl2Shader = new Qt3D::QShaderProgram();
    gl2Shader->setVertexShaderCode(gl2Shader->loadSource(QUrl(QStringLiteral("qrc:/final_gl2.vert"))));
    gl2Shader->setFragmentShaderCode(gl2Shader->loadSource(QUrl(QStringLiteral("qrc:/final_gl2.frag"))));

    m_gl2Pass->addAnnotation(m_passCriterion);
    m_gl2Pass->setShaderProgram(gl2Shader);
    m_gl2Technique->addPass(m_gl2Pass);

    addTechnique(m_gl3Technique);
    addTechnique(m_gl2Technique);
}

QList<Qt3D::QAnnotation *> FinalEffect::passCriteria() const
{
    return QList<Qt3D::QAnnotation *>() << m_passCriterion;
}
