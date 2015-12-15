/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <Qt3DCore/QAbstractAspect>
#include <Qt3DCore/qaspectengine.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>

using namespace Qt3DCore;

#define FAKE_ASPECT(ClassName) \
class ClassName : public QAbstractAspect \
{ \
    Q_OBJECT \
public: \
    explicit ClassName(QObject *parent = 0) \
        : QAbstractAspect(parent) {} \
\
private: \
    void onRootEntityChanged(QEntity *) Q_DECL_OVERRIDE {} \
    void onInitialize(const QVariantMap &) Q_DECL_OVERRIDE {} \
    void onStartup() Q_DECL_OVERRIDE {} \
    void onShutdown() Q_DECL_OVERRIDE {} \
    void onCleanup() Q_DECL_OVERRIDE {} \
\
    QVector<QAspectJobPtr> jobsToExecute(qint64) Q_DECL_OVERRIDE \
    { \
        return QVector<QAspectJobPtr>(); \
    } \
    \
    QVariant executeCommand(const QStringList &args) Q_DECL_OVERRIDE \
    { \
        if (args.size() >= 2 && args.first() == QLatin1Literal("echo")) { \
            QStringList list = args; \
            list.removeFirst(); \
            return QString("%1 said '%2'").arg(metaObject()->className()).arg(list.join(QLatin1Char(' '))); \
        } \
        \
        return QVariant(); \
    }\
};

FAKE_ASPECT(FakeAspect)
FAKE_ASPECT(FakeAspect2)
FAKE_ASPECT(FakeAspect3)

QT3D_REGISTER_ASPECT("fake", FakeAspect)
QT3D_REGISTER_ASPECT("otherfake", FakeAspect2)


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
        QVERIFY(engine->rootEntity() == Q_NULLPTR);
        delete engine;
    }

    void setRootEntity()
    {
        QAspectEngine *engine = new QAspectEngine;

        QEntity *e = new QEntity;
        e->setObjectName("root");
        engine->setRootEntity(e);

        QSharedPointer<QEntity> root = engine->rootEntity();
        QVERIFY(root == e);
        QVERIFY(root->objectName() == "root");
        root = QSharedPointer<QEntity>();
        QVERIFY(engine->rootEntity()->objectName() == "root");

        delete engine;
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
        engine.setRootEntity(root);

        // THEN
        // Nothing particular happen on exit, especially no crash
    };

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
                 QString("Loaded aspects:\n * fake"));

        // WHEN
        engine.registerAspect("otherfake");

        // THEN
        QCOMPARE(engine.executeCommand("list aspects").toString(),
                 QString("Loaded aspects:\n * fake\n * otherfake"));

        // WHEN
        engine.registerAspect(new FakeAspect3);

        // THEN
        QCOMPARE(engine.executeCommand("list aspects").toString(),
                 QString("Loaded aspects:\n * fake\n * otherfake\n * <unnamed>"));
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
};

QTEST_MAIN(tst_QAspectEngine)

#include "tst_qaspectengine.moc"
