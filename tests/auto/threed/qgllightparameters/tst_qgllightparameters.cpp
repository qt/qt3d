/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include "qgllightparameters.h"
#include <QtGui/qmatrix4x4.h>

class tst_QGLLightParameters : public QObject
{
    Q_OBJECT
public:
    tst_QGLLightParameters() {}
    ~tst_QGLLightParameters() {}

private slots:
    void create();
    void modify();
    void type();
    void transform();
};

void tst_QGLLightParameters::create()
{
    // Test that a newly created object has the correct defaults.
    QGLLightParameters params;
    QVERIFY(params.type() == QGLLightParameters::Directional);
    QVERIFY(params.position() == QVector3D(0, 0, 0));
    QVERIFY(params.direction() == QVector3D(0, 0, 1));
    QVERIFY(params.ambientColor() == QColor(0, 0, 0, 255));
    QVERIFY(params.diffuseColor() == QColor(255, 255, 255, 255));
    QVERIFY(params.specularColor() == QColor(255, 255, 255, 255));
    QVERIFY(params.spotDirection() == QVector3D(0, 0, -1));
    QCOMPARE(params.spotExponent(), 0.0f);
    QCOMPARE(params.spotAngle(), 180.0f);
    QCOMPARE(params.spotCosAngle(), -1.0f);
    QCOMPARE(params.constantAttenuation(), 1.0f);
    QCOMPARE(params.linearAttenuation(), 0.0f);
    QCOMPARE(params.quadraticAttenuation(), 0.0f);
}

void tst_QGLLightParameters::modify()
{
    // Test modifying each field individually.
    QGLLightParameters params;
    QSignalSpy positionSpy(&params, SIGNAL(positionChanged()));
    QSignalSpy directionSpy(&params, SIGNAL(directionChanged()));
    QSignalSpy ambientSpy(&params, SIGNAL(ambientColorChanged()));
    QSignalSpy diffuseSpy(&params, SIGNAL(diffuseColorChanged()));
    QSignalSpy specularSpy(&params, SIGNAL(specularColorChanged()));
    QSignalSpy spotDirectionSpy(&params, SIGNAL(spotDirectionChanged()));
    QSignalSpy spotExponentSpy(&params, SIGNAL(spotExponentChanged()));
    QSignalSpy spotAngleSpy(&params, SIGNAL(spotAngleChanged()));
    QSignalSpy constSpy(&params, SIGNAL(constantAttenuationChanged()));
    QSignalSpy linearSpy(&params, SIGNAL(linearAttenuationChanged()));
    QSignalSpy quadSpy(&params, SIGNAL(quadraticAttenuationChanged()));
    QSignalSpy lightSpy(&params, SIGNAL(lightChanged()));

    params.setPosition(QVector3D(1, 2, -3));
    QVERIFY(params.position() == QVector3D(1.0f, 2.0f, -3.0f));
    QVERIFY(params.direction() == QVector3D(0, 0, 0));
    QVERIFY(params.type() == QGLLightParameters::Positional);
    QCOMPARE(positionSpy.size(), 1);
    QCOMPARE(directionSpy.size(), 1);
    QCOMPARE(lightSpy.size(), 1);

    params.setDirection(QVector3D(-1, -2, 3));
    QVERIFY(params.position() == QVector3D(0, 0, 0));
    QVERIFY(params.direction() == QVector3D(-1.0f, -2.0f, 3.0f));
    QVERIFY(params.type() == QGLLightParameters::Directional);
    QCOMPARE(positionSpy.size(), 2);
    QCOMPARE(directionSpy.size(), 2);
    QCOMPARE(lightSpy.size(), 2);

    params.setAmbientColor(Qt::red);
    QCOMPARE(params.ambientColor().red(), 255);
    QCOMPARE(params.ambientColor().green(), 0);
    QCOMPARE(params.ambientColor().blue(), 0);
    QCOMPARE(params.ambientColor().alpha(), 255);
    QCOMPARE(ambientSpy.size(), 1);
    QCOMPARE(lightSpy.size(), 3);

    params.setDiffuseColor(Qt::green);
    QCOMPARE(params.diffuseColor().red(), 0);
    QCOMPARE(params.diffuseColor().green(), 255);
    QCOMPARE(params.diffuseColor().blue(), 0);
    QCOMPARE(params.diffuseColor().alpha(), 255);
    QCOMPARE(diffuseSpy.size(), 1);
    QCOMPARE(lightSpy.size(), 4);

    params.setSpecularColor(Qt::black);
    QCOMPARE(params.specularColor().red(), 0);
    QCOMPARE(params.specularColor().green(), 0);
    QCOMPARE(params.specularColor().blue(), 0);
    QCOMPARE(params.specularColor().alpha(), 255);
    QCOMPARE(specularSpy.size(), 1);
    QCOMPARE(lightSpy.size(), 5);

    params.setSpotDirection(QVector3D(0, 1, 0));
    QVERIFY(params.spotDirection() == QVector3D(0.0f, 1.0f, 0.0f));
    QCOMPARE(spotDirectionSpy.size(), 1);
    QCOMPARE(lightSpy.size(), 6);

    params.setSpotExponent(23.5f);
    QCOMPARE(params.spotExponent(), 23.5f);
    QCOMPARE(spotExponentSpy.size(), 1);
    QCOMPARE(lightSpy.size(), 7);

    params.setSpotAngle(90.0f);
    QCOMPARE(params.spotAngle(), 90.0f);
    QVERIFY(params.spotCosAngle() <= 0.000001f);    // Fuzzy compare is not fuzzy enough!
    QCOMPARE(spotAngleSpy.size(), 1);
    QCOMPARE(lightSpy.size(), 8);

    params.setSpotAngle(180.0f);
    QCOMPARE(params.spotCosAngle(), -1.0f);
    QCOMPARE(spotAngleSpy.size(), 2);
    QCOMPARE(lightSpy.size(), 9);

    params.setConstantAttenuation(16.0f);
    QCOMPARE(params.constantAttenuation(), 16.0f);
    QCOMPARE(constSpy.size(), 1);
    QCOMPARE(lightSpy.size(), 10);

    params.setLinearAttenuation(-3.5f);
    QCOMPARE(params.linearAttenuation(), -3.5f);
    QCOMPARE(linearSpy.size(), 1);
    QCOMPARE(lightSpy.size(), 11);

    params.setQuadraticAttenuation(4.0f);
    QCOMPARE(params.quadraticAttenuation(), 4.0f);
    QCOMPARE(quadSpy.size(), 1);
    QCOMPARE(lightSpy.size(), 12);

    // Test that we don't get any side effects between properties.
    QVERIFY(params.position() == QVector3D(0, 0, 0));
    QVERIFY(params.direction() == QVector3D(-1.0f, -2.0f, 3.0f));
    QVERIFY(params.type() == QGLLightParameters::Directional);
    QCOMPARE(params.ambientColor().red(), 255);
    QCOMPARE(params.ambientColor().green(), 0);
    QCOMPARE(params.ambientColor().blue(), 0);
    QCOMPARE(params.ambientColor().alpha(), 255);
    QCOMPARE(params.diffuseColor().red(), 0);
    QCOMPARE(params.diffuseColor().green(), 255);
    QCOMPARE(params.diffuseColor().blue(), 0);
    QCOMPARE(params.diffuseColor().alpha(), 255);
    QCOMPARE(params.specularColor().red(), 0);
    QCOMPARE(params.specularColor().green(), 0);
    QCOMPARE(params.specularColor().blue(), 0);
    QCOMPARE(params.specularColor().alpha(), 255);
    QVERIFY(params.spotDirection() == QVector3D(0.0f, 1.0f, 0.0f));
    QCOMPARE(params.spotExponent(), 23.5f);
    QCOMPARE(params.spotAngle(), 180.0f);
    QCOMPARE(params.spotCosAngle(), -1.0f);
    QCOMPARE(params.constantAttenuation(), 16.0f);
    QCOMPARE(params.linearAttenuation(), -3.5f);
    QCOMPARE(params.quadraticAttenuation(), 4.0f);

    // Set the properties to same values and check for no further signals.
    params.setDirection(QVector3D(-1, -2, 3));
    params.setAmbientColor(Qt::red);
    params.setDiffuseColor(Qt::green);
    params.setSpecularColor(Qt::black);
    params.setSpotDirection(QVector3D(0, 1, 0));
    params.setSpotExponent(23.5f);
    params.setSpotAngle(180.0f);
    params.setConstantAttenuation(16.0f);
    params.setLinearAttenuation(-3.5f);
    params.setQuadraticAttenuation(4.0f);
    QCOMPARE(positionSpy.size(), 2);
    QCOMPARE(directionSpy.size(), 2);
    QCOMPARE(ambientSpy.size(), 1);
    QCOMPARE(diffuseSpy.size(), 1);
    QCOMPARE(specularSpy.size(), 1);
    QCOMPARE(spotDirectionSpy.size(), 1);
    QCOMPARE(spotExponentSpy.size(), 1);
    QCOMPARE(spotAngleSpy.size(), 2);
    QCOMPARE(constSpy.size(), 1);
    QCOMPARE(linearSpy.size(), 1);
    QCOMPARE(quadSpy.size(), 1);
    QCOMPARE(lightSpy.size(), 12);

    // Check signal emission when changing from positional to positional.
    params.setPosition(QVector3D(1, 2, -3));
    QCOMPARE(positionSpy.size(), 3);
    QCOMPARE(directionSpy.size(), 3);
    QCOMPARE(lightSpy.size(), 13);
    params.setPosition(QVector3D(1, 2, -3));
    QVERIFY(params.position() == QVector3D(1.0f, 2.0f, -3.0f));
    QVERIFY(params.direction() == QVector3D(0, 0, 0));
    QCOMPARE(positionSpy.size(), 3);
    QCOMPARE(directionSpy.size(), 3);
    QCOMPARE(lightSpy.size(), 13);
    params.setPosition(QVector3D(4, 1, -2));
    QCOMPARE(positionSpy.size(), 4);
    QCOMPARE(directionSpy.size(), 3);
    QCOMPARE(lightSpy.size(), 14);
    QVERIFY(params.position() == QVector3D(4.0f, 1.0f, -2.0f));
    QVERIFY(params.direction() == QVector3D(0, 0, 0));
}

void tst_QGLLightParameters::type()
{
    QGLLightParameters params;
    QVERIFY(params.type() == QGLLightParameters::Directional);
    QCOMPARE(params.position(), QVector3D(0.0f, 0.0f, 0.0f));
    QCOMPARE(params.direction(), QVector3D(0.0f, 0.0f, 1.0f));
    params.setDirection(QVector3D(1.0f, 0.0f, 1.0f));
    QVERIFY(params.type() == QGLLightParameters::Directional);
    QCOMPARE(params.position(), QVector3D(0.0f, 0.0f, 0.0f));
    QCOMPARE(params.direction(), QVector3D(1.0f, 0.0f, 1.0f));
    params.setPosition(QVector3D(2.0f, 0.0f, 2.0f));
    QVERIFY(params.type() == QGLLightParameters::Positional);
    QCOMPARE(params.position(), QVector3D(2.0f, 0.0f, 2.0f));
    QCOMPARE(params.direction(), QVector3D(0.0f, 0.0f, 0.0f));
}

void tst_QGLLightParameters::transform()
{
    QGLLightParameters params;
    params.setPosition(QVector3D(1, 2, -3));
    params.setSpotDirection(QVector3D(-5, 1, 3));

    QMatrix4x4 m;
    m.translate(-5.0f, 1.5f, 0.5f);
    m.rotate(45.0f, 1.0f, 1.0f, 1.0f);
    m.scale(2.0f);

    QVERIFY(qFuzzyCompare(params.eyePosition(m), m * QVector4D(1, 2, -3, 1)));
    QVERIFY(qFuzzyCompare(params.eyeSpotDirection(m), m.mapVector(QVector3D(-5, 1, 3))));

    params.setDirection(QVector3D(-1, -2, 3));
    QVERIFY(qFuzzyCompare(params.eyePosition(m), m * QVector4D(-1, -2, 3, 0)));
}

QTEST_APPLESS_MAIN(tst_QGLLightParameters)

#include "tst_qgllightparameters.moc"
