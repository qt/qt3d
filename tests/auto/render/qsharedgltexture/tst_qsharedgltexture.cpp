// Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

// TODO Remove in Qt6
#include <QtCore/qcompilerdetection.h>
QT_WARNING_DISABLE_DEPRECATED

#include <QtTest/QTest>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/private/qtexture_p.h>
#include <QObject>
#include <QSignalSpy>
#include "testarbiter.h"

class tst_QSharedGLTexture : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QSharedGLTexture glTexture;

        // THEN
        QCOMPARE(glTexture.textureId(), -1);
        QCOMPARE(glTexture.target(), Qt3DRender::QAbstractTexture::TargetAutomatic);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::QSharedGLTexture glTexture;

        {
            // WHEN
            QSignalSpy spy(&glTexture, SIGNAL(textureIdChanged(int)));
            const int newValue = 883;
            glTexture.setTextureId(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(glTexture.textureId(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            glTexture.setTextureId(newValue);

            // THEN
            QCOMPARE(glTexture.textureId(), newValue);
            QCOMPARE(spy.size(), 0);
        }
    }

    void checkTextureIdUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QSharedGLTexture glTexture;
        arbiter.setArbiterOnNode(&glTexture);

        {
            // WHEN
            glTexture.setTextureId(1584);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &glTexture);

            arbiter.clear();
        }

        {
            // WHEN
            glTexture.setTextureId(1584);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

};

QTEST_MAIN(tst_QSharedGLTexture)

#include "tst_qsharedgltexture.moc"
