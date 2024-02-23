// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DRender/qsetfence.h>
#include <Qt3DRender/private/qsetfence_p.h>
#include <QObject>
#include <QSignalSpy>
#include <Qt3DCore/private/qscene_p.h>
#include "testarbiter.h"

class MySetFence : public Qt3DRender::QSetFence
{
public:
};

class tst_QSetFence : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<Qt3DRender::QSetFence::HandleType>("HandleType");
    }

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QSetFence setFence;

        // THEN
        QCOMPARE(setFence.handleType(), Qt3DRender::QSetFence::NoHandle);
        QCOMPARE(setFence.handle(), QVariant());
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DCore::QScene scene;
        MySetFence setFence;

        Qt3DCore::QNodePrivate::get(&setFence)->setScene(&scene);

        {
            // WHEN
            QSignalSpy spy(&setFence, SIGNAL(handleTypeChanged(HandleType)));

            // THEN
            QVERIFY(spy.isValid());

            // THEN
            auto *dNode = static_cast<Qt3DRender::QSetFencePrivate *>(Qt3DRender::QSetFencePrivate::get(&setFence));
            dNode->setHandleType(Qt3DRender::QSetFence::OpenGLFenceId);

            // THEN
            QCOMPARE(setFence.handleType(), Qt3DRender::QSetFence::OpenGLFenceId);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            dNode->setHandleType(Qt3DRender::QSetFence::OpenGLFenceId);

            // THEN
            QCOMPARE(spy.size(), 0);
        }

        {
            // WHEN
            QSignalSpy spy(&setFence, SIGNAL(handleChanged(QVariant)));

            // THEN
            QVERIFY(spy.isValid());

            // WHEN
            auto *dNode = static_cast<Qt3DRender::QSetFencePrivate *>(Qt3DRender::QSetFencePrivate::get(&setFence));
            dNode->setHandle(QVariant(984));

            // THEN
            QCOMPARE(setFence.handle(),QVariant(984));
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            dNode->setHandle(QVariant(984));

            // THEN
            QCOMPARE(spy.size(), 0);
        }
    }
};

QTEST_MAIN(tst_QSetFence)

#include "tst_qsetfence.moc"
