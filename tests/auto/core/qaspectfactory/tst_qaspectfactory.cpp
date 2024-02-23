// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DCore/private/qaspectfactory_p.h>
#include <Qt3DCore/QAbstractAspect>

using namespace QT_PREPEND_NAMESPACE(Qt3DCore);

#define FAKE_ASPECT(ClassName) \
class ClassName : public QAbstractAspect \
{ \
    Q_OBJECT \
public: \
    explicit ClassName(QObject *parent = 0) \
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
};

FAKE_ASPECT(DefaultFakeAspect)
FAKE_ASPECT(AnotherFakeAspect)

QT3D_REGISTER_ASPECT("default", DefaultFakeAspect)

class tst_QAspectFactory : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void shoulHaveDefaultState()
    {
        // GIVEN
        QAspectFactory factory;

        // THEN
        QCOMPARE(factory.availableFactories().size(), 2);
        // order is not deterministic
        QVERIFY(factory.availableFactories().contains(QLatin1String("core")));
        QVERIFY(factory.availableFactories().contains(QLatin1String("default")));

        // WHEN
        QAbstractAspect *aspect = factory.createAspect(QLatin1String("default"));

        // THEN
        QVERIFY(qobject_cast<DefaultFakeAspect*>(aspect) != nullptr);
        QVERIFY(aspect->parent() == nullptr);
    }

    void shouldKnowAspectNames()
    {
        // GIVEN
        QAspectFactory factory;

        // WHEN
        DefaultFakeAspect fake;
        AnotherFakeAspect missing;

        // THEN
        QCOMPARE(factory.aspectName(&fake), QLatin1String("default"));
        QCOMPARE(factory.aspectName(&missing), QLatin1String());
    }

    void shouldGracefulyHandleMissingFactories()
    {
        // GIVEN
        QAspectFactory factory;

        // WHEN
        QAbstractAspect *aspect = factory.createAspect(QLatin1String("missing"), this);

        // THEN
        QVERIFY(qobject_cast<AnotherFakeAspect*>(aspect) == nullptr);
    }
};

QTEST_MAIN(tst_QAspectFactory)

#include "tst_qaspectfactory.moc"
