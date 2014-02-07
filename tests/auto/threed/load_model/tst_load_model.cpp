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

#include "qglabstractscene.h"

class tst_LoadModel : public QObject
{
    Q_OBJECT
public:
    tst_LoadModel() {}
    ~tst_LoadModel() {}

private slots:
    void load_data();
    void load();
    void create_data();
    void create();
};

// Just try to load all the models in the resources file - no checking
void tst_LoadModel::load_data()
{
    QString model_path(QLatin1String(":/data/models"));
    QDir data(model_path);

    // only load if its a supported format
    QStringList entries = data.entryList(QGLAbstractScene::supportedFormats());

    QTest::addColumn<QString>("model");

    for (int i = 0; i < entries.size(); ++i)
        QTest::newRow(qPrintable(entries.at(i))) << entries.at(i);
}

void tst_LoadModel::load()
{
    QFETCH(QString, model);

    QGLAbstractScene *scene = 0;
    QString model_path(QLatin1String(":/data/models/%1"));
    model_path = model_path.arg(model);

    QFile f(model_path);
    QVERIFY(f.open(QIODevice::ReadOnly));

    QUrl u;
    u.setPath(model_path);
    u.setScheme("file");

    scene = QGLAbstractScene::loadScene((&f), u);
    QVERIFY(scene != 0);
}

void tst_LoadModel::create_data()
{
    QTest::addColumn<QString>("model");
    QTest::addColumn<QString>("options");
    QTest::addColumn<QString>("importName");
    QTest::addColumn<QString>("nodeName");
    QTest::addColumn<QString>("materialName");
    QTest::addColumn<QRgb>("materialDiffuse");
    QTest::addColumn<QString>("texture");
    QTest::addColumn<int>("expected_vertices");
    QTest::addColumn<int>("expected_indices");

    ////// --- OBJ ---

    // default is faceted for obj
    QTest::newRow("cube-obj-default")
            << "basic-cube.obj" << ""
            << "basic-cube.obj" << "CubeObject_CubeMesh" << "Red"
            << qRgb(188, 32, 32) << "tex"
            << 24 << 36;

    // force a faceted load
    QTest::newRow("cube-obj-faceted")
            << "basic-cube.obj" << "ForceFaceted"
            << "basic-cube.obj" << "CubeObject_CubeMesh" << "Red"
            << qRgb(188, 32, 32) << "tex"
            << 24 << 36;

    // force a smooth load
    QTest::newRow("cube-obj-smooth")
            << "basic-cube.obj" << "ForceSmooth"
            << "basic-cube.obj" << "CubeObject_CubeMesh" << "Red"
            << qRgb(188, 32, 32) << "tex"
            << 24 << 36;


    ////// --- 3DS ---

    // default is faceted
    QTest::newRow("cube-3ds-default")
            << "basic-cube.3ds" << ""
            << "basic-cube.3ds" << "CubeObject::SlateGray" << "SlateGray"
            << qRgb(94, 142, 155) << "tex"
            << 12 << 36;

    // force smooth
    QTest::newRow("cube-3ds-smooth")
            << "basic-cube.3ds" << "ForceSmooth"
            << "basic-cube.3ds" << "CubeObject::Red" << "Red"
            << qRgb(234, 40, 40) << "tex"
            << 12 << 36;


    ////// --- wave model ---

    // default is faceted
    QTest::newRow("wave-obj-default")
            << "wave.obj" << ""
               << "wave.obj" << "Wave_Obj" << ""
                  << qRgb(0, 0, 0) << ""
                     << 259 << 1296;

    // force smooth
    QTest::newRow("wave-obj-faceted")
            << "wave.obj" << "ForceFaceted"
               << "wave.obj" << "Wave_Obj" << ""
                  << qRgb(0, 0, 0) << ""
                     << 864 << 1296;
}

void tst_LoadModel::create()
{
    QFETCH(QString, model);
    QFETCH(QString, options);
    QFETCH(QString, importName);
    QFETCH(QString, nodeName);
    QFETCH(QString, materialName);
    QFETCH(QRgb, materialDiffuse);
    QFETCH(QString, texture);
    QFETCH(int, expected_vertices);
    QFETCH(int, expected_indices);

//    if (model == QLatin1String("wave.obj"))
//    {
//        QSKIP("Fails on Qt5");
//    }

    QGLAbstractScene *scene = 0;
    QString model_path(QLatin1String(":/data/models/%1"));
    model_path = model_path.arg(model);

    QFile f(model_path);
    QVERIFY(f.open(QIODevice::ReadOnly));

    QUrl u;
    u.setPath(model_path);
    u.setScheme("file");

    scene = QGLAbstractScene::loadScene(&f, u, QString(), options);
    QVERIFY(scene != 0);

    QGLSceneNode *node = scene->mainNode();

    //qDumpScene(node);

    QCOMPARE(node->objectName(), importName);

    QGLSceneNode *foundGeometry = 0;
    QGLSceneNode *foundNode = nodeName.isEmpty() ? (QGLSceneNode *)1 : 0;
    QGLSceneNode *foundMaterial = materialName.isEmpty() ? (QGLSceneNode *)1 : 0;
    QList<QGLSceneNode*> list = node->allChildren();
    for (int i = 0; i < list.size(); ++ i)
    {
        if (!foundGeometry && list.at(i)->geometry().count() > 0)
            foundGeometry = list.at(i);
        if (!foundNode && list.at(i)->objectName() == nodeName)
            foundNode = list.at(i);
        if (!foundMaterial && list.at(i)->material() &&
                (list.at(i)->material()->objectName() == materialName))
            foundMaterial = list.at(i);
    }

    QVERIFY2(foundGeometry != 0, "A non-null geometry node was not found");
    QVERIFY2(foundNode != 0, qPrintable(nodeName + QLatin1String(" named node not found")));
    QVERIFY2(foundMaterial != 0, qPrintable(materialName + QLatin1String(" named material not found")));

    int r, g, b;
    if (!materialName.isEmpty())
    {
        Q_ASSERT(foundMaterial);
        foundMaterial->material()->diffuseColor().getRgb(&r, &g, &b);
        QCOMPARE(materialDiffuse, qRgb(r, g, b));
    }

    Q_ASSERT(foundGeometry);
    QGeometryData data = foundGeometry->geometry();
    QVERIFY(data.count() > 0);

    QCOMPARE(data.vertices().count(), expected_vertices);
    QCOMPARE(data.normals().count(), expected_vertices);
    QCOMPARE(data.indices().count(), expected_indices);
}

QTEST_APPLESS_MAIN(tst_LoadModel)

#include "tst_load_model.moc"
