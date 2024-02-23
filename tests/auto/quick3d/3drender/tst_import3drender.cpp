// Copyright (C) 2016 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only


#include <QtTest/QTest>
#include <QObject>
#include <qmlscenereader.h>

class tst_import3drender : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkQMLImports()
    {
        // GIVEN
        QmlSceneReader sceneReader(QUrl("qrc:/3drender.qml"));

        // THEN
        QVERIFY(sceneReader.root() != nullptr);
    }
};

QTEST_MAIN(tst_import3drender)

#include "tst_import3drender.moc"
