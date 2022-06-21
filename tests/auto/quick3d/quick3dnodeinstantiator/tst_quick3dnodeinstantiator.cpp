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
    QCOMPARE(instantiators.count(), 4);

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

    QCOMPARE(activeSpy.count(), 0);
    QCOMPARE(countSpy.count(), 0);
    QCOMPARE(objectSpy.count(), 0);
    QCOMPARE(modelSpy.count(), 0);

    instantiator->setActive(true);
    QCOMPARE(instantiator->isActive(), true);
    QCOMPARE(instantiator->count(), 1);

    QCOMPARE(activeSpy.count(), 1);
    QCOMPARE(countSpy.count(), 1);
    QCOMPARE(objectSpy.count(), 1);
    QCOMPARE(modelSpy.count(), 0);

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

    QCOMPARE(activeSpy.count(), 0);
    QCOMPARE(countSpy.count(), 0);
    QCOMPARE(objectSpy.count(), 0);
    QCOMPARE(modelSpy.count(), 0);

    instantiator->setModel(QVariant(2));
    QCOMPARE(instantiator->count(), 2);

    QCOMPARE(activeSpy.count(), 0);
    QCOMPARE(countSpy.count(), 1);
    QCOMPARE(objectSpy.count(), 2);
    QCOMPARE(modelSpy.count(), 1);

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
QTEST_MAIN(tst_quick3dnodeinstantiator)

#include "tst_quick3dnodeinstantiator.moc"
#include "moc_stringmodel.cpp"
