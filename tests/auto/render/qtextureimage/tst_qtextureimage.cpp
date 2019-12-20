/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            textureImage.setSource(newValue);

            // THEN
            QCOMPARE(textureImage.source(), newValue);
            QCOMPARE(spy.count(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&textureImage, SIGNAL(mirroredChanged(bool)));
            const bool newValue = false;
            textureImage.setMirrored(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(textureImage.isMirrored(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            textureImage.setMirrored(newValue);

            // THEN
            QCOMPARE(textureImage.isMirrored(), newValue);
            QCOMPARE(spy.count(), 0);
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
