/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include <QtTest/QTest>
#include <Qt3DRenderer/private/rendershader_p.h>
#include <Qt3DRenderer/qshaderprogram.h>

class tst_RenderShader : public QObject
{
    Q_OBJECT
private slots:

    void hasCoherentInitialState();
    void matchesFrontendPeer();
    void cleanupLeavesACoherentState();
};


Qt3D::QShaderProgram *createFrontendShader()
{
    Qt3D::QShaderProgram *shader = new Qt3D::QShaderProgram();

    shader->setVertexShaderCode(QByteArrayLiteral(
                                    "#version 150"\
                                    "in vec3 vertexPosition;"\
                                    "in vec3 vertexColor; "\
                                    "out vec3 color;"\
                                    "void main()"\
                                    "{"\
                                    "    color = vertexColor;"\
                                    "    gl_Position = vec4( vertexPosition, 1.0 );"\
                                    "}"));

    shader->setFragmentShaderCode(QByteArrayLiteral(
                                      "#version 150"\
                                      "in vec3 color;"\
                                      "out vec4 fragColor;"\
                                      "void main()"\
                                      "{"\
                                      "   fragColor = vec4( color, 1.0 );"\
                                      "}"));

    return shader;
}

void tst_RenderShader::hasCoherentInitialState()
{
    Qt3D::Render::RenderShader *shader = new Qt3D::Render::RenderShader();

    QCOMPARE(shader->isLoaded(), false);
    QCOMPARE(shader->dna(), 0U);
    QVERIFY(shader->uniformsNames().isEmpty());
    QVERIFY(shader->attributesNames().isEmpty());
    QVERIFY(shader->uniformBlockNames().isEmpty());
    QVERIFY(shader->uniforms().isEmpty());
    QVERIFY(shader->attributes().isEmpty());
    QVERIFY(shader->uniformBlocks().isEmpty());
}

void tst_RenderShader::matchesFrontendPeer()
{
    Qt3D::QShaderProgram *frontend = createFrontendShader();
    Qt3D::Render::RenderShader *backend = new Qt3D::Render::RenderShader();

    backend->updateFromPeer(frontend);
    QCOMPARE(backend->isLoaded(), false);
    QVERIFY(backend->dna() != 0U);

    for (int i = Qt3D::QShaderProgram::Vertex; i <= Qt3D::QShaderProgram::Compute; ++i)
        QCOMPARE(backend->shaderCode()[i],
                 frontend->shaderCode( static_cast<const Qt3D::QShaderProgram::ShaderType>(i)));
}

void tst_RenderShader::cleanupLeavesACoherentState()
{
    Qt3D::QShaderProgram *frontend = createFrontendShader();
    Qt3D::Render::RenderShader *shader = new Qt3D::Render::RenderShader();

    shader->updateFromPeer(frontend);

    shader->cleanup();

    QCOMPARE(shader->isLoaded(), false);
    QCOMPARE(shader->dna(), 0U);
    QVERIFY(shader->uniformsNames().isEmpty());
    QVERIFY(shader->attributesNames().isEmpty());
    QVERIFY(shader->uniformBlockNames().isEmpty());
    QVERIFY(shader->uniforms().isEmpty());
    QVERIFY(shader->attributes().isEmpty());
    QVERIFY(shader->uniformBlocks().isEmpty());
}

QTEST_APPLESS_MAIN(tst_RenderShader)

#include "tst_rendershader.moc"
