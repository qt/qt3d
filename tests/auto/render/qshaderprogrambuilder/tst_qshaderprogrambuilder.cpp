// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only


#include <QtTest/QTest>
#include <Qt3DRender/qshaderprogram.h>
#include <Qt3DRender/qshaderprogrambuilder.h>
#include <Qt3DRender/private/qshaderprogrambuilder_p.h>
#include <QObject>
#include <QSignalSpy>
#include "testarbiter.h"

class tst_QShaderProgramBuilder : public Qt3DRender::QShaderProgramBuilder
{
    Q_OBJECT
public:
    tst_QShaderProgramBuilder()
        : Qt3DRender::QShaderProgramBuilder()
    {
        qRegisterMetaType<Qt3DRender::QShaderProgram*>("Qt3DRender::QShaderProgram*");
    }

private Q_SLOTS:
    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QShaderProgramBuilder builder;

        // THEN
        QVERIFY(!builder.shaderProgram());
        QVERIFY(builder.enabledLayers().isEmpty());
        QCOMPARE(builder.vertexShaderGraph(), QUrl());
        QCOMPARE(builder.tessellationControlShaderGraph(), QUrl());
        QCOMPARE(builder.tessellationEvaluationShaderGraph(), QUrl());
        QCOMPARE(builder.geometryShaderGraph(), QUrl());
        QCOMPARE(builder.fragmentShaderGraph(), QUrl());
        QCOMPARE(builder.computeShaderGraph(), QUrl());
        QCOMPARE(builder.vertexShaderCode(), QByteArray());
        QCOMPARE(builder.fragmentShaderCode(), QByteArray());
        QCOMPARE(builder.computeShaderCode(), QByteArray());
        QCOMPARE(builder.geometryShaderCode(), QByteArray());
        QCOMPARE(builder.tessellationEvaluationShaderCode(), QByteArray());
        QCOMPARE(builder.tessellationControlShaderCode(), QByteArray());
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::QShaderProgramBuilder builder;

        {
            // WHEN
            QSignalSpy spy(&builder, SIGNAL(shaderProgramChanged(Qt3DRender::QShaderProgram*)));
            auto newValue = new Qt3DRender::QShaderProgram(&builder);
            builder.setShaderProgram(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(builder.shaderProgram(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            builder.setShaderProgram(newValue);

            // THEN
            QCOMPARE(builder.shaderProgram(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&builder, SIGNAL(enabledLayersChanged(QStringList)));
            const auto newValue = QStringList() << "foo" << "bar";
            builder.setEnabledLayers(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(builder.enabledLayers(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            builder.setEnabledLayers(newValue);

            // THEN
            QCOMPARE(builder.enabledLayers(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&builder, SIGNAL(vertexShaderGraphChanged(QUrl)));
            const auto newValue = QUrl::fromEncoded("qrc:/vertex.json");
            builder.setVertexShaderGraph(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(builder.vertexShaderGraph(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            builder.setVertexShaderGraph(newValue);

            // THEN
            QCOMPARE(builder.vertexShaderGraph(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&builder, SIGNAL(tessellationControlShaderGraphChanged(QUrl)));
            const auto newValue = QUrl::fromEncoded("qrc:/tesscontrol.json");
            builder.setTessellationControlShaderGraph(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(builder.tessellationControlShaderGraph(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            builder.setTessellationControlShaderGraph(newValue);

            // THEN
            QCOMPARE(builder.tessellationControlShaderGraph(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&builder, SIGNAL(tessellationEvaluationShaderGraphChanged(QUrl)));
            const auto newValue = QUrl::fromEncoded("qrc:/tesseval.json");
            builder.setTessellationEvaluationShaderGraph(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(builder.tessellationEvaluationShaderGraph(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            builder.setTessellationEvaluationShaderGraph(newValue);

            // THEN
            QCOMPARE(builder.tessellationEvaluationShaderGraph(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&builder, SIGNAL(geometryShaderGraphChanged(QUrl)));
            const auto newValue = QUrl::fromEncoded("qrc:/geometry.json");
            builder.setGeometryShaderGraph(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(builder.geometryShaderGraph(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            builder.setGeometryShaderGraph(newValue);

            // THEN
            QCOMPARE(builder.geometryShaderGraph(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&builder, SIGNAL(fragmentShaderGraphChanged(QUrl)));
            const auto newValue = QUrl::fromEncoded("qrc:/fragment.json");
            builder.setFragmentShaderGraph(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(builder.fragmentShaderGraph(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            builder.setFragmentShaderGraph(newValue);

            // THEN
            QCOMPARE(builder.fragmentShaderGraph(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&builder, SIGNAL(computeShaderGraphChanged(QUrl)));
            const auto newValue = QUrl::fromEncoded("qrc:/compute.json");
            builder.setComputeShaderGraph(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(builder.computeShaderGraph(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            builder.setComputeShaderGraph(newValue);

            // THEN
            QCOMPARE(builder.computeShaderGraph(), newValue);
            QCOMPARE(spy.size(), 0);
        }
    }

    void checkShaderProgramBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QShaderProgramBuilder> builder(new Qt3DRender::QShaderProgramBuilder);
        {
            // WHEN
            Qt3DRender::QShaderProgram program;
            builder->setShaderProgram(&program);

            // THEN
            QCOMPARE(program.parent(), builder.data());
            QCOMPARE(builder->shaderProgram(), &program);
        }
        // THEN (Should not crash and effect be unset)
        QVERIFY(!builder->shaderProgram());

        {
            // WHEN
            Qt3DRender::QShaderProgramBuilder someOtherBuilder;
            QScopedPointer<Qt3DRender::QShaderProgram> program(new Qt3DRender::QShaderProgram(&someOtherBuilder));
            builder->setShaderProgram(program.data());

            // THEN
            QCOMPARE(program->parent(), &someOtherBuilder);
            QCOMPARE(builder->shaderProgram(), program.data());

            // WHEN
            builder.reset();
            program.reset();

            // THEN Should not crash when the effect is destroyed (tests for failed removal of destruction helper)
        }
    }

    void checkShaderProgramUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QShaderProgramBuilder builder;
        arbiter.setArbiterOnNode(&builder);
        auto program = new Qt3DRender::QShaderProgram(&builder);

        {
            // WHEN
            builder.setShaderProgram(program);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &builder);

            arbiter.clear();
        }

        {
            // WHEN
            builder.setShaderProgram(program);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }
    }

    void checkEnabledLayersUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QShaderProgramBuilder builder;
        arbiter.setArbiterOnNode(&builder);
        const auto layers = QStringList() << "foo" << "bar";

        {
            // WHEN
            builder.setEnabledLayers(layers);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &builder);

            arbiter.clear();
        }

        {
            // WHEN
            builder.setEnabledLayers(layers);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }
    }

    void checkVertexShaderGraphUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QShaderProgramBuilder builder;
        arbiter.setArbiterOnNode(&builder);

        {
            // WHEN
            builder.setVertexShaderGraph(QUrl::fromEncoded("qrc:/vertex.json"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &builder);

            arbiter.clear();
        }

        {
            // WHEN
            builder.setVertexShaderGraph(QUrl::fromEncoded("qrc:/vertex.json"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkTessellationControlShaderGraphUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QShaderProgramBuilder builder;
        arbiter.setArbiterOnNode(&builder);

        {
            // WHEN
            builder.setTessellationControlShaderGraph(QUrl::fromEncoded("qrc:/tesscontrol.json"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &builder);

            arbiter.clear();
        }

        {
            // WHEN
            builder.setTessellationControlShaderGraph(QUrl::fromEncoded("qrc:/tesscontrol.json"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkTessellationEvaluationShaderGraphUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QShaderProgramBuilder builder;
        arbiter.setArbiterOnNode(&builder);

        {
            // WHEN
            builder.setTessellationEvaluationShaderGraph(QUrl::fromEncoded("qrc:/tesseval.json"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &builder);

            arbiter.clear();
        }

        {
            // WHEN
            builder.setTessellationEvaluationShaderGraph(QUrl::fromEncoded("qrc:/tesseval.json"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkGeometryShaderGraphUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QShaderProgramBuilder builder;
        arbiter.setArbiterOnNode(&builder);

        {
            // WHEN
            builder.setGeometryShaderGraph(QUrl::fromEncoded("qrc:/geometry.json"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &builder);

            arbiter.clear();
        }

        {
            // WHEN
            builder.setGeometryShaderGraph(QUrl::fromEncoded("qrc:/geometry.json"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkFragmentShaderGraphUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QShaderProgramBuilder builder;
        arbiter.setArbiterOnNode(&builder);

        {
            // WHEN
            builder.setFragmentShaderGraph(QUrl::fromEncoded("qrc:/fragment.json"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &builder);

            arbiter.clear();
        }

        {
            // WHEN
            builder.setFragmentShaderGraph(QUrl::fromEncoded("qrc:/fragment.json"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkComputeShaderGraphUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QShaderProgramBuilder builder;
        arbiter.setArbiterOnNode(&builder);

        {
            // WHEN
            builder.setComputeShaderGraph(QUrl::fromEncoded("qrc:/compute.json"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &builder);

            arbiter.clear();
        }

        {
            // WHEN
            builder.setComputeShaderGraph(QUrl::fromEncoded("qrc:/compute.json"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }
    }

    void checkGeneratedCodePropertyUpdates()
    {
        Qt3DRender::QShaderProgramBuilderPrivate *dBuilder = static_cast<decltype(dBuilder)>(Qt3DCore::QNodePrivate::get(this));
        {
            // WHEN
            QSignalSpy spy(this, SIGNAL(vertexShaderCodeChanged(QByteArray)));
            dBuilder->setShaderCode(QByteArrayLiteral("vertex"), Qt3DRender::QShaderProgram::Vertex);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(spy.size(), 1);
            QCOMPARE(vertexShaderCode(), QByteArrayLiteral("vertex"));
        }
        {
            // WHEN
            QSignalSpy spy(this, SIGNAL(fragmentShaderCodeChanged(QByteArray)));
            dBuilder->setShaderCode(QByteArrayLiteral("fragment"), Qt3DRender::QShaderProgram::Fragment);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(spy.size(), 1);
            QCOMPARE(fragmentShaderCode(), QByteArrayLiteral("fragment"));
        }
        {
            // WHEN
            QSignalSpy spy(this, SIGNAL(geometryShaderCodeChanged(QByteArray)));
            dBuilder->setShaderCode(QByteArrayLiteral("geometry"), Qt3DRender::QShaderProgram::Geometry);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(spy.size(), 1);
            QCOMPARE(geometryShaderCode(), QByteArrayLiteral("geometry"));
        }
        {
            // WHEN
            QSignalSpy spy(this, SIGNAL(computeShaderCodeChanged(QByteArray)));
            dBuilder->setShaderCode(QByteArrayLiteral("compute"), Qt3DRender::QShaderProgram::Compute);


            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(spy.size(), 1);
            QCOMPARE(computeShaderCode(), QByteArrayLiteral("compute"));
        }
        {
            // WHEN
            QSignalSpy spy(this, SIGNAL(tessellationControlShaderCodeChanged(QByteArray)));
            dBuilder->setShaderCode(QByteArrayLiteral("control"), Qt3DRender::QShaderProgram::TessellationControl);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(spy.size(), 1);
            QCOMPARE(tessellationControlShaderCode(), QByteArrayLiteral("control"));
        }
        {
            // WHEN
            QSignalSpy spy(this, SIGNAL(tessellationEvaluationShaderCodeChanged(QByteArray)));
            dBuilder->setShaderCode(QByteArrayLiteral("eval"), Qt3DRender::QShaderProgram::TessellationEvaluation);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(spy.size(), 1);
            QCOMPARE(tessellationEvaluationShaderCode(), QByteArrayLiteral("eval"));
        }
    }

};

QTEST_MAIN(tst_QShaderProgramBuilder)

#include "tst_qshaderprogrambuilder.moc"
