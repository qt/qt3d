// Copyright (C) 2016 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include <QtTest/QTest>
#include <Qt3DRender/qshaderprogram.h>
#include <Qt3DRender/private/qshaderprogram_p.h>
#include <QObject>
#include <QSignalSpy>
#include "testarbiter.h"

// We need to call QShaderProgram::sceneChangeEvent
class tst_QShaderProgram : public Qt3DRender::QShaderProgram
{
    Q_OBJECT

public:
    tst_QShaderProgram()
        : Qt3DRender::QShaderProgram()
    {
        qRegisterMetaType<Qt3DRender::QShaderProgram::Format>("Format");
    }

private Q_SLOTS:

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QShaderProgram shaderProgram;

        // THEN
        QCOMPARE(shaderProgram.vertexShaderCode(), QByteArray());
        QCOMPARE(shaderProgram.tessellationControlShaderCode(), QByteArray());
        QCOMPARE(shaderProgram.tessellationEvaluationShaderCode(), QByteArray());
        QCOMPARE(shaderProgram.geometryShaderCode(), QByteArray());
        QCOMPARE(shaderProgram.fragmentShaderCode(), QByteArray());
        QCOMPARE(shaderProgram.computeShaderCode(), QByteArray());
        QCOMPARE(shaderProgram.log(), QString());
        QCOMPARE(shaderProgram.status(), Qt3DRender::QShaderProgram::NotReady);
        QCOMPARE(shaderProgram.format(), Qt3DRender::QShaderProgram::GLSL);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::QShaderProgram shaderProgram;

        {
            // WHEN
            QSignalSpy spy(&shaderProgram, SIGNAL(vertexShaderCodeChanged(QByteArray)));
            const QByteArray newValue = "VERTEX";
            shaderProgram.setVertexShaderCode(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(shaderProgram.vertexShaderCode(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            shaderProgram.setVertexShaderCode(newValue);

            // THEN
            QCOMPARE(shaderProgram.vertexShaderCode(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&shaderProgram, SIGNAL(tessellationControlShaderCodeChanged(QByteArray)));
            const QByteArray newValue = "TESSELATION_CONTROL";
            shaderProgram.setTessellationControlShaderCode(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(shaderProgram.tessellationControlShaderCode(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            shaderProgram.setTessellationControlShaderCode(newValue);

            // THEN
            QCOMPARE(shaderProgram.tessellationControlShaderCode(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&shaderProgram, SIGNAL(tessellationEvaluationShaderCodeChanged(QByteArray)));
            const QByteArray newValue = "TESSELATION_EVALUTATION";
            shaderProgram.setTessellationEvaluationShaderCode(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(shaderProgram.tessellationEvaluationShaderCode(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            shaderProgram.setTessellationEvaluationShaderCode(newValue);

            // THEN
            QCOMPARE(shaderProgram.tessellationEvaluationShaderCode(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&shaderProgram, SIGNAL(geometryShaderCodeChanged(QByteArray)));
            const QByteArray newValue = "GEOMETRY";
            shaderProgram.setGeometryShaderCode(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(shaderProgram.geometryShaderCode(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            shaderProgram.setGeometryShaderCode(newValue);

            // THEN
            QCOMPARE(shaderProgram.geometryShaderCode(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&shaderProgram, SIGNAL(fragmentShaderCodeChanged(QByteArray)));
            const QByteArray newValue = "FRAGMENT";
            shaderProgram.setFragmentShaderCode(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(shaderProgram.fragmentShaderCode(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            shaderProgram.setFragmentShaderCode(newValue);

            // THEN
            QCOMPARE(shaderProgram.fragmentShaderCode(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&shaderProgram, SIGNAL(computeShaderCodeChanged(QByteArray)));
            const QByteArray newValue = "COMPUTE";
            shaderProgram.setComputeShaderCode(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(shaderProgram.computeShaderCode(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            shaderProgram.setComputeShaderCode(newValue);

            // THEN
            QCOMPARE(shaderProgram.computeShaderCode(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&shaderProgram, SIGNAL(formatChanged(Format)));
            const QShaderProgram::Format newValue = QShaderProgram::SPIRV;
            shaderProgram.setFormat(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(shaderProgram.format(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            shaderProgram.setFormat(newValue);

            // THEN
            QCOMPARE(shaderProgram.format(), newValue);
            QCOMPARE(spy.size(), 0);
        }
    }

    void checkVertexShaderCodeUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QShaderProgram shaderProgram;
        arbiter.setArbiterOnNode(&shaderProgram);

        {
            // WHEN
            shaderProgram.setVertexShaderCode(QByteArrayLiteral("in vec3 toto;"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &shaderProgram);

            arbiter.clear();
        }

        {
            // WHEN
            shaderProgram.setVertexShaderCode(QByteArrayLiteral("in vec3 toto;"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkTessellationControlShaderCodeUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QShaderProgram shaderProgram;
        arbiter.setArbiterOnNode(&shaderProgram);

        {
            // WHEN
            shaderProgram.setTessellationControlShaderCode(QByteArrayLiteral("in vec3 toto2;"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &shaderProgram);

            arbiter.clear();
        }

        {
            // WHEN
            shaderProgram.setTessellationControlShaderCode(QByteArrayLiteral("in vec3 toto2;"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkTessellationEvaluationShaderCodeUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QShaderProgram shaderProgram;
        arbiter.setArbiterOnNode(&shaderProgram);

        {
            // WHEN
            shaderProgram.setTessellationEvaluationShaderCode(QByteArrayLiteral("in vec3 toto3;"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &shaderProgram);

            arbiter.clear();
        }

        {
            // WHEN
            shaderProgram.setTessellationEvaluationShaderCode(QByteArrayLiteral("in vec3 toto3;"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkGeometryShaderCodeUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QShaderProgram shaderProgram;
        arbiter.setArbiterOnNode(&shaderProgram);

        {
            // WHEN
            shaderProgram.setGeometryShaderCode(QByteArrayLiteral("in vec3 toto4;"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &shaderProgram);

            arbiter.clear();
        }

        {
            // WHEN
            shaderProgram.setGeometryShaderCode(QByteArrayLiteral("in vec3 toto4;"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkFragmentShaderCodeUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QShaderProgram shaderProgram;
        arbiter.setArbiterOnNode(&shaderProgram);

        {
            // WHEN
            shaderProgram.setFragmentShaderCode(QByteArrayLiteral("out vec4 fragColor;"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &shaderProgram);

            arbiter.clear();
        }

        {
            // WHEN
            shaderProgram.setFragmentShaderCode(QByteArrayLiteral("out vec4 fragColor;"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkComputeShaderCodeUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QShaderProgram shaderProgram;
        arbiter.setArbiterOnNode(&shaderProgram);

        {
            // WHEN
            shaderProgram.setComputeShaderCode(QByteArrayLiteral("uniform vec3 temp;"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &shaderProgram);

            arbiter.clear();
        }

        {
            // WHEN
            shaderProgram.setComputeShaderCode(QByteArrayLiteral("uniform vec3 temp;"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkLogPropertyUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        arbiter.setArbiterOnNode(this);
        QSignalSpy spy(this, SIGNAL(logChanged(QString)));
        const QString logValue = QStringLiteral("Some log...");

        // THEN
        QVERIFY(spy.isValid());

        // WHEN
        auto *dNode = static_cast<Qt3DRender::QShaderProgramPrivate *>(Qt3DCore::QNodePrivate::get(this));
        dNode->setLog(logValue);

        // THEN
        QCOMPARE(spy.size(), 1);
        QCOMPARE(arbiter.dirtyNodes().size(), 0);
        QCOMPARE(log(), logValue);

        // WHEN
        spy.clear();
        dNode->setLog(logValue);

        // THEN
        QCOMPARE(spy.size(), 0);
        QCOMPARE(arbiter.dirtyNodes().size(), 0);
        QCOMPARE(log(), logValue);

        // Cleanup
        Qt3DCore::QNodePrivate::get(this)->setArbiter(nullptr);
    }

    void checkStatusPropertyUpdate()
    {
        // GIVEN
        qRegisterMetaType<Qt3DRender::QShaderProgram::Status>("Status");
        TestArbiter arbiter;
        arbiter.setArbiterOnNode(this);
        QSignalSpy spy(this, SIGNAL(statusChanged(Status)));
        const Qt3DRender::QShaderProgram::Status newStatus = Qt3DRender::QShaderProgram::Error;

        // THEN
        QVERIFY(spy.isValid());

        // WHEN
        auto *dNode = static_cast<Qt3DRender::QShaderProgramPrivate *>(Qt3DCore::QNodePrivate::get(this));
        dNode->setStatus(newStatus);

        // THEN
        QCOMPARE(spy.size(), 1);
        QCOMPARE(arbiter.dirtyNodes().size(), 0);
        QCOMPARE(status(), newStatus);

        // WHEN
        spy.clear();
        dNode->setStatus(newStatus);

        // THEN
        QCOMPARE(spy.size(), 0);
        QCOMPARE(arbiter.dirtyNodes().size(), 0);
        QCOMPARE(status(), newStatus);

        // Cleanup
        Qt3DCore::QNodePrivate::get(this)->setArbiter(nullptr);
    }

    void checkIncludes()
    {
        // GIVEN
        Qt3DRender::QShaderProgram shaderProgram;
        QByteArray includedContent = shaderProgram.loadSource(QUrl(QStringLiteral("qrc:/included.frag")));

        // WHEN (test relative include)
        QByteArray mainContent = shaderProgram.loadSource(QUrl(QStringLiteral("qrc:/main.frag")));

        // THEN
        QVERIFY(mainContent.indexOf(includedContent) == 0);

        // WHEN (test absolute include)
        mainContent = shaderProgram.loadSource(QUrl(QStringLiteral("qrc:/mainabsolute.frag")));

        // THEN
        QVERIFY(mainContent.indexOf(includedContent) == 0);
    }

    void checkFormatPropertyUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QShaderProgram shaderProgram;
        arbiter.setArbiterOnNode(&shaderProgram);

        QSignalSpy spy(&shaderProgram, SIGNAL(formatChanged(Format)));

        // THEN
        QVERIFY(spy.isValid());

        {
            // WHEN
            shaderProgram.setFormat(QShaderProgram::SPIRV);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(spy.size(), 1);
            QCOMPARE(arbiter.dirtyNodes().size(), 1);

            spy.clear();
        }

        {
            // WHEN
            shaderProgram.setFormat(QShaderProgram::SPIRV);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(spy.size(), 0);
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
        }
    }

};

QTEST_MAIN(tst_QShaderProgram)

#include "tst_qshaderprogram.moc"
