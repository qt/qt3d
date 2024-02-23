// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <QTimer>

#include <Qt3DCore/QAbstractAspect>
#include <Qt3DCore/qaspectengine.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>

using namespace Qt3DCore;

class PrintRootAspect : public QAbstractAspect
{
    Q_OBJECT
public:
    explicit PrintRootAspect(QObject *parent = 0)
        : QAbstractAspect(parent)
        , m_rootEntityId()
    {
        qDebug() << Q_FUNC_INFO;
    }

private:
    void onRegistered() override
    {
        qDebug() << Q_FUNC_INFO;
    }

    void onEngineStartup() override
    {
        qDebug() << Q_FUNC_INFO;
        m_rootEntityId = rootEntityId();
    }

    void onEngineShutdown() override
    {
        qDebug() << Q_FUNC_INFO;
    }

    std::vector<QAspectJobPtr> jobsToExecute(qint64) override \
    {
        if (m_rootEntityId)
            qDebug() << Q_FUNC_INFO << m_rootEntityId;
        return {};
    }

    QNodeId m_rootEntityId;
};

#define FAKE_ASPECT(ClassName, dependAspects) \
class ClassName : public QAbstractAspect \
{ \
    Q_OBJECT \
public: \
    explicit ClassName(QObject *parent = nullptr) \
        : QAbstractAspect(parent) {} \
    \
private: \
    void onRegistered() override {} \
    void onEngineStartup() override {} \
    void onEngineShutdown() override {} \
    \
    std::vector<QAspectJobPtr> jobsToExecute(qint64) override \
    { \
        return std::vector<QAspectJobPtr>(); \
    } \
    \
    QVariant executeCommand(const QStringList &args) override \
    { \
        if (args.size() >= 2 && args.first() == QLatin1String("echo")) { \
            QStringList list = args; \
            list.removeFirst(); \
            return QString("%1 said '%2'").arg(metaObject()->className()).arg(list.join(QLatin1Char(' '))); \
        } \
        \
        return QVariant(); \
    } \
    QStringList dependencies() const override { return dependAspects; } \
};

FAKE_ASPECT(FakeAspect, {})
FAKE_ASPECT(FakeAspect2, {})
FAKE_ASPECT(FakeAspect3, {})
FAKE_ASPECT(FakeAspectDependent, {QLatin1String("fake")})

QT3D_REGISTER_ASPECT("fake", FakeAspect)
QT3D_REGISTER_ASPECT("otherfake", FakeAspect2)
QT3D_REGISTER_ASPECT("dependfake", FakeAspectDependent)


class tst_QAspectEngine : public QObject
{
    Q_OBJECT
public:
    tst_QAspectEngine() : QObject() {}
    ~tst_QAspectEngine() {}

private Q_SLOTS:
    // TODO: Add more QAspectEngine tests

    void constructionDestruction()
    {
        QAspectEngine *engine = new QAspectEngine;
        QVERIFY(engine->rootEntity() == nullptr);
        delete engine;
    }

    void setRootEntity()
    {
        QAspectEngine *engine = new QAspectEngine;

        QEntity *e = new QEntity;
        e->setObjectName("root");
        engine->setRootEntity(QEntityPtr(e));

        QSharedPointer<QEntity> root = engine->rootEntity();
        QVERIFY(root == e);
        QVERIFY(root->objectName() == "root");
        root = QSharedPointer<QEntity>();
        QVERIFY(engine->rootEntity()->objectName() == "root");

        delete engine;
    }

    void shouldNotCrashInNormalStartupShutdownSequence()
    {
#ifdef Q_OS_MACOS
        QSKIP("Test frequently times out. See QTBUG-80660.");
#endif

        // GIVEN
        // An initialized aspect engine...
        QAspectEngine engine;
        // ...and a simple aspect
        PrintRootAspect *aspect = new PrintRootAspect;

        // WHEN
        // We register the aspect
        engine.registerAspect(aspect);

        // THEN
        const auto registeredAspects = engine.aspects();
        QCOMPARE(registeredAspects.size(), 1);
        QCOMPARE(registeredAspects.first(), aspect);

        // WHEN
        QEntityPtr entity(new QEntity);
        entity->setObjectName("RootEntity");
        // we set a scene root entity
        engine.setRootEntity(entity);

        QEventLoop eventLoop;
        QTimer::singleShot(1000, &eventLoop, SLOT(quit()));
        eventLoop.exec();

        // THEN
        // we don't crash and...
        const auto rootEntity = engine.rootEntity();
        QCOMPARE(rootEntity, entity);

        // WHEN
        // we set an empty/null scene root...
        engine.setRootEntity(QEntityPtr());
        QTimer::singleShot(600, &eventLoop, SLOT(quit()));

        // ...and allow events to process...
        eventLoop.exec();

        // THEN
        // ... we don't crash.

        // TODO: Add more tests to check for
        // * re-setting a scene
        // * deregistering aspects
        // * destroying the aspect engine
    }

    void shouldNotCrashWhenEntityIsAddedThenImmediatelyDeleted()
    {
        // GIVEN
        // An initialized aspect engine...
        QAspectEngine engine;
        // ...and a simple aspect
        PrintRootAspect *aspect = new PrintRootAspect;

        // WHEN
        // We register the aspect
        engine.registerAspect(aspect);

        // THEN
        const auto registeredAspects = engine.aspects();
        QCOMPARE(registeredAspects.size(), 1);
        QCOMPARE(registeredAspects.first(), aspect);

        // WHEN
        QEntityPtr entity(new QEntity);
        entity->setObjectName("RootEntity");
        // we set a scene root entity
        engine.setRootEntity(entity);

        QEventLoop eventLoop;
        QTimer::singleShot(1000, &eventLoop, SLOT(quit()));
        eventLoop.exec();

        // THEN
        // we don't crash and...
        const auto rootEntity = engine.rootEntity();
        QCOMPARE(rootEntity, entity);

        // WHEN
        // we create a child node and delete within the same spin of
        // the event loop
        Qt3DCore::QEntity *childEntity = new Qt3DCore::QEntity(entity.data());
        delete childEntity;
        entity = nullptr;
        QTimer::singleShot(600, &eventLoop, SLOT(quit()));

        // ...and allow events to process...
        eventLoop.exec();

        // ...and we don't crash when the childEntity is removed from the
        // post construction init routines
    }

    void shouldNotCrashOnShutdownWhenComponentIsCreatedWithParentBeforeItsEntity()
    {
        // GIVEN
        QEntity *root = new QEntity;
        // A component parented to an entity...
        QComponent *component = new Qt3DCore::QTransform(root);
        // ... created *before* the entity it will be added to.
        QEntity *entity = new QEntity(root);
        entity->addComponent(component);

        // An initialized engine (so that the arbiter has been fed)
        QAspectEngine engine;

        // WHEN
        engine.setRootEntity(QEntityPtr(root));

        // THEN
        // Nothing particular happen on exit, especially no crash
    }

    void shouldRegisterAspectsByName()
    {
        // GIVEN
        QAspectEngine engine;

        // THEN
        QVERIFY(engine.aspects().isEmpty());

        // WHEN
        engine.registerAspect("fake");

        // THEN
        QCOMPARE(engine.aspects().size(), 1);
        QVERIFY(qobject_cast<FakeAspect*>(engine.aspects().first()));
    }

    void shouldListLoadedAspects()
    {
        // GIVEN
        QAspectEngine engine;

        // THEN
        QCOMPARE(engine.executeCommand("list aspects").toString(),
                 QString("No loaded aspect"));

        // WHEN
        engine.registerAspect("fake");

        // THEN
        QCOMPARE(engine.executeCommand("list aspects").toString(),
                 QString("fake"));

        // WHEN
        engine.registerAspect("otherfake");

        // THEN
        QCOMPARE(engine.executeCommand("list aspects").toString(),
                 QString("fake\notherfake"));

        // WHEN
        engine.registerAspect(new FakeAspect3);

        // THEN
        QCOMPARE(engine.executeCommand("list aspects").toString(),
                 QString("fake\notherfake\n<unnamed>"));
    }

    void shouldDelegateCommandsToAspects()
    {
        // GIVEN
        QAspectEngine engine;
        engine.registerAspect("fake");
        engine.registerAspect("otherfake");
        engine.registerAspect(new FakeAspect3);

        // WHEN
        QVariant output = engine.executeCommand("fake echo foo bar");

        // THEN
        QVERIFY(output.isValid());
        QCOMPARE(output.toString(), QString("FakeAspect said 'foo bar'"));

        // WHEN
        output = engine.executeCommand("otherfake echo fizz buzz");

        // THEN
        QVERIFY(output.isValid());
        QCOMPARE(output.toString(), QString("FakeAspect2 said 'fizz buzz'"));

        // WHEN
        output = engine.executeCommand("mehfake echo fizz buzz");

        // THEN
        QVERIFY(!output.isValid());

        // WHEN
        output = engine.executeCommand("fake mooh fizz buzz");

        // THEN
        QVERIFY(!output.isValid());
    }

    void shouldRegisterDependentAspects()
    {
        // GIVEN
        QAspectEngine engine;

        // THEN
        QVERIFY(engine.aspects().isEmpty());

        // WHEN
        engine.registerAspect("dependfake");

        // THEN
        QCOMPARE(engine.aspects().size(), 2);
        QVERIFY(qobject_cast<FakeAspect*>(engine.aspects().at(0)));
        QVERIFY(qobject_cast<FakeAspectDependent*>(engine.aspects().at(1)));
    }
};

QTEST_MAIN(tst_QAspectEngine)

#include "tst_qaspectengine.moc"
