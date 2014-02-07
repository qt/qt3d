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
#include "qglmaterialcollection.h"

class tst_QGLMaterialCollection : public QObject
{
    Q_OBJECT
public:
    tst_QGLMaterialCollection() : destroyCount(0) {}
    ~tst_QGLMaterialCollection() {}

private slots:
    void simple();
    void addMaterial();
    void ownership();
    void markUsed();

public slots:
    void materialDestroyed() { ++destroyCount; }

private:
    int destroyCount;
};

// Simple sanity check on the collection.
void tst_QGLMaterialCollection::simple()
{
    QGLMaterialCollection *coll = new QGLMaterialCollection();
    QVERIFY(coll->isEmpty());
    QCOMPARE(coll->size(), 0);
    QVERIFY(!coll->material(0));
    QCOMPARE(coll->indexOf(QLatin1String("foo")), -1);

    destroyCount = 0;

    QGLMaterial *mat = new QGLMaterial(this);
    mat->setObjectName(QLatin1String("foo"));
    connect(mat, SIGNAL(destroyed()), this, SLOT(materialDestroyed()));
    QCOMPARE(coll->addMaterial(mat), 0);
    QVERIFY(mat->parent() == coll); // Ownership transfers to collection.

    QVERIFY(!coll->isEmpty());
    QCOMPARE(coll->size(), 1);
    QVERIFY(coll->material(0) == mat);
    QCOMPARE(coll->indexOf(QLatin1String("foo")), 0);
    QCOMPARE(coll->indexOf(QLatin1String("bar")), -1);

    QCOMPARE(destroyCount, 0);
    delete coll;
    QCOMPARE(destroyCount, 1);  // Check that the material was destroyed.
}

// Test adding lots of materials and then looking them up.
// Also acts as a torture test for the underlying data structures.
void tst_QGLMaterialCollection::addMaterial()
{
    const int Size = 2048;
    {
        QScopedPointer<QGLMaterialCollection> coll( new QGLMaterialCollection() );
        QScopedArrayPointer<QGLMaterial*> materials( new QGLMaterial* [Size] );
        for (int index = 0; index < Size; ++index) {
            QGLMaterial *mat = new QGLMaterial();
            mat->setObjectName(QString::number(index));
            connect(mat, SIGNAL(destroyed()), this, SLOT(materialDestroyed()));
            materials[index] = mat;
            QCOMPARE(coll->addMaterial(mat), index);
        }
        QCOMPARE(coll->size(), Size);
        QVERIFY(!coll->isEmpty());

        for (int index = 0; index < Size; ++index) {
            QString name = QString::number(index);
            QVERIFY(coll->material(index) == materials[index]);
            QVERIFY(coll->material(name) == materials[index]);
            QVERIFY(coll->contains(materials[index]));
            QVERIFY(coll->contains(name));
            QCOMPARE(coll->indexOf(materials[index]), index);
            QCOMPARE(coll->indexOf(name), index);
            QCOMPARE(coll->materialName(index), name);
        }

        QGLMaterial *mat = new QGLMaterial();

        QVERIFY(!coll->material(-1));
        QVERIFY(!coll->material(coll->size()));
        QVERIFY(!coll->contains(0));
        QVERIFY(!coll->contains(mat));
        QVERIFY(!coll->contains(QLatin1String("foo")));
        QCOMPARE(coll->indexOf(0), -1);
        QCOMPARE(coll->indexOf(mat), -1);
        QCOMPARE(coll->indexOf(QLatin1String("foo")), -1);

        delete mat;

        destroyCount = 0;
        // coll goes out of scope here; all materials will be destroyed.
    }
    QCOMPARE(destroyCount, Size);
}

// Test collection ownership.
void tst_QGLMaterialCollection::ownership()
{
    QGLMaterialCollection coll;

    QGLMaterial *mat = new QGLMaterial(this);
    mat->setObjectName(QLatin1String("foo"));

    QGLMaterial *mat2 = new QGLMaterial(this);
    mat2->setObjectName(QLatin1String("bar"));

    // Adding the same material multiple times returns the same index.
    QCOMPARE(coll.addMaterial(mat), 0);
    QCOMPARE(coll.addMaterial(mat2), 1);
    QCOMPARE(coll.addMaterial(mat), 0);
    QVERIFY(coll.material(0) == mat);
    QVERIFY(coll.material(1) == mat2);
    QCOMPARE(coll.indexOf(QLatin1String("foo")), 0);
    QCOMPARE(coll.indexOf(QLatin1String("bar")), 1);
    QCOMPARE(coll.size(), 2);

    // Shouldn't be able to add to another collection yet.
    QGLMaterialCollection coll2;
    QCOMPARE(coll2.addMaterial(mat), -1);
    QVERIFY(coll2.isEmpty());

    // Remove a material from the first collection and add it to the second.
    QVERIFY(mat->parent() == &coll);
    coll.removeMaterial(mat);
    QVERIFY(!mat->parent());    // Parent should revert to null when removed.
    QCOMPARE(coll.size(), 2);   // Still two elements, one is null.
    QVERIFY(coll.material(0) == 0);
    QVERIFY(coll.material(1) == mat2);
    QCOMPARE(coll.indexOf(QLatin1String("foo")), -1);
    QCOMPARE(coll.indexOf(QLatin1String("bar")), 1);
    QCOMPARE(coll2.addMaterial(mat), 0);
    QVERIFY(mat->parent() == &coll2);
    QCOMPARE(coll2.size(), 1);
    QVERIFY(coll2.material(0) == mat);
    QCOMPARE(coll2.indexOf(QLatin1String("foo")), 0);
}

// Test material marking.
void tst_QGLMaterialCollection::markUsed()
{
    QGLMaterialCollection coll;

    QGLMaterial *mat = new QGLMaterial(this);
    mat->setObjectName(QLatin1String("foo"));
    connect(mat, SIGNAL(destroyed()), this, SLOT(materialDestroyed()));

    QGLMaterial *mat2 = new QGLMaterial(this);
    mat2->setObjectName(QLatin1String("bar"));
    connect(mat2, SIGNAL(destroyed()), this, SLOT(materialDestroyed()));

    QGLMaterial *mat3 = new QGLMaterial(this);
    mat3->setObjectName(QLatin1String("baz"));
    connect(mat3, SIGNAL(destroyed()), this, SLOT(materialDestroyed()));

    coll.addMaterial(mat);
    coll.addMaterial(mat2);
    coll.addMaterial(mat3);

    QVERIFY(!coll.isMaterialUsed(0));
    QVERIFY(!coll.isMaterialUsed(1));
    QVERIFY(!coll.isMaterialUsed(2));

    coll.markMaterialAsUsed(1);
    QVERIFY(!coll.isMaterialUsed(0));
    QVERIFY(coll.isMaterialUsed(1));
    QVERIFY(!coll.isMaterialUsed(2));

    QVERIFY(coll.material(0) == mat);
    QVERIFY(coll.material(1) == mat2);
    QVERIFY(coll.material(2) == mat3);

    destroyCount = 0;
    coll.removeUnusedMaterials();
    QCOMPARE(destroyCount, 2);

    QVERIFY(coll.material(0) == 0);
    QVERIFY(coll.material(1) == mat2);
    QVERIFY(coll.material(2) == 0);

    QVERIFY(!coll.contains(QLatin1String("foo")));
    QVERIFY(coll.contains(QLatin1String("bar")));
    QVERIFY(!coll.contains(QLatin1String("baz")));

    // Bounds checks.
    QVERIFY(!coll.isMaterialUsed(-1));
    QVERIFY(!coll.isMaterialUsed(3));
}

QTEST_APPLESS_MAIN(tst_QGLMaterialCollection)

#include "tst_qglmaterialcollection.moc"
