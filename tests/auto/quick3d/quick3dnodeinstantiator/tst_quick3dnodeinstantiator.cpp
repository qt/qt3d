// Copyright (C) 2013 Research In Motion.
// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "../../shared/util.h"
#include "stringmodel.h"

#include <qtest.h>
#include <QSignalSpy>
#include <QDebug>

#include <Qt3DQuick/private/quick3dnodeinstantiator_p.h>

#include <QtQml/qqmlengine.h>
#include <QtQml/qqmlcomponent.h>
#include <QtQml/qqmlcontext.h>
#include <QtQml/qqmlincubator.h>

using namespace Qt3DCore::Quick;

class tst_quick3dnodeinstantiator: public QQmlDataTest
{
    Q_OBJECT

private slots:
    void createNone();
    void createSingle();
    void createMultiple();
    void createNested();
    void stringModel();
    void activeProperty();
    void intModelChange();
    void createAndRemove();
    void asynchronous_data();
    void asynchronous();
};

void tst_quick3dnodeinstantiator::createNone()
{
    QQmlEngine engine;
    QQmlComponent component(&engine, testFileUrl("createNone.qml"));
    const auto root = qobject_cast<Qt3DCore::QNode*>(component.create());
    QVERIFY(root != 0);
    const auto instantiator = root->findChild<Quick3DNodeInstantiator*>();
    QVERIFY(instantiator != 0);
    QCOMPARE(instantiator->isActive(), true);
    QCOMPARE(instantiator->count(), 0);
    QCOMPARE(instantiator->property("success").toBool(), true);
    QVERIFY(instantiator->delegate()->isReady());
}

void tst_quick3dnodeinstantiator::createSingle()
{
    QQmlEngine engine;
    QQmlComponent component(&engine, testFileUrl("createSingle.qml"));
    const auto root = qobject_cast<Qt3DCore::QNode*>(component.create());
    QVERIFY(root != 0);
    const auto instantiator = root->findChild<Quick3DNodeInstantiator*>();
    QVERIFY(instantiator != 0);
    QCOMPARE(instantiator->isActive(), true);
    QCOMPARE(instantiator->count(), 1);
    QVERIFY(instantiator->delegate()->isReady());

    QObject *object = instantiator->object();
    QVERIFY(object);
    QCOMPARE(object->parent(), root);
    QCOMPARE(object->property("success").toBool(), true);
    QCOMPARE(object->property("idx").toInt(), 0);
}

void tst_quick3dnodeinstantiator::createMultiple()
{
    QQmlEngine engine;
    QQmlComponent component(&engine, testFileUrl("createMultiple.qml"));
    const auto root = qobject_cast<Qt3DCore::QNode*>(component.create());
    QVERIFY(root != 0);
    const auto instantiator = root->findChild<Quick3DNodeInstantiator*>();
    QVERIFY(instantiator != 0);
    QCOMPARE(instantiator->isActive(), true);
    QCOMPARE(instantiator->count(), 10);

    for (int i = 0; i < 10; i++) {
        QObject *object = instantiator->objectAt(i);
        QVERIFY(object);
        QCOMPARE(object->parent(), root);
        QCOMPARE(object->property("success").toBool(), true);
        QCOMPARE(object->property("idx").toInt(), i);
    }
}

void tst_quick3dnodeinstantiator::createNested()
{
    QQmlEngine engine;
    QQmlComponent component(&engine, testFileUrl("createNested.qml"));
    const auto root = qobject_cast<Qt3DCore::QNode*>(component.create());
    QVERIFY(root != 0);

    auto instantiators = root->findChildren<Quick3DNodeInstantiator*>();
    QCOMPARE(instantiators.size(), 4);

    const auto outerInstantiator = instantiators.takeFirst();
    QCOMPARE(outerInstantiator->isActive(), true);
    QCOMPARE(outerInstantiator->count(), 3);

    for (const auto instantiator : instantiators) {
        QCOMPARE(instantiator->isActive(), true);
        QCOMPARE(instantiator->count(), 4);

        for (int i = 0; i < 4; i++) {
            auto object = instantiator->objectAt(i);
            QVERIFY(object);
            QCOMPARE(object->parent(), root);
            QCOMPARE(object->property("success").toBool(), true);
            QCOMPARE(object->property("idx").toInt(), i);
        }
    }
}

void tst_quick3dnodeinstantiator::stringModel()
{
    QQmlEngine engine;
    QQmlComponent component(&engine, testFileUrl("stringModel.qml"));
    const auto root = qobject_cast<Qt3DCore::QNode*>(component.create());
    QVERIFY(root != 0);
    const auto instantiator = root->findChild<Quick3DNodeInstantiator*>();
    QVERIFY(instantiator != 0);
    QCOMPARE(instantiator->isActive(), true);
    QCOMPARE(instantiator->count(), 4);

    for (int i = 0; i < 4; i++) {
        QObject *object = instantiator->objectAt(i);
        QVERIFY(object);
        QCOMPARE(object->parent(), root);
        QCOMPARE(object->property("success").toBool(), true);
    }
}

void tst_quick3dnodeinstantiator::activeProperty()
{
    QQmlEngine engine;
    QQmlComponent component(&engine, testFileUrl("inactive.qml"));
    const auto root = qobject_cast<Qt3DCore::QNode*>(component.create());
    QVERIFY(root != 0);
    const auto instantiator = root->findChild<Quick3DNodeInstantiator*>();
    QVERIFY(instantiator != 0);
    QSignalSpy activeSpy(instantiator, SIGNAL(activeChanged()));
    QSignalSpy countSpy(instantiator, SIGNAL(countChanged()));
    QSignalSpy objectSpy(instantiator, SIGNAL(objectChanged()));
    QSignalSpy modelSpy(instantiator, SIGNAL(modelChanged()));
    QCOMPARE(instantiator->isActive(), false);
    QCOMPARE(instantiator->count(), 0);
    QVERIFY(instantiator->delegate()->isReady());

    QCOMPARE(activeSpy.size(), 0);
    QCOMPARE(countSpy.size(), 0);
    QCOMPARE(objectSpy.size(), 0);
    QCOMPARE(modelSpy.size(), 0);

    instantiator->setActive(true);
    QCOMPARE(instantiator->isActive(), true);
    QCOMPARE(instantiator->count(), 1);

    QCOMPARE(activeSpy.size(), 1);
    QCOMPARE(countSpy.size(), 1);
    QCOMPARE(objectSpy.size(), 1);
    QCOMPARE(modelSpy.size(), 0);

    QObject *object = instantiator->object();
    QVERIFY(object);
    QCOMPARE(object->parent(), root);
    QCOMPARE(object->property("success").toBool(), true);
    QCOMPARE(object->property("idx").toInt(), 0);
}

void tst_quick3dnodeinstantiator::intModelChange()
{
    QQmlEngine engine;
    QQmlComponent component(&engine, testFileUrl("createMultiple.qml"));
    const auto root = qobject_cast<Qt3DCore::QNode*>(component.create());
    QVERIFY(root != 0);
    const auto instantiator = root->findChild<Quick3DNodeInstantiator*>();
    QVERIFY(instantiator != 0);
    QSignalSpy activeSpy(instantiator, SIGNAL(activeChanged()));
    QSignalSpy countSpy(instantiator, SIGNAL(countChanged()));
    QSignalSpy objectSpy(instantiator, SIGNAL(objectChanged()));
    QSignalSpy modelSpy(instantiator, SIGNAL(modelChanged()));
    QCOMPARE(instantiator->count(), 10);

    QCOMPARE(activeSpy.size(), 0);
    QCOMPARE(countSpy.size(), 0);
    QCOMPARE(objectSpy.size(), 0);
    QCOMPARE(modelSpy.size(), 0);

    instantiator->setModel(QVariant(2));
    QCOMPARE(instantiator->count(), 2);

    QCOMPARE(activeSpy.size(), 0);
    QCOMPARE(countSpy.size(), 1);
    QCOMPARE(objectSpy.size(), 2);
    QCOMPARE(modelSpy.size(), 1);

    for (int i = 0; i < 2; i++) {
        QObject *object = instantiator->objectAt(i);
        QVERIFY(object);
        QCOMPARE(object->parent(), root);
        QCOMPARE(object->property("success").toBool(), true);
        QCOMPARE(object->property("idx").toInt(), i);
    }
}

void tst_quick3dnodeinstantiator::createAndRemove()
{
    QQmlEngine engine;
    QQmlComponent component(&engine, testFileUrl("createAndRemove.qml"));
    StringModel *model = new StringModel("model1");
    engine.rootContext()->setContextProperty("model1", model);
    QObject *rootObject = component.create();
    QVERIFY(rootObject != nullptr);

    Quick3DNodeInstantiator *instantiator =
        qobject_cast<Quick3DNodeInstantiator*>(rootObject->findChild<QObject*>("instantiator1"));
    QVERIFY(instantiator != nullptr);
    model->drop(1);
    const QList<QString> names = { "Beta", "Gamma", "Delta" };
    for (int i = 0; i < 3; i++) {
        QObject *object = instantiator->objectAt(i);
        QVERIFY(object);
        QCOMPARE(object->property("datum").toString(), names[i]);
    }
}

void tst_quick3dnodeinstantiator::asynchronous_data()
{
    QTest::addColumn<bool>("asyncIncubator");
    QTest::addColumn<QString>("fileName");

    QTest::newRow("Asynchronous Instantiator") << false << "createMultipleAsync.qml";
    QTest::newRow("Nested-asynchronous Instantiator") << true << "createMultiple.qml";
}

void tst_quick3dnodeinstantiator::asynchronous()
{
    QFETCH(bool, asyncIncubator);
    QFETCH(QString, fileName);

    QQmlEngine engine;
    QQmlIncubationController incubationController;
    engine.setIncubationController(&incubationController);
    QQmlComponent component(&engine, testFileUrl(fileName));
    QQmlIncubator incubator(asyncIncubator ? QQmlIncubator::Asynchronous : QQmlIncubator::Synchronous);
    component.create(incubator);
    while (!incubator.isReady())
        incubationController.incubateFor(10);
    QObject *rootObject = incubator.object();
    QVERIFY(rootObject != nullptr);
    while (incubationController.incubatingObjectCount() > 0)
        incubationController.incubateFor(10);
    Quick3DNodeInstantiator *instantiator = rootObject->findChild<Quick3DNodeInstantiator *>();
    QVERIFY(instantiator != nullptr);
    QCOMPARE(instantiator->isActive(), true);
    QCOMPARE(instantiator->count(), 10);

    for (int i=0; i<10; i++) {
        QObject *object = instantiator->objectAt(i);
        QVERIFY(object);
        QCOMPARE(object->parent(), rootObject);
        QCOMPARE(object->property("success").toBool(), true);
        QCOMPARE(object->property("idx").toInt(), i);
    }
}


QTEST_MAIN(tst_quick3dnodeinstantiator)

#include "tst_quick3dnodeinstantiator.moc"
#include "moc_stringmodel.cpp"
