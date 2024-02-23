// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <QSignalSpy>

#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qenvironmentlight.h>
#include <Qt3DRender/private/qenvironmentlight_p.h>
#include <Qt3DRender/private/qshaderdata_p.h>

#include "testarbiter.h"

class tst_QEnvironmentLight: public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QEnvironmentLight light;

        // THEN
        QVERIFY(light.findChild<Qt3DRender::QShaderData*>());
        QCOMPARE(light.irradiance(), nullptr);
        QCOMPARE(light.specular(), nullptr);
    }

    void shouldTakeOwnershipOfParentlessTextures()
    {
        // GIVEN
        Qt3DRender::QEnvironmentLight light;
        auto irradiance = new Qt3DRender::QTexture2D;
        auto specular = new Qt3DRender::QTexture2D;

        // WHEN
        light.setIrradiance(irradiance);
        light.setSpecular(specular);

        // THEN
        QCOMPARE(irradiance->parent(), &light);
        QCOMPARE(specular->parent(), &light);
    }

    void shouldNotChangeOwnershipOfParentedTextures()
    {
        // GIVEN
        Qt3DCore::QNode node;
        Qt3DRender::QEnvironmentLight light;
        auto irradiance = new Qt3DRender::QTexture2D(&node);
        auto specular = new Qt3DRender::QTexture2D(&node);

        // WHEN
        light.setIrradiance(irradiance);
        light.setSpecular(specular);

        // WHEN
        delete irradiance;
        delete specular;

        // THEN
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::QEnvironmentLight light;
        auto shaderData = light.findChild<Qt3DRender::QShaderData*>();

        {
            auto texture = new Qt3DRender::QTexture2D(&light);
            QSignalSpy spy(&light, &Qt3DRender::QEnvironmentLight::irradianceChanged);

            // WHEN
            light.setIrradiance(texture);

            // THEN
            QCOMPARE(light.irradiance(), texture);
            QCOMPARE(shaderData->property("irradiance").value<Qt3DRender::QAbstractTexture*>(), texture);
            QCOMPARE(spy.size(), 1);
            QCOMPARE(spy.takeFirst().first().value<Qt3DRender::QAbstractTexture*>(), texture);

            // WHEN
            light.setIrradiance(texture);

            // THEN
            QCOMPARE(light.irradiance(), texture);
            QCOMPARE(shaderData->property("irradiance").value<Qt3DRender::QAbstractTexture*>(), texture);
            QCOMPARE(spy.size(), 0);

            // WHEN
            light.setIrradiance(nullptr);

            // THEN
            QCOMPARE(light.irradiance(), nullptr);
            QCOMPARE(shaderData->property("irradiance").value<Qt3DRender::QAbstractTexture*>(), nullptr);
            QCOMPARE(spy.size(), 1);
            QCOMPARE(spy.takeFirst().first().value<Qt3DRender::QAbstractTexture*>(), nullptr);
        }
        {
            auto texture = new Qt3DRender::QTexture2D(&light);
            QSignalSpy spy(&light, &Qt3DRender::QEnvironmentLight::irradianceChanged);

            // WHEN
            light.setIrradiance(texture);

            // THEN
            QCOMPARE(light.irradiance(), texture);
            QCOMPARE(shaderData->property("irradiance").value<Qt3DRender::QAbstractTexture*>(), texture);
            QCOMPARE(spy.size(), 1);
            QCOMPARE(spy.takeFirst().first().value<Qt3DRender::QAbstractTexture*>(), texture);

            // WHEN
            delete texture;

            // THEN
            QCOMPARE(light.irradiance(), nullptr);
            QCOMPARE(shaderData->property("irradiance").value<Qt3DRender::QAbstractTexture*>(), nullptr);
            QCOMPARE(spy.size(), 1);
            QCOMPARE(spy.takeFirst().first().value<Qt3DRender::QAbstractTexture*>(), nullptr);
        }
        {
            auto texture = new Qt3DRender::QTexture2D;
            QSignalSpy spy(&light, &Qt3DRender::QEnvironmentLight::irradianceChanged);

            // WHEN
            light.setIrradiance(texture);

            // THEN
            QCOMPARE(light.irradiance(), texture);
            QCOMPARE(shaderData->property("irradiance").value<Qt3DRender::QAbstractTexture*>(), texture);
            QCOMPARE(spy.size(), 1);
            QCOMPARE(spy.takeFirst().first().value<Qt3DRender::QAbstractTexture*>(), texture);

            // WHEN
            delete texture;

            // THEN
            QCOMPARE(light.irradiance(), nullptr);
            QCOMPARE(shaderData->property("irradiance").value<Qt3DRender::QAbstractTexture*>(), nullptr);
            QCOMPARE(spy.size(), 1);
            QCOMPARE(spy.takeFirst().first().value<Qt3DRender::QAbstractTexture*>(), nullptr);
        }
        {
            auto texture = new Qt3DRender::QTexture2D(&light);
            QSignalSpy spy(&light, &Qt3DRender::QEnvironmentLight::irradianceChanged);

            // WHEN
            light.setIrradiance(texture);

            // THEN
            QCOMPARE(light.irradiance(), texture);
            QCOMPARE(shaderData->property("irradiance").value<Qt3DRender::QAbstractTexture*>(), texture);
            QCOMPARE(shaderData->property("irradianceSize").value<QVector3D>(),
                     QVector3D(texture->width(), texture->height(), texture->depth()));
            QCOMPARE(spy.size(), 1);
            QCOMPARE(spy.takeFirst().first().value<Qt3DRender::QAbstractTexture*>(), texture);

            // WHEN
            texture->setWidth(883);

            // THEN
            QCOMPARE(shaderData->property("irradianceSize").value<QVector3D>(),
                     QVector3D(883.0f, texture->height(), texture->depth()));

            // WHEN
            texture->setHeight(1340);

            // THEN
            QCOMPARE(shaderData->property("irradianceSize").value<QVector3D>(),
                     QVector3D(883.0f, 1340.0f, texture->depth()));

            // WHEN
            texture->setDepth(1584);

            // THEN
            QCOMPARE(shaderData->property("irradianceSize").value<QVector3D>(),
                     QVector3D(883.0f, 1340.0f, 1584.0f));

            // WHEN
            delete texture;

            // THEN
            QCOMPARE(light.irradiance(), nullptr);
            QCOMPARE(shaderData->property("irradiance").value<Qt3DRender::QAbstractTexture*>(), nullptr);
            QCOMPARE(spy.size(), 1);
            QCOMPARE(spy.takeFirst().first().value<Qt3DRender::QAbstractTexture*>(), nullptr);

            // THEN
            QCOMPARE(shaderData->property("irradianceSize").value<QVector3D>(),
                     QVector3D());
        }
        {
            auto texture = new Qt3DRender::QTexture2D(&light);
            QSignalSpy spy(&light, &Qt3DRender::QEnvironmentLight::specularChanged);

            // WHEN
            light.setSpecular(texture);

            // THEN
            QCOMPARE(light.specular(), texture);
            QCOMPARE(shaderData->property("specular").value<Qt3DRender::QAbstractTexture*>(), texture);
            QCOMPARE(spy.size(), 1);
            QCOMPARE(spy.takeFirst().first().value<Qt3DRender::QAbstractTexture*>(), texture);

            // WHEN
            light.setSpecular(texture);

            // THEN
            QCOMPARE(light.specular(), texture);
            QCOMPARE(shaderData->property("specular").value<Qt3DRender::QAbstractTexture*>(), texture);
            QCOMPARE(spy.size(), 0);

            // WHEN
            light.setSpecular(nullptr);

            // THEN
            QCOMPARE(light.specular(), nullptr);
            QCOMPARE(shaderData->property("specular").value<Qt3DRender::QAbstractTexture*>(), nullptr);
            QCOMPARE(spy.size(), 1);
            QCOMPARE(spy.takeFirst().first().value<Qt3DRender::QAbstractTexture*>(), nullptr);
        }
        {
            auto texture = new Qt3DRender::QTexture2D(&light);
            QSignalSpy spy(&light, &Qt3DRender::QEnvironmentLight::specularChanged);

            // WHEN
            light.setSpecular(texture);

            // THEN
            QCOMPARE(light.specular(), texture);
            QCOMPARE(shaderData->property("specular").value<Qt3DRender::QAbstractTexture*>(), texture);
            QCOMPARE(spy.size(), 1);
            QCOMPARE(spy.takeFirst().first().value<Qt3DRender::QAbstractTexture*>(), texture);

            // WHEN
            delete texture;

            // THEN
            QCOMPARE(light.specular(), nullptr);
            QCOMPARE(shaderData->property("specular").value<Qt3DRender::QAbstractTexture*>(), nullptr);
            QCOMPARE(spy.size(), 1);
            QCOMPARE(spy.takeFirst().first().value<Qt3DRender::QAbstractTexture*>(), nullptr);
        }
        {
            auto texture = new Qt3DRender::QTexture2D;
            QSignalSpy spy(&light, &Qt3DRender::QEnvironmentLight::specularChanged);

            // WHEN
            light.setSpecular(texture);

            // THEN
            QCOMPARE(light.specular(), texture);
            QCOMPARE(shaderData->property("specular").value<Qt3DRender::QAbstractTexture*>(), texture);
            QCOMPARE(spy.size(), 1);
            QCOMPARE(spy.takeFirst().first().value<Qt3DRender::QAbstractTexture*>(), texture);

            // WHEN
            delete texture;

            // THEN
            QCOMPARE(light.specular(), nullptr);
            QCOMPARE(shaderData->property("specular").value<Qt3DRender::QAbstractTexture*>(), nullptr);
            QCOMPARE(spy.size(), 1);
            QCOMPARE(spy.takeFirst().first().value<Qt3DRender::QAbstractTexture*>(), nullptr);
        }
        {
            auto texture = new Qt3DRender::QTexture2D(&light);
            QSignalSpy spy(&light, &Qt3DRender::QEnvironmentLight::specularChanged);

            // WHEN
            light.setSpecular(texture);

            // THEN
            QCOMPARE(light.specular(), texture);
            QCOMPARE(shaderData->property("specular").value<Qt3DRender::QAbstractTexture*>(), texture);
            QCOMPARE(shaderData->property("specularSize").value<QVector3D>(),
                     QVector3D(texture->width(), texture->height(), texture->depth()));
            QCOMPARE(spy.size(), 1);
            QCOMPARE(spy.takeFirst().first().value<Qt3DRender::QAbstractTexture*>(), texture);

            // WHEN
            texture->setWidth(883);

            // THEN
            QCOMPARE(shaderData->property("specularSize").value<QVector3D>(),
                     QVector3D(883.0f, texture->height(), texture->depth()));

            // WHEN
            texture->setHeight(1340);

            // THEN
            QCOMPARE(shaderData->property("specularSize").value<QVector3D>(),
                     QVector3D(883.0f, 1340.0f, texture->depth()));

            // WHEN
            texture->setDepth(1584);

            // THEN
            QCOMPARE(shaderData->property("specularSize").value<QVector3D>(),
                     QVector3D(883.0f, 1340.0f, 1584.0f));

            // WHEN
            delete texture;

            // THEN
            QCOMPARE(light.specular(), nullptr);
            QCOMPARE(shaderData->property("specular").value<Qt3DRender::QAbstractTexture*>(), nullptr);
            QCOMPARE(spy.size(), 1);
            QCOMPARE(spy.takeFirst().first().value<Qt3DRender::QAbstractTexture*>(), nullptr);

            // THEN
            QCOMPARE(shaderData->property("specularSize").value<QVector3D>(),
                     QVector3D());
        }
    }
};

QTEST_MAIN(tst_QEnvironmentLight)

#include "tst_qenvironmentlight.moc"
