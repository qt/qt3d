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

#include <Qt3DRender/qabstractlight.h>
#include <Qt3DRender/private/qabstractlight_p.h>
#include <Qt3DRender/qpointlight.h>
#include <Qt3DRender/qdirectionallight.h>
#include <Qt3DRender/qspotlight.h>
#include <Qt3DRender/private/qpointlight_p.h>
#include <Qt3DRender/private/qdirectionallight_p.h>
#include <Qt3DRender/private/qspotlight_p.h>

#include "testarbiter.h"

class DummyLight : public Qt3DRender::QAbstractLight
{
    Q_OBJECT

public:
    explicit DummyLight(Qt3DCore::QNode *parent = nullptr)
        : QAbstractLight(*new Qt3DRender::QAbstractLightPrivate(QAbstractLight::PointLight), parent)
    {}
};


// We need to call QNode::clone which is protected
// So we sublcass QNode instead of QObject
class tst_QAbstractLight: public Qt3DCore::QNode
{
    Q_OBJECT

private Q_SLOTS:
    void checkLightPropertyUpdates()
    {
        TestArbiter arbiter;
        QScopedPointer<Qt3DRender::QAbstractLight> light(new DummyLight);
        arbiter.setArbiterOnNode(light.data());

        light->setColor(Qt::red);
        light->setIntensity(0.8f); // change from the default of 0.5f

        QCOMPARE(arbiter.dirtyNodes().size(), 2);
        QVERIFY(arbiter.dirtyNodes()[0]->metaObject()->inherits(&Qt3DRender::QShaderData::staticMetaObject));
        QCOMPARE(arbiter.dirtyNodes()[1], light.data());

        arbiter.clear();
    }

    void checkPointLightPropertyUpdates()
    {
        TestArbiter arbiter;
        QScopedPointer<Qt3DRender::QPointLight> pointLight(new Qt3DRender::QPointLight);
        arbiter.setArbiterOnNode(pointLight.data());

        pointLight->setColor(Qt::green);
        pointLight->setIntensity(0.8f);
        pointLight->setConstantAttenuation(0.5f);
        pointLight->setLinearAttenuation(0.0f);     // No actual event triggered as 0.0f is default
        pointLight->setQuadraticAttenuation(1.0f);

        QCOMPARE(arbiter.dirtyNodes().size(), 2);
        QVERIFY(arbiter.dirtyNodes()[0]->metaObject()->inherits(&Qt3DRender::QShaderData::staticMetaObject));
        QCOMPARE(arbiter.dirtyNodes()[1], pointLight.data());

        arbiter.clear();
    }

    void checkDirectionalLightPropertyUpdates()
    {
        TestArbiter arbiter;
        QScopedPointer<Qt3DRender::QDirectionalLight> dirLight(new Qt3DRender::QDirectionalLight);
        arbiter.setArbiterOnNode(dirLight.data());

        dirLight->setColor(Qt::blue);
        dirLight->setIntensity(0.8f);
        dirLight->setWorldDirection(QVector3D(0.5f, 0.0f, -1.0f));

        QCOMPARE(arbiter.dirtyNodes().size(), 2);
        QVERIFY(arbiter.dirtyNodes()[0]->metaObject()->inherits(&Qt3DRender::QShaderData::staticMetaObject));
        QCOMPARE(arbiter.dirtyNodes()[1], dirLight.data());

        arbiter.clear();
    }

    void checkSpotLightPropertyUpdates()
    {
        TestArbiter arbiter;
        QScopedPointer<Qt3DRender::QSpotLight> spotLight(new Qt3DRender::QSpotLight);
        arbiter.setArbiterOnNode(spotLight.data());

        spotLight->setColor(Qt::lightGray);
        spotLight->setIntensity(0.8f);
        spotLight->setLocalDirection(QVector3D(0.5f, 0.0f, -1.0f));
        spotLight->setCutOffAngle(0.75f);

        QCOMPARE(arbiter.dirtyNodes().size(), 2);
        QVERIFY(arbiter.dirtyNodes()[0]->metaObject()->inherits(&Qt3DRender::QShaderData::staticMetaObject));
        QCOMPARE(arbiter.dirtyNodes()[1], spotLight.data());

        arbiter.clear();
    }
};

QTEST_MAIN(tst_QAbstractLight)

#include "tst_qabstractlight.moc"
