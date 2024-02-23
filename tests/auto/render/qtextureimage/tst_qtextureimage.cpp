// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

// TODO Remove in Qt6
#include <QtCore/qcompilerdetection.h>
QT_WARNING_DISABLE_DEPRECATED

#include <QtTest/QTest>
#include <Qt3DRender/qtextureimage.h>
#include <Qt3DRender/private/qtextureimage_p.h>
#include <QObject>
#include <QSignalSpy>
#include "testarbiter.h"

class tst_QTextureImage : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QTextureImage textureImage;

        // THEN
        QCOMPARE(textureImage.source(), QUrl());
        QCOMPARE(textureImage.status(), Qt3DRender::QTextureImage::None);
        QCOMPARE(textureImage.isMirrored(), true);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::QTextureImage textureImage;

        {
            // WHEN
            QSignalSpy spy(&textureImage, SIGNAL(sourceChanged(QUrl)));
            const QUrl newValue(QStringLiteral("Boulder"));
            textureImage.setSource(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(textureImage.source(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            textureImage.setSource(newValue);

            // THEN
            QCOMPARE(textureImage.source(), newValue);
            QCOMPARE(spy.size(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&textureImage, SIGNAL(mirroredChanged(bool)));
            const bool newValue = false;
            textureImage.setMirrored(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(textureImage.isMirrored(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            textureImage.setMirrored(newValue);

            // THEN
            QCOMPARE(textureImage.isMirrored(), newValue);
            QCOMPARE(spy.size(), 0);
        }
    }

    void checkSourceUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QTextureImage textureImage;
        arbiter.setArbiterOnNode(&textureImage);

        {
            // WHEN
            textureImage.setSource(QUrl(QStringLiteral("Qt3D")));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &textureImage);

            arbiter.clear();
        }

        {
            // WHEN
            textureImage.setSource(QUrl(QStringLiteral("Qt3D")));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkMirroredUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QTextureImage textureImage;
        arbiter.setArbiterOnNode(&textureImage);

        {
            // WHEN
            textureImage.setMirrored(false);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &textureImage);

            arbiter.clear();
        }

        {
            // WHEN
            textureImage.setMirrored(false);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

};

QTEST_MAIN(tst_QTextureImage)

#include "tst_qtextureimage.moc"
