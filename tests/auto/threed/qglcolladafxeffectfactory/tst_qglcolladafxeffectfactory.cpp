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
#include "qglcolladafxeffectfactory.h"

#include <QXmlStreamReader>
#include <QColor>
#include <QOpenGLBuffer>

QT_BEGIN_NAMESPACE
class QGLColladaFxEffect;
QT_END_NAMESPACE

Q_DECLARE_METATYPE(QGLColladaFxEffect*)

static void cleanupEffectList(QList<QGLColladaFxEffect*> &effects)
{
    while (effects.count())
    {
        delete effects.back();
        effects.pop_back();
    }
}



// Inherit from QGLColladaFxEffectFactory to get access to it's innards
class tst_QGLColladaFxEffectFactory : public QObject, QGLColladaFxEffectFactory
{
    Q_OBJECT
public:
    tst_QGLColladaFxEffectFactory() {}
    ~tst_QGLColladaFxEffectFactory() {}

private slots:
    void create_data();
    void create();
    void processFloatList_data();
    void processFloatList();
    void processFloatListArray_data();
    void processFloatListArray();
    void processColorElement_data();
    void processColorElement();
    void processLibraryImagesElement();
    void loadEffectsFromFile();
    void exportImportEffect_data();
    void exportImportEffect();
};



void tst_QGLColladaFxEffectFactory::create_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<int>("effectCount");

    QTest::newRow("null") << "" << 0;
    QTest::newRow("nonexistent file") << "nonexistentfile.dae" << 0;
    QTest::newRow("collada cube example") << ":/collada_cube.xml" << 1;
}



void tst_QGLColladaFxEffectFactory::create()
{
    QFETCH(QString, fileName);
    QFETCH(int, effectCount);

    QList<QGLColladaFxEffect*> effects = QGLColladaFxEffectFactory::loadEffectsFromFile(fileName);
    QCOMPARE(effects.count(), effectCount);
    cleanupEffectList(effects);
}



void tst_QGLColladaFxEffectFactory::processFloatList_data()
{
    QTest::addColumn<QString>("xmlString");
    QTest::addColumn<QVariant >("expectedResult");

    QTest::newRow("null") << "" << QVariant();

    QVariant expectedSingleResult = 0.456f;
    QTest::newRow("single_float") << "<floats>0.456</floats>" << expectedSingleResult;

    QVariant expectedPrecisionResult = 1.234567f;
    QTest::newRow("seven digit accuracy") << "<floats>1.234567</floats>" << expectedPrecisionResult;

    QVariant expected3Dresult = QVector3D(0.0f, 0.3f, 6.6f);
    QTest::newRow("3 floats to vector 3D") << "<floats>0.0 0.3 6.6</floats>" << expected3Dresult;

    QVariant expected4DResult = QVector4D(0.0f, 1.1f, 2.2f, 3.3f);
    QTest::newRow("4 floats to vector 4D") << "<floats>0.0 1.1 2.2 3.3</floats>" << expected4DResult;

    QVariant expectedColorResult = QVector4D(0.0f, 1.1f, 2.2f, 3.3f);
    QTest::newRow("4 float color to vector 4D") << "<color>0.0 1.1 2.2 3.3</color>" << expectedColorResult;
}



void tst_QGLColladaFxEffectFactory::processFloatList()
{
    QFETCH(QString, xmlString);
    QFETCH(QVariant, expectedResult);

    QXmlStreamReader xml(xmlString);

    xml.readNext(); // startDocument
    xml.readNext();

    QVariant result = QGLColladaFxEffectFactory::processFloatList( xml );

    QCOMPARE(result, expectedResult);
}

void tst_QGLColladaFxEffectFactory::processFloatListArray_data()
{
    QTest::addColumn<QString>("xmlString");
    QTest::addColumn<QArray<float> >("expectedResult");

    QArray<float> expectedSevenDigitArray;
    expectedSevenDigitArray << 1234567.0f << 1.234567f << 76.54321f << 1111.111f << 22222.22f << 7645213.0f;
    QTest::newRow("seven_digits") << "<floats>1234567.0 1.234567 76.54321 1111.111 22222.22 7645213.0</floats>" << expectedSevenDigitArray;

}

void tst_QGLColladaFxEffectFactory::processFloatListArray()
{
    QFETCH(QString, xmlString);
    QFETCH(QArray<float>, expectedResult);

    QXmlStreamReader xml(xmlString);

    xml.readNext(); // startDocument
    xml.readNext();

    QArray<float> result = QGLColladaFxEffectFactory::processFloatList( xml ).value<QArray<float> >();

    QCOMPARE(result, expectedResult);

}

void tst_QGLColladaFxEffectFactory::processColorElement_data()
{
    QTest::addColumn<QString>("xmlString");
    QTest::addColumn<QColor>("expectedResult");

    QColor errorColor = QColor(0, 0, 0, 255);
    QTest::newRow("null") << "" << errorColor;

    QColor expectedColor3dResult = QColor::fromRgbF(0.0f, 0.4f, 0.8f);
    QTest::newRow("color 3 floats") << "<color>0.0 0.4 0.8</color>" << expectedColor3dResult;

    QColor expectedColor4dResult = QColor::fromRgbF(0.0f, 0.2f, 0.4f, 0.8f);
    QTest::newRow("color 4 floats") << "<color>0.0 0.2 0.4 0.8</color>" << expectedColor4dResult;

    QTest::newRow("malformed color 1 float") << "<color>0.5</color>" << errorColor;
    QTest::newRow("malformed color 2 float") << "<color>0.1 0.7</color>" << errorColor;
    QTest::newRow("malformed color 5 float") << "<color>0.1 0.2 0.3 0.4 0.5</color>" << errorColor;
}



void tst_QGLColladaFxEffectFactory::processColorElement()
{
    QFETCH(QString, xmlString);
    QFETCH(QColor, expectedResult);

    QXmlStreamReader xml(xmlString);

    xml.readNext(); // startDocument
    xml.readNext();

    QColor result = QGLColladaFxEffectFactory::processColorElement( xml );
    QCOMPARE(result, expectedResult);
}


void tst_QGLColladaFxEffectFactory::processLibraryImagesElement()
{
    // Example from Collada Fx 1.4 (escapes and ':' character added):
    QXmlStreamReader xml("<library_images>\
                         <image name=\"Rose\">\
                                     <init_from>\
                                         :../flowers/rose01.jpg\
                                         </init_from>\
                                         </image>\
                                         </library_images>");

    xml.readNext(); // startDocument
    xml.readNext();

    ResultState state;
    QGLColladaFxEffectFactory::processLibraryImagesElement( xml, &state );

    // The structure is just an implementation detail, but the param and image
    // should be well formed:
    QVariant param = state.paramNames.values().at(0);
    QCOMPARE(param.type(), QVariant::Image);
    QCOMPARE(state.paramNames.value(QLatin1String("Rose")), param);
    QImage image = param.value<QImage>();
    QCOMPARE(image.size(), QSize(50,75));
}

void tst_QGLColladaFxEffectFactory::loadEffectsFromFile()
{
    QList<QGLColladaFxEffect*> cubeResult =
            QGLColladaFxEffectFactory::loadEffectsFromFile(QLatin1String(":/collada_cube.xml"));

    QCOMPARE(cubeResult.count(), 1);

    QGLColladaFxEffect* cubeEffect = cubeResult.at(0);

    QEXPECT_FAIL("", "Missing functionality", Continue);
    QCOMPARE(cubeEffect->id(), QLatin1String("whitePhong"));

    QCOMPARE(cubeEffect->sid(), QLatin1String("phong1"));
    QVERIFY2(cubeEffect->material()->emittedLight() ==
             QColor::fromRgbF(0.0f, 0.0f, 0.3f, 1.0f), "Emission color doesn't match");
    QVERIFY2(cubeEffect->material()->ambientColor() ==
             QColor::fromRgbF(1.0f, 0.0f, 0.0f, 1.0f), "Ambient color doesn't match");
    QVERIFY2(cubeEffect->material()->diffuseColor() == QColor::fromRgbF(0.0f, 1.0f, 0.0f, 1.0f),
             "Diffuse color doesn't match");
    QVERIFY2(cubeEffect->material()->specularColor() ==
             QColor::fromRgbF(1.0f, 1.0f, 1.0f, 1.0f),
             "Specular color doesn't match");
    QVERIFY2(cubeEffect->material()->shininess() == 20, "Shininess doesn't match");
}

void tst_QGLColladaFxEffectFactory::exportImportEffect_data()
{
    QTest::addColumn<QGLColladaFxEffect*>("effect");
    QTest::addColumn<QString>("effectId");
    QTest::addColumn<QString>("techniqueSid");

    QGLColladaFxEffect* effect = new QGLColladaFxEffect;
    QTest::newRow("empty effect") << effect << QString::fromLatin1("EmptyEffect") << QString::fromLatin1("EmptyTechnique");

    effect = new QGLColladaFxEffect;
    effect->setFragmentShader("test fragment shader");
    effect->setVertexShader("Test vertex shader");
    QGLMaterial* material = new QGLMaterial;
    material->setAmbientColor(QColor(1,2,3));
    material->setDiffuseColor(QColor(2,3,4));
    material->setEmittedLight(QColor(255,255,255));
    material->setShininess(129);
    material->setSpecularColor(QColor(3,4,5));
    effect->setMaterial(material);

    QTest::newRow("Test Effect") << effect << QString::fromLatin1("TestEffect") << QString::fromLatin1("TestTechnique");
}

void tst_QGLColladaFxEffectFactory::exportImportEffect()
{
    QFETCH(QGLColladaFxEffect*, effect);
    QFETCH(QString, effectId);
    QFETCH(QString, techniqueSid);
    QString colladaEffectString = QGLColladaFxEffectFactory::exportEffect(effect, effectId, techniqueSid);
    QXmlStreamReader xml(colladaEffectString);

    QList<QGLColladaFxEffect*> importedEffects = QGLColladaFxEffectFactory::loadEffectsFromXml(xml);
    QCOMPARE(importedEffects.count(), 1);

    QGLColladaFxEffect* importedEffect = importedEffects[0];
    QCOMPARE(effect->fragmentShader(), importedEffect->fragmentShader());
    QCOMPARE(effect->vertexShader(), importedEffect->vertexShader());
    QEXPECT_FAIL("Test Effect", "QGLColladaFxEffect import/export does not respect materials", Continue);
    QCOMPARE(effect->material(), importedEffect->material());
    QCOMPARE(effect->supportsPicking(), importedEffect->supportsPicking());
}

QTEST_APPLESS_MAIN(tst_QGLColladaFxEffectFactory)

#include "tst_qglcolladafxeffectfactory.moc"
