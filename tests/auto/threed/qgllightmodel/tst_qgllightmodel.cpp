/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: http://www.qt-project.org/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include "qgllightmodel.h"

class tst_QGLLightModel : public QObject
{
    Q_OBJECT
public:
    tst_QGLLightModel() {}
    ~tst_QGLLightModel() {}

private slots:
    void create();
    void modify();
};

void tst_QGLLightModel::create()
{
    // Test that a newly created object has the correct defaults.
    QGLLightModel model;
    QVERIFY(model.model() == QGLLightModel::OneSided);
    QVERIFY(model.colorControl() == QGLLightModel::SingleColor);
    QVERIFY(model.viewerPosition() == QGLLightModel::ViewerAtInfinity);
    QCOMPARE(model.ambientSceneColor().redF(), 0.2);
    QCOMPARE(model.ambientSceneColor().greenF(), 0.2);
    QCOMPARE(model.ambientSceneColor().blueF(), 0.2);
    QCOMPARE(model.ambientSceneColor().alphaF(), 1.0);
}

void tst_QGLLightModel::modify()
{
    // Test modifying each field individually.
    QGLLightModel model;
    QSignalSpy modelSpy(&model, SIGNAL(modelChanged()));
    QSignalSpy controlSpy(&model, SIGNAL(colorControlChanged()));
    QSignalSpy viewerSpy(&model, SIGNAL(viewerPositionChanged()));
    QSignalSpy ambientSpy(&model, SIGNAL(ambientSceneColorChanged()));
    QSignalSpy objectSpy(&model, SIGNAL(lightModelChanged()));

    model.setModel(QGLLightModel::TwoSided);
    QVERIFY(model.model() == QGLLightModel::TwoSided);
    QCOMPARE(modelSpy.size(), 1);
    QCOMPARE(objectSpy.size(), 1);

    model.setColorControl(QGLLightModel::SeparateSpecularColor);
    QVERIFY(model.colorControl() == QGLLightModel::SeparateSpecularColor);
    QCOMPARE(controlSpy.size(), 1);
    QCOMPARE(objectSpy.size(), 2);

    model.setViewerPosition(QGLLightModel::LocalViewer);
    QVERIFY(model.viewerPosition() == QGLLightModel::LocalViewer);
    QCOMPARE(viewerSpy.size(), 1);
    QCOMPARE(objectSpy.size(), 3);

    model.setAmbientSceneColor(Qt::red);
    QCOMPARE(model.ambientSceneColor().red(), 255);
    QCOMPARE(model.ambientSceneColor().green(), 0);
    QCOMPARE(model.ambientSceneColor().blue(), 0);
    QCOMPARE(model.ambientSceneColor().alpha(), 255);
    QCOMPARE(ambientSpy.size(), 1);
    QCOMPARE(objectSpy.size(), 4);

    // Test that we don't get any side effects between properties.
    QVERIFY(model.model() == QGLLightModel::TwoSided);
    QVERIFY(model.colorControl() == QGLLightModel::SeparateSpecularColor);
    QVERIFY(model.viewerPosition() == QGLLightModel::LocalViewer);

    // Set the properties to same values and check for no further signals.
    model.setModel(QGLLightModel::TwoSided);
    model.setColorControl(QGLLightModel::SeparateSpecularColor);
    model.setViewerPosition(QGLLightModel::LocalViewer);
    model.setAmbientSceneColor(Qt::red);
    QCOMPARE(modelSpy.size(), 1);
    QCOMPARE(controlSpy.size(), 1);
    QCOMPARE(viewerSpy.size(), 1);
    QCOMPARE(ambientSpy.size(), 1);
    QCOMPARE(objectSpy.size(), 4);
}

QTEST_APPLESS_MAIN(tst_QGLLightModel)

#include "tst_qgllightmodel.moc"
