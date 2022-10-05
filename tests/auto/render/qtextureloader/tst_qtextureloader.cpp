// Copyright (C) 2016 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

// TODO Remove in Qt6
#include <QtCore/qcompilerdetection.h>
QT_WARNING_DISABLE_DEPRECATED

#include <QtTest/QTest>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/private/qtexture_p.h>
#include <QObject>
#include <QSignalSpy>
#include "testarbiter.h"

class tst_QTextureLoader : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QTextureLoader textureLoader;

        // THEN
        QCOMPARE(textureLoader.source(), QUrl());
        QCOMPARE(textureLoader.isMirrored(), true);
        QCOMPARE(textureLoader.target(), Qt3DRender::QTextureLoader::TargetAutomatic);
        QCOMPARE(textureLoader.wrapMode()->x(), Qt3DRender::QTextureWrapMode::Repeat);
        QCOMPARE(textureLoader.wrapMode()->y(), Qt3DRender::QTextureWrapMode::Repeat);
        QCOMPARE(textureLoader.magnificationFilter(), Qt3DRender::QTextureLoader::Linear);
        QCOMPARE(textureLoader.minificationFilter(),  Qt3DRender::QTextureLoader::LinearMipMapLinear);
        QCOMPARE(textureLoader.generateMipMaps(), true);
        QCOMPARE(textureLoader.maximumAnisotropy(), 16.0f);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::QTextureLoader textureLoader;

        {
            // WHEN
            QSignalSpy spy(&textureLoader, SIGNAL(sourceChanged(QUrl)));
            const QUrl newValue(QStringLiteral("http://msn.com"));
            textureLoader.setSource(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(textureLoader.source(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            textureLoader.setSource(newValue);

            // THEN
            QCOMPARE(textureLoader.source(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&textureLoader, SIGNAL(mirroredChanged(bool)));
            const bool newValue = false;
            textureLoader.setMirrored(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(textureLoader.isMirrored(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            textureLoader.setMirrored(newValue);

            // THEN
            QCOMPARE(textureLoader.isMirrored(), newValue);
            QCOMPARE(spy.size(), 0);
        }
    }

    void checkSourceUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QTextureLoader textureLoader;
        arbiter.setArbiterOnNode(&textureLoader);

        {
            // WHEN
            textureLoader.setSource(QUrl(QStringLiteral("Gary")));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &textureLoader);

            Qt3DRender::QAbstractTexturePrivate *d = dynamic_cast<Qt3DRender::QAbstractTexturePrivate *>(Qt3DRender::QAbstractTexturePrivate::get(&textureLoader));
            const auto generator = qSharedPointerCast<Qt3DRender::QTextureFromSourceGenerator>(d->dataFunctor());
            QVERIFY(generator);
            QCOMPARE(generator->url(), QUrl(QStringLiteral("Gary")));

            arbiter.clear();
        }
    }

    void checkMirroredUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QTextureLoader textureLoader;
        arbiter.setArbiterOnNode(&textureLoader);

        {
            // WHEN
            textureLoader.setMirrored(false);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &textureLoader);

            Qt3DRender::QAbstractTexturePrivate *d = dynamic_cast<Qt3DRender::QAbstractTexturePrivate *>(Qt3DRender::QAbstractTexturePrivate::get(&textureLoader));
            const auto generator = qSharedPointerCast<Qt3DRender::QTextureFromSourceGenerator>(d->dataFunctor());
            QVERIFY(generator);
            QCOMPARE(generator->isMirrored(), false);

            arbiter.clear();
        }

        {
            // WHEN
            textureLoader.setMirrored(false);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

};

QTEST_MAIN(tst_QTextureLoader)

#include "tst_qtextureloader.moc"
