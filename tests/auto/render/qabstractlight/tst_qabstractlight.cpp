/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
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
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/qnodecreatedchangegenerator_p.h>

#include <Qt3DRender/qabstractlight.h>
#include <Qt3DRender/private/qabstractlight_p.h>
#include <Qt3DRender/qpointlight.h>
#include <Qt3DRender/qdirectionallight.h>
#include <Qt3DRender/qspotlight.h>
#include <Qt3DRender/private/qpointlight_p.h>
#include <Qt3DRender/private/qdirectionallight_p.h>
#include <Qt3DRender/private/qspotlight_p.h>

#include "testpostmanarbiter.h"

class DummyLight : public Qt3DRender::QAbstractLight
{
    Q_OBJECT

public:
    explicit DummyLight(Qt3DCore::QNode *parent = Q_NULLPTR)
        : QAbstractLight(*new Qt3DRender::QAbstractLightPrivate(QAbstractLight::PointLight), parent)
    {}
};


// We need to call QNode::clone which is protected
// So we sublcass QNode instead of QObject
class tst_QAbstractLight: public Qt3DCore::QNode
{
    Q_OBJECT

private Q_SLOTS:
    // TO DO: Test should be rewritten to query the properties from the attached QShaderData

//    void checkLightCloning()
//    {
//        // GIVEN
//        DummyLight light;
//        light.setColor(Qt::red);
//        light.setIntensity(0.5f);

//        // WHEN
//        Qt3DCore::QNodeCreatedChangeGenerator creationChangeGenerator(material);
//        QVector<Qt3DCore::QNodeCreatedChangeBasePtr> creationChanges = creationChangeGenerator.creationChanges();

//        // THEN
//        QVERIFY(creationChanges.size() >= 1);

//        const Qt3DCore::QNodeCreatedChangePtr<Qt3DRender::QA> creationChangeData =
//                qSharedPointerCast<Qt3DCore::QNodeCreatedChange<Qt3DRender::QMaterialData>>(creationChanges.first());
//        const Qt3DRender::QMaterialData &cloneData = creationChangeData->data;


//        QScopedPointer<Qt3DRender::QAbstractLight> lightClone(static_cast<Qt3DRender::QAbstractLight *>(QNode::clone(&light)));
//        QVERIFY(lightClone.data());
//        QCOMPARE(light.color(), lightClone->color());
//        QCOMPARE(light.intensity(), lightClone->intensity());
//    }

//    void checkPointLightCloning()
//    {
//        Qt3DRender::QPointLight pointLight;
//        QCOMPARE(pointLight.type(), Qt3DRender::QAbstractLight::PointLight);
//        pointLight.setColor(Qt::green);
//        pointLight.setIntensity(0.5f);
//        pointLight.setConstantAttenuation(0.5f);
//        pointLight.setLinearAttenuation(0.0f);      // No actual event triggered as 0.0f is default
//        pointLight.setQuadraticAttenuation(1.0f);

//        QScopedPointer<Qt3DRender::QPointLight> pointLightClone(static_cast<Qt3DRender::QPointLight *>(QNode::clone(&pointLight)));
//        QVERIFY(pointLightClone.data());
//        QCOMPARE(pointLightClone->type(), Qt3DRender::QAbstractLight::PointLight);
//        QCOMPARE(pointLight.color(), pointLightClone->color());
//        QCOMPARE(pointLight.intensity(), pointLightClone->intensity());
//        QCOMPARE(pointLight.constantAttenuation(), pointLightClone->constantAttenuation());
//        QCOMPARE(pointLight.linearAttenuation(), pointLightClone->linearAttenuation());
//        QCOMPARE(pointLight.quadraticAttenuation(), pointLightClone->quadraticAttenuation());
//    }

//    void checkDirectionalLightCloning()
//    {
//        Qt3DRender::QDirectionalLight dirLight;
//        QCOMPARE(dirLight.type(), Qt3DRender::QAbstractLight::DirectionalLight);
//        dirLight.setColor(Qt::blue);
//        dirLight.setIntensity(0.5f);
//        dirLight.setWorldDirection(QVector3D(0, 0, -1));

//        QScopedPointer<Qt3DRender::QDirectionalLight> dirLightClone(static_cast<Qt3DRender::QDirectionalLight *>(QNode::clone(&dirLight)));
//        QVERIFY(dirLightClone.data());
//        QCOMPARE(dirLightClone->type(), Qt3DRender::QAbstractLight::DirectionalLight);
//        QCOMPARE(dirLight.color(), dirLightClone->color());
//        QCOMPARE(dirLight.intensity(), dirLightClone->intensity());
//        QCOMPARE(dirLight.worldDirection(), dirLightClone->worldDirection());
//    }

//    void checkSpotLightCloning()
//    {
//        Qt3DRender::QSpotLight spotLight;
//        QCOMPARE(spotLight.type(), Qt3DRender::QAbstractLight::SpotLight);
//        spotLight.setColor(Qt::lightGray);
//        spotLight.setIntensity(0.5f);
//        spotLight.setLocalDirection(QVector3D(0, 0, -1));
//        spotLight.setCutOffAngle(0.75f);

//        QScopedPointer<Qt3DRender::QSpotLight> spotLightClone(static_cast<Qt3DRender::QSpotLight *>(QNode::clone(&spotLight)));
//        QVERIFY(spotLightClone.data());
//        QCOMPARE(spotLightClone->type(), Qt3DRender::QAbstractLight::SpotLight);
//        QCOMPARE(spotLight.color(), spotLightClone->color());
//        QCOMPARE(spotLight.intensity(), spotLightClone->intensity());
//        QCOMPARE(spotLight.localDirection(), spotLightClone->localDirection());
//        QCOMPARE(spotLight.cutOffAngle(), spotLightClone->cutOffAngle());
//    }

    void checkLightPropertyUpdates()
    {
        QScopedPointer<Qt3DRender::QAbstractLight> light(new DummyLight);
        TestArbiter lightArbiter(light.data());

        light->setColor(Qt::red);
        light->setIntensity(0.5f);
        QCoreApplication::processEvents();

        QCOMPARE(lightArbiter.events.size(), 2);
        Qt3DCore::QNodePropertyChangePtr change = lightArbiter.events[0].staticCast<Qt3DCore::QNodePropertyChange>();
        QCOMPARE(change->propertyName(), "color");
        QCOMPARE(change->subjectId(), light->id());
        QCOMPARE(change->value().value<QColor>(), QColor(Qt::red));
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);
        change = lightArbiter.events[1].staticCast<Qt3DCore::QNodePropertyChange>();
        QCOMPARE(change->propertyName(), "intensity");
        QCOMPARE(change->subjectId(), light->id());
        QCOMPARE(change->value().value<float>(), 0.5f);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        lightArbiter.events.clear();

        light->setColor(Qt::red);
        QCoreApplication::processEvents();

        QCOMPARE(lightArbiter.events.size(), 0);

        lightArbiter.events.clear();
    }

    void checkPointLightPropertyUpdates()
    {
        QScopedPointer<Qt3DRender::QPointLight> pointLight(new Qt3DRender::QPointLight);
        TestArbiter pointLightArbiter(pointLight.data());

        pointLight->setColor(Qt::green);
        pointLight->setIntensity(0.5f);
        pointLight->setConstantAttenuation(0.5f);
        pointLight->setLinearAttenuation(0.0f);     // No actual event triggered as 0.0f is default
        pointLight->setQuadraticAttenuation(1.0f);
        QCoreApplication::processEvents();

        QCOMPARE(pointLightArbiter.events.size(), 4);
        Qt3DCore::QNodePropertyChangePtr change = pointLightArbiter.events[0].staticCast<Qt3DCore::QNodePropertyChange>();
        QCOMPARE(change->propertyName(), "color");
        QCOMPARE(change->subjectId(), pointLight->id());
        QCOMPARE(change->value().value<QColor>(), QColor(Qt::green));
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);
        change = pointLightArbiter.events[1].staticCast<Qt3DCore::QNodePropertyChange>();
        QCOMPARE(change->propertyName(), "intensity");
        QCOMPARE(change->subjectId(), pointLight->id());
        QCOMPARE(change->value().value<float>(), 0.5f);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);
        change = pointLightArbiter.events[2].staticCast<Qt3DCore::QNodePropertyChange>();
        QCOMPARE(change->propertyName(), "constantAttenuation");
        QCOMPARE(change->subjectId(), pointLight->id());
        QCOMPARE(change->value().value<float>(), 0.5f);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);
        change = pointLightArbiter.events[3].staticCast<Qt3DCore::QNodePropertyChange>();
        QCOMPARE(change->propertyName(), "quadraticAttenuation");
        QCOMPARE(change->subjectId(), pointLight->id());
        QCOMPARE(change->value().value<float>(), 1.0f);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        pointLightArbiter.events.clear();
    }

    void checkDirectionalLightPropertyUpdates()
    {
        QScopedPointer<Qt3DRender::QDirectionalLight> dirLight(new Qt3DRender::QDirectionalLight);
        TestArbiter dirLightArbiter(dirLight.data());

        dirLight->setColor(Qt::blue);
        dirLight->setIntensity(0.5f);
        dirLight->setWorldDirection(QVector3D(0.5f, 0.0f, -1.0f));
        QCoreApplication::processEvents();

        QCOMPARE(dirLightArbiter.events.size(), 3);
        Qt3DCore::QNodePropertyChangePtr change = dirLightArbiter.events[0].staticCast<Qt3DCore::QNodePropertyChange>();
        QCOMPARE(change->propertyName(), "color");
        QCOMPARE(change->subjectId(), dirLight->id());
        QCOMPARE(change->value().value<QColor>(), QColor(Qt::blue));
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);
        change = dirLightArbiter.events[1].staticCast<Qt3DCore::QNodePropertyChange>();
        QCOMPARE(change->propertyName(), "intensity");
        QCOMPARE(change->subjectId(), dirLight->id());
        QCOMPARE(change->value().value<float>(), 0.5f);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);
        change = dirLightArbiter.events[2].staticCast<Qt3DCore::QNodePropertyChange>();
        QCOMPARE(change->propertyName(), "worldDirection");
        QCOMPARE(change->subjectId(), dirLight->id());
        QCOMPARE(change->value().value<QVector3D>(), QVector3D(0.5f, 0.0f, -1.0f));
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        dirLightArbiter.events.clear();
    }

    void checkSpotLightPropertyUpdates()
    {
        QScopedPointer<Qt3DRender::QSpotLight> spotLight(new Qt3DRender::QSpotLight);
        TestArbiter spotLightArbiter(spotLight.data());

        spotLight->setColor(Qt::lightGray);
        spotLight->setIntensity(0.5f);
        spotLight->setLocalDirection(QVector3D(0.5f, 0.0f, -1.0f));
        spotLight->setCutOffAngle(0.75f);
        QCoreApplication::processEvents();

        QCOMPARE(spotLightArbiter.events.size(), 4);
        Qt3DCore::QNodePropertyChangePtr change = spotLightArbiter.events[0].staticCast<Qt3DCore::QNodePropertyChange>();
        QCOMPARE(change->propertyName(), "color");
        QCOMPARE(change->subjectId(), spotLight->id());
        QCOMPARE(change->value().value<QColor>(), QColor(Qt::lightGray));
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);
        change = spotLightArbiter.events[1].staticCast<Qt3DCore::QNodePropertyChange>();
        QCOMPARE(change->propertyName(), "intensity");
        QCOMPARE(change->subjectId(), spotLight->id());
        QCOMPARE(change->value().value<float>(), 0.5f);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);
        change = spotLightArbiter.events[2].staticCast<Qt3DCore::QNodePropertyChange>();
        QCOMPARE(change->propertyName(), "localDirection");
        QCOMPARE(change->subjectId(), spotLight->id());
        QCOMPARE(change->value().value<QVector3D>(), QVector3D(0.5f, 0.0f, -1.0f));
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);
        change = spotLightArbiter.events[3].staticCast<Qt3DCore::QNodePropertyChange>();
        QCOMPARE(change->propertyName(), "cutOffAngle");
        QCOMPARE(change->subjectId(), spotLight->id());
        QCOMPARE(change->value().value<float>(), 0.75f);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        spotLightArbiter.events.clear();
    }
};

QTEST_MAIN(tst_QAbstractLight)

#include "tst_qabstractlight.moc"
