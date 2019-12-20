/****************************************************************************
**
** Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QTest>
#include <Qt3DRender/qshaderimage.h>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/private/qshaderimage_p.h>
#include <QObject>
#include <QSignalSpy>
#include "testarbiter.h"

class tst_QShaderImage : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<Qt3DRender::QAbstractTexture*>("Qt3DRender::QAbstractTexture*");

    }

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QShaderImage shaderImage;

        // THEN
        QVERIFY(shaderImage.texture() == nullptr);
        QCOMPARE(shaderImage.layered(), false);
        QCOMPARE(shaderImage.mipLevel(), 0);
        QCOMPARE(shaderImage.layer(), 0);
        QCOMPARE(shaderImage.access(), Qt3DRender::QShaderImage::ReadWrite);
        QCOMPARE(shaderImage.format(), Qt3DRender::QShaderImage::Automatic);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::QShaderImage shaderImage;

        {
            // WHEN
            QSignalSpy spy(&shaderImage, SIGNAL(textureChanged(Qt3DRender::QAbstractTexture *)));
            Qt3DRender::QTexture2D newValue;
            shaderImage.setTexture(&newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(shaderImage.texture(), &newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            shaderImage.setTexture(&newValue);

            // THEN
            QCOMPARE(shaderImage.texture(), &newValue);
            QCOMPARE(spy.count(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&shaderImage, SIGNAL(layeredChanged(bool)));
            const bool newValue = true;
            shaderImage.setLayered(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(shaderImage.layered(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            shaderImage.setLayered(newValue);

            // THEN
            QCOMPARE(shaderImage.layered(), newValue);
            QCOMPARE(spy.count(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&shaderImage, SIGNAL(mipLevelChanged(int)));
            const int newValue = 12;
            shaderImage.setMipLevel(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(shaderImage.mipLevel(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            shaderImage.setMipLevel(newValue);

            // THEN
            QCOMPARE(shaderImage.mipLevel(), newValue);
            QCOMPARE(spy.count(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&shaderImage, SIGNAL(layerChanged(int)));
            const int newValue = 2;
            shaderImage.setLayer(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(shaderImage.layer(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            shaderImage.setLayer(newValue);

            // THEN
            QCOMPARE(shaderImage.layer(), newValue);
            QCOMPARE(spy.count(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&shaderImage, SIGNAL(accessChanged(Access)));
            const Qt3DRender::QShaderImage::Access newValue = Qt3DRender::QShaderImage::ReadOnly;
            shaderImage.setAccess(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(shaderImage.access(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            shaderImage.setAccess(newValue);

            // THEN
            QCOMPARE(shaderImage.access(), newValue);
            QCOMPARE(spy.count(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&shaderImage, SIGNAL(formatChanged(ImageFormat)));
            const Qt3DRender::QShaderImage::ImageFormat newValue = Qt3DRender::QShaderImage::RG8U;
            shaderImage.setFormat(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(shaderImage.format(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            shaderImage.setFormat(newValue);

            // THEN
            QCOMPARE(shaderImage.format(), newValue);
            QCOMPARE(spy.count(), 0);
        }
    }

    void checkTextureUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QShaderImage shaderImage;
        arbiter.setArbiterOnNode(&shaderImage);
        Qt3DRender::QTexture2D t;

        {
            // WHEN
            shaderImage.setTexture(&t);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &shaderImage);

            arbiter.clear();
        }

        {
            // WHEN
            shaderImage.setTexture(&t);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkLayeredUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QShaderImage shaderImage;
        arbiter.setArbiterOnNode(&shaderImage);

        {
            // WHEN
            shaderImage.setLayered(true);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &shaderImage);

            arbiter.clear();
        }

        {
            // WHEN
            shaderImage.setLayered(true);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkMipLevelUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QShaderImage shaderImage;
        arbiter.setArbiterOnNode(&shaderImage);

        {
            // WHEN
            shaderImage.setMipLevel(5);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &shaderImage);

            arbiter.clear();
        }

        {
            // WHEN
            shaderImage.setMipLevel(5);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkLayerUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QShaderImage shaderImage;
        arbiter.setArbiterOnNode(&shaderImage);

        {
            // WHEN
            shaderImage.setLayer(8);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &shaderImage);

            arbiter.clear();
        }

        {
            // WHEN
            shaderImage.setLayer(8);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkAccessUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QShaderImage shaderImage;
        arbiter.setArbiterOnNode(&shaderImage);

        {
            // WHEN
            shaderImage.setAccess(Qt3DRender::QShaderImage::WriteOnly);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &shaderImage);

            arbiter.clear();
        }

        {
            // WHEN
            shaderImage.setAccess(Qt3DRender::QShaderImage::WriteOnly);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkFormatUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QShaderImage shaderImage;
        arbiter.setArbiterOnNode(&shaderImage);

        {
            // WHEN
            shaderImage.setFormat(Qt3DRender::QShaderImage::RG16F);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &shaderImage);

            arbiter.clear();
        }

        {
            // WHEN
            shaderImage.setFormat(Qt3DRender::QShaderImage::RG16F);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

    }

    void checkTextureBookeeping()
    {
        // GIVEN
        Qt3DRender::QShaderImage shaderImage;
        QSignalSpy spy(&shaderImage, SIGNAL(textureChanged(Qt3DRender::QAbstractTexture *)));

        {
            // WHEN
            Qt3DRender::QTexture2D tex;
            shaderImage.setTexture(&tex);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(shaderImage.texture(), &tex);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
        }
        // THEN
        QCOMPARE(spy.count(), 1);
        QVERIFY(shaderImage.texture() == nullptr);
    }

};

QTEST_MAIN(tst_QShaderImage)

#include "tst_qshaderimage.moc"
