/****************************************************************************
**
** Copyright (C) 2020  Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QTest>
#include <Qt3DRender/private/uniform_p.h>
#include <shaderparameterpack_p.h>
#include <glshader_p.h>
#include <shadervariables_p.h>
#include <QRandomGenerator>

using namespace Qt3DRender::Render;
using namespace Qt3DRender::Render::OpenGL;


class tst_BenchShaderParameterPack : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void checkPackUniformInsert()
    {
        // GIVEN
        PackUniformHash pack;

        QVector<int> randKeys(64);
        QRandomGenerator gen;

        for (int i = 0; i < 64; ++i)
            randKeys[i] = gen.generate();

        QBENCHMARK {
            for (const int key : qAsConst(randKeys))
                pack.insert(key, UniformValue(key));
        }
    }

    void prepareUniforms()
    {
        // GIVEN
        GLShader shader;
        QVector<ShaderUniform> uniformDescriptions;
        for (int i = 0; i < 30; i++) {
            ShaderUniform u;
            u.m_name = QString::number(i);
            uniformDescriptions << u;
        }
        shader.initializeUniforms(uniformDescriptions);

        // THEN
        QCOMPARE(shader.uniforms().size(), 30);

        // WHEN
        QVector<int> testNames;
        for (int i = 0; i < 10; ++i)
            testNames << shader.uniforms()[i * 3].m_nameId;

        // WHEN
        ShaderParameterPack pack;
        for (const int nameId : qAsConst(testNames))
            pack.setUniform(nameId, UniformValue(nameId));

        QBENCHMARK {
            shader.prepareUniforms(pack);
        }
    }
};

QTEST_MAIN(tst_BenchShaderParameterPack)

#include "tst_bench_shaderparameterpack.moc"
