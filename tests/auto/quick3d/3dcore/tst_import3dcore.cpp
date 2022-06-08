// Copyright (C) 2016 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0


#include <QtTest/QTest>
#include <QObject>
#include <qmlscenereader.h>

class tst_import3dcore : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkQMLImports()
    {
        // GIVEN
        QmlSceneReader sceneReader(QUrl("qrc:/3dcore.qml"));

        // THEN
        QVERIFY(sceneReader.root() != nullptr);
    }
};

QTEST_MAIN(tst_import3dcore)

#include "tst_import3dcore.moc"
