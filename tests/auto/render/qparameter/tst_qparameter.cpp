// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/private/qparameter_p.h>
#include <QObject>
#include <QSignalSpy>
#include <Qt3DCore/qentity.h>
#include "testarbiter.h"

class tst_QParameter : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QParameter parameter;

        // THEN
        QCOMPARE(parameter.name(), QString());
        QCOMPARE(parameter.value(), QVariant());
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::QParameter parameter;

        {
            // WHEN
            QSignalSpy spy(&parameter, SIGNAL(nameChanged(QString)));
            const QString newValue = QStringLiteral("SomeName");
            parameter.setName(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(parameter.name(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            parameter.setName(newValue);

            // THEN
            QCOMPARE(parameter.name(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&parameter, SIGNAL(valueChanged(QVariant)));
            const QVariant newValue(454.0f);
            parameter.setValue(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(parameter.value(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            parameter.setValue(newValue);

            // THEN
            QCOMPARE(parameter.value(), newValue);
            QCOMPARE(spy.size(), 0);
        }
    }

    void checkNameUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QParameter parameter;
        arbiter.setArbiterOnNode(&parameter);

        {
            // WHEN
            parameter.setName(QStringLiteral("Bruce"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &parameter);

            arbiter.clear();
        }

        {
            // WHEN
            parameter.setName(QStringLiteral("Bruce"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkValueUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QParameter parameter;
        arbiter.setArbiterOnNode(&parameter);

        {
            // WHEN
            parameter.setValue(QVariant(383.0f));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &parameter);

            arbiter.clear();
        }

        {
            // WHEN
            parameter.setValue(QVariant(383.0f));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

        // WHEN -> QNode -> QNodeId
        {
            Qt3DCore::QEntity testEntity;

            {
                // WHEN
                parameter.setValue(QVariant::fromValue(&testEntity));
                QCoreApplication::processEvents();

                // THEN
                QCOMPARE(arbiter.dirtyNodes().size(), 1);
                QCOMPARE(arbiter.dirtyNodes().front(), &parameter);

                arbiter.clear();
            }

            {
                // WHEN
                parameter.setValue(QVariant::fromValue(&testEntity));
                QCoreApplication::processEvents();

                // THEN
                QCOMPARE(arbiter.dirtyNodes().size(), 0);
            }

        }

    }

    void checkBookeeping()
    {
        // GIVEN
        Qt3DRender::QParameter parameter;
        QSignalSpy spy(&parameter, SIGNAL(valueChanged(QVariant)));

        {
            // WHEN
            QScopedPointer<Qt3DCore::QNode> node(new Qt3DCore::QNode());
            parameter.setValue(QVariant::fromValue(node.data()));

            // THEN
            QCOMPARE(spy.size(), 1);
            QCOMPARE(parameter.value(), QVariant::fromValue(node.data()));
            spy.clear();
        }

        // THEN
        QCOMPARE(spy.size(), 1);
        QCOMPARE(parameter.value(), QVariant());
    }

};

QTEST_MAIN(tst_QParameter)

#include "tst_qparameter.moc"
