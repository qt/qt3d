/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#include "finaleffect.h"

#include <Qt3DRenderer/QOpenGLFilter>
#include <Qt3DRenderer/QShaderProgram>

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
    gl3Shader->setVertexShaderSourceFile(QStringLiteral(":/final_gl3.vert"));
    gl3Shader->setFragmentShaderSourceFile(QStringLiteral(":/final_gl3.frag"));

    m_gl3Pass->addCriterion(m_passCriterion);
    m_gl3Pass->setShaderProgram(gl3Shader);
    m_gl3Technique->addPass(m_gl3Pass);

    Qt3D::QShaderProgram *gl2Shader = new Qt3D::QShaderProgram();
    gl2Shader->setVertexShaderSourceFile(QStringLiteral(":/final_gl2.vert"));
    gl2Shader->setFragmentShaderSourceFile(QStringLiteral(":/final_gl2.frag"));

    m_gl2Pass->addCriterion(m_passCriterion);
    m_gl2Pass->setShaderProgram(gl2Shader);
    m_gl2Technique->addPass(m_gl2Pass);

    addTechnique(m_gl3Technique);
    addTechnique(m_gl2Technique);
}

QList<Qt3D::QAnnotation *> FinalEffect::passCriteria() const
{
    return QList<Qt3D::QAnnotation *>() << m_passCriterion;
}
