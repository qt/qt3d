/****************************************************************************
**
** Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
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

#include <QtTest/QTest>
#include <Qt3DRender/qtexturedataupdate.h>
#include <Qt3DRender/qabstracttexture.h>
#include <Qt3DRender/qtextureimagedata.h>

class tst_QTextureDataUpdate : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QTextureDataUpdate textureUpdate;

        // THEN
        QCOMPARE(textureUpdate.x(), 0);
        QCOMPARE(textureUpdate.y(), 0);
        QCOMPARE(textureUpdate.z(), 0);
        QCOMPARE(textureUpdate.layer(), 0);
        QCOMPARE(textureUpdate.mipLevel(), 0);
        QCOMPARE(textureUpdate.face(), Qt3DRender::QAbstractTexture::CubeMapPositiveX);
        QVERIFY(textureUpdate.data().isNull());
    }

    void checkSettersAndGetters()
    {
        // GIVEN
        Qt3DRender::QTextureDataUpdate textureUpdate;

        // WHEN
        textureUpdate.setX(883);

        // THEN
        QCOMPARE(textureUpdate.x(), 883);

        // WHEN
        textureUpdate.setY(1340);
        // THEN
        QCOMPARE(textureUpdate.y(), 1340);

        // WHEN
        textureUpdate.setZ(1584);

        // THEN
        QCOMPARE(textureUpdate.z(), 1584);

        // WHEN
        textureUpdate.setLayer(454);

        // THEN
        QCOMPARE(textureUpdate.layer(), 454);

        // WHEN
        textureUpdate.setMipLevel(350);
        // THEN
        QCOMPARE(textureUpdate.mipLevel(), 350);

        // WHEN
        textureUpdate.setFace(Qt3DRender::QAbstractTexture::CubeMapNegativeZ);

        // THEN
        QCOMPARE(textureUpdate.face(), Qt3DRender::QAbstractTexture::CubeMapNegativeZ);

        // WHEN
        Qt3DRender::QTextureImageDataPtr imgData = Qt3DRender::QTextureImageDataPtr::create();
        textureUpdate.setData(imgData);

        // THEN
        QCOMPARE(textureUpdate.data(), imgData);
    }

    void checkCopyConstruction()
    {
        // GIVEN
        Qt3DRender::QTextureDataUpdate textureUpdate;
        textureUpdate.setX(883);
        textureUpdate.setY(1340);
        textureUpdate.setZ(1584);
        textureUpdate.setLayer(454);
        textureUpdate.setMipLevel(350);
        textureUpdate.setFace(Qt3DRender::QAbstractTexture::CubeMapNegativeZ);
        Qt3DRender::QTextureImageDataPtr imgData = Qt3DRender::QTextureImageDataPtr::create();
        textureUpdate.setData(imgData);

        // WHEN
        Qt3DRender::QTextureDataUpdate textureUpdateCpy(textureUpdate);

        // THEN
        QCOMPARE(textureUpdateCpy.x(), 883);
        QCOMPARE(textureUpdateCpy.y(), 1340);
        QCOMPARE(textureUpdateCpy.z(), 1584);
        QCOMPARE(textureUpdateCpy.layer(), 454);
        QCOMPARE(textureUpdateCpy.mipLevel(), 350);
        QCOMPARE(textureUpdateCpy.face(), Qt3DRender::QAbstractTexture::CubeMapNegativeZ);
        QCOMPARE(textureUpdateCpy.data(), imgData);
    }

    void checkAssigment()
    {
        // GIVEN
        Qt3DRender::QTextureDataUpdate textureUpdate;
        Qt3DRender::QTextureDataUpdate textureUpdateAssign;
        textureUpdate.setX(883);
        textureUpdate.setY(1340);
        textureUpdate.setZ(1584);
        textureUpdate.setLayer(454);
        textureUpdate.setMipLevel(350);
        textureUpdate.setFace(Qt3DRender::QAbstractTexture::CubeMapNegativeZ);
        Qt3DRender::QTextureImageDataPtr imgData = Qt3DRender::QTextureImageDataPtr::create();
        textureUpdate.setData(imgData);

        // WHEN
        textureUpdateAssign = textureUpdate;

        // THEN
        QCOMPARE(textureUpdateAssign.x(), 883);
        QCOMPARE(textureUpdateAssign.y(), 1340);
        QCOMPARE(textureUpdateAssign.z(), 1584);
        QCOMPARE(textureUpdateAssign.layer(), 454);
        QCOMPARE(textureUpdateAssign.mipLevel(), 350);
        QCOMPARE(textureUpdateAssign.face(), Qt3DRender::QAbstractTexture::CubeMapNegativeZ);
        QCOMPARE(textureUpdateAssign.data(), imgData);

        QCOMPARE(textureUpdateAssign.x(), textureUpdate.x());
        QCOMPARE(textureUpdateAssign.y(), textureUpdate.y());
        QCOMPARE(textureUpdateAssign.z(), textureUpdate.z());
        QCOMPARE(textureUpdateAssign.layer(), textureUpdate.layer());
        QCOMPARE(textureUpdateAssign.mipLevel(), textureUpdate.mipLevel());
        QCOMPARE(textureUpdateAssign.face(), textureUpdate.face());
        QCOMPARE(textureUpdateAssign.data(), textureUpdate.data());
    }

    void checkConversionToVariant()
    {
        // GIVEN
        Qt3DRender::QTextureDataUpdate textureUpdate;
        textureUpdate.setX(883);
        textureUpdate.setY(1340);
        textureUpdate.setZ(1584);
        textureUpdate.setLayer(454);
        textureUpdate.setMipLevel(350);
        textureUpdate.setFace(Qt3DRender::QAbstractTexture::CubeMapNegativeZ);
        Qt3DRender::QTextureImageDataPtr imgData = Qt3DRender::QTextureImageDataPtr::create();
        textureUpdate.setData(imgData);

        // WHEN
        const QVariant v = QVariant::fromValue(textureUpdate);
        const Qt3DRender::QTextureDataUpdate out = v.value<Qt3DRender::QTextureDataUpdate>();

        // THEN
        QCOMPARE(out.x(), 883);
        QCOMPARE(out.y(), 1340);
        QCOMPARE(out.z(), 1584);
        QCOMPARE(out.layer(), 454);
        QCOMPARE(out.mipLevel(), 350);
        QCOMPARE(out.face(), Qt3DRender::QAbstractTexture::CubeMapNegativeZ);
        QCOMPARE(out.data(), imgData);
    }

    void checkComparison()
    {
        // GIVEN
        Qt3DRender::QTextureDataUpdate textureUpdate1;
        Qt3DRender::QTextureDataUpdate textureUpdate2;

        // THEN
        QVERIFY(textureUpdate1 == textureUpdate2);
        QVERIFY(!(textureUpdate1 != textureUpdate2));

        // WHEN
        textureUpdate1.setX(883);

        // THEN
        QVERIFY(textureUpdate1 != textureUpdate2);

        // WHEN
        textureUpdate2.setX(883);

        // THEN
        QVERIFY(textureUpdate1 == textureUpdate2);

        // WHEN
        textureUpdate1.setY(883);

        // THEN
        QVERIFY(textureUpdate1 != textureUpdate2);

        // WHEN
        textureUpdate2.setY(883);

        // THEN
        QVERIFY(textureUpdate1 == textureUpdate2);

        // WHEN
        textureUpdate1.setZ(883);

        // THEN
        QVERIFY(textureUpdate1 != textureUpdate2);

        // WHEN
        textureUpdate2.setZ(883);

        // THEN
        QVERIFY(textureUpdate1 == textureUpdate2);

        // WHEN
        textureUpdate1.setLayer(883);

        // THEN
        QVERIFY(textureUpdate1 != textureUpdate2);

        // WHEN
        textureUpdate2.setLayer(883);

        // THEN
        QVERIFY(textureUpdate1 == textureUpdate2);

        // WHEN
        textureUpdate1.setMipLevel(883);

        // THEN
        QVERIFY(textureUpdate1 != textureUpdate2);

        // WHEN
        textureUpdate2.setMipLevel(883);

        // THEN
        QVERIFY(textureUpdate1 == textureUpdate2);

        // WHEN
        textureUpdate1.setFace(Qt3DRender::QAbstractTexture::CubeMapNegativeX);

        // THEN
        QVERIFY(textureUpdate1 != textureUpdate2);

        // WHEN
        textureUpdate2.setFace(Qt3DRender::QAbstractTexture::CubeMapNegativeX);

        // THEN
        QVERIFY(textureUpdate1 == textureUpdate2);

        // WHEN
        Qt3DRender::QTextureImageDataPtr imgData = Qt3DRender::QTextureImageDataPtr::create();
        textureUpdate1.setData(imgData);

        // THEN
        QVERIFY(textureUpdate1 != textureUpdate2);

        // WHEN
        textureUpdate2.setData(imgData);

        // THEN
        QVERIFY(textureUpdate1 == textureUpdate2);
    }
};

QTEST_APPLESS_MAIN(tst_QTextureDataUpdate)

#include "tst_qtexturedataupdate.moc"
