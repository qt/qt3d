// Copyright (C) 2020  Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

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

        std::vector<int> randKeys(64);
        QRandomGenerator gen;

        for (size_t i = 0; i < 64; ++i)
            randKeys[i] = gen.generate();

        QBENCHMARK {
            for (const int key : std::as_const(randKeys))
                pack.insert(key, UniformValue(key));
        }
    }

    void prepareUniforms()
    {
        // GIVEN
        GLShader shader;
        std::vector<ShaderUniform> uniformDescriptions;
        for (int i = 0; i < 30; i++) {
            ShaderUniform u;
            u.m_name = QString::number(i);
            uniformDescriptions.push_back(u);
        }
        shader.initializeUniforms(uniformDescriptions);

        // THEN
        QCOMPARE(shader.uniforms().size(), 30);

        // WHEN
        std::vector<int> testNames;
        for (int i = 0; i < 10; ++i)
            testNames.push_back(shader.uniforms()[i * 3].m_nameId);

        // WHEN
        ShaderParameterPack pack;
        for (const int nameId : testNames)
            pack.setUniform(nameId, UniformValue(nameId));

        QBENCHMARK {
            shader.prepareUniforms(pack);
        }
    }
};

QTEST_MAIN(tst_BenchShaderParameterPack)

#include "tst_bench_shaderparameterpack.moc"
