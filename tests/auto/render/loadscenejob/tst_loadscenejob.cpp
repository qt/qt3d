// Copyright (C) 2016 Paul Lemire <paul.lemire350@gmail.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DRender/private/scene_p.h>
#include <Qt3DRender/private/loadscenejob_p.h>
#include <Qt3DRender/private/qsceneimporter_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DRender/private/scenemanager_p.h>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/private/qbackendnode_p.h>
#include "testarbiter.h"

class TestSceneImporter : public Qt3DRender::QSceneImporter
{
public:
    explicit TestSceneImporter(bool supportsFormat, bool shouldFail)
        : m_supportsFormat(supportsFormat)
        , m_shouldFail(shouldFail)
    {}

    void setSource(const QUrl &source) override
    {
        m_source = source;
    }

    void setData(const QByteArray& data, const QString &basePath) override
    {
        Q_UNUSED(data);
        Q_UNUSED(basePath);
    }

    bool areFileTypesSupported(const QStringList &extensions) const override
    {
        Q_UNUSED(extensions);
        return m_supportsFormat;
    }

    Qt3DCore::QEntity *scene(const QString &) override
    {
        return m_shouldFail ? nullptr : new Qt3DCore::QEntity();
    }

    Qt3DCore::QEntity *node(const QString &) override
    {
        return m_shouldFail ? nullptr : new Qt3DCore::QEntity();
    }

    QUrl source() const
    {
        return m_source;
    }

private:
    QUrl m_source;
    bool m_supportsFormat;
    bool m_shouldFail;
};

class tst_LoadSceneJob : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        QUrl url;
        Qt3DCore::QNodeId nodeId;
        Qt3DRender::Render::LoadSceneJob backendLoadSceneJob(url, nodeId);

        // THEN
        QCOMPARE(backendLoadSceneJob.source(), url);
        QCOMPARE(backendLoadSceneJob.sceneComponentId(), nodeId);
        QVERIFY(backendLoadSceneJob.nodeManagers() == nullptr);
        QCOMPARE(backendLoadSceneJob.sceneImporters().size(), 0);
    }

    void checkInitialize()
    {
        // GIVEN
        const QUrl url(QStringLiteral("file:///URL"));
        const Qt3DCore::QNodeId sceneId = Qt3DCore::QNodeId::createId();
        Qt3DRender::Render::NodeManagers nodeManagers;
        TestSceneImporter fakeImporter(true, true);

        // WHEN
        Qt3DRender::Render::LoadSceneJob backendLoadSceneJob(url, sceneId);
        backendLoadSceneJob.setNodeManagers(&nodeManagers);
        backendLoadSceneJob.setSceneImporters(
                    QList<Qt3DRender::QSceneImporter *>() << &fakeImporter);

        // THEN
        QCOMPARE(backendLoadSceneJob.source(), url);
        QCOMPARE(backendLoadSceneJob.sceneComponentId(), sceneId);
        QVERIFY(backendLoadSceneJob.nodeManagers() == &nodeManagers);
        QCOMPARE(backendLoadSceneJob.sceneImporters().size(), 1);
        QCOMPARE(backendLoadSceneJob.sceneImporters().first(), &fakeImporter);
    }

    void checkRunValidSourceSupportedFormat()
    {
        QSKIP("Can't test successful loading");

        // GIVEN
        const QUrl url(QStringLiteral("file:///URL"));
        TestArbiter arbiter;
        Qt3DRender::Render::NodeManagers nodeManagers;
        TestSceneImporter fakeImporter(true, false);
        Qt3DCore::QNodeId sceneId = Qt3DCore::QNodeId::createId();
        Qt3DRender::Render::Scene *scene = nodeManagers.sceneManager()->getOrCreateResource(sceneId);

        // THEN
        QVERIFY(scene != nullptr);

        // WHEN
        Qt3DRender::Render::LoadSceneJob loadSceneJob(url, sceneId);
        loadSceneJob.setNodeManagers(&nodeManagers);
        loadSceneJob.setSceneImporters(QList<Qt3DRender::QSceneImporter *>() << &fakeImporter);
        loadSceneJob.run();

        // THEN
        Qt3DRender::Render::LoadSceneJobPrivate *dJob = static_cast<decltype(dJob)>(Qt3DCore::QAspectJobPrivate::get(&loadSceneJob));
        QCOMPARE(dJob->m_status, Qt3DRender::QSceneLoader::Ready);
        QVERIFY(dJob->m_sceneSubtree != nullptr);
    }

    void checkEmptySource()
    {
        // GIVEN
        QUrl url;
        TestArbiter arbiter;
        Qt3DRender::Render::NodeManagers nodeManagers;
        TestSceneImporter fakeImporter(true, false);
        Qt3DCore::QNodeId sceneId = Qt3DCore::QNodeId::createId();
        Qt3DRender::Render::Scene *scene = nodeManagers.sceneManager()->getOrCreateResource(sceneId);

        // THEN
        QVERIFY(scene != nullptr);

        // WHEN
        Qt3DRender::Render::LoadSceneJob loadSceneJob(url, sceneId);
        loadSceneJob.setNodeManagers(&nodeManagers);
        loadSceneJob.setSceneImporters(QList<Qt3DRender::QSceneImporter *>() << &fakeImporter);
        loadSceneJob.run();

        // THEN
        Qt3DRender::Render::LoadSceneJobPrivate *dJob = static_cast<decltype(dJob)>(Qt3DCore::QAspectJobPrivate::get(&loadSceneJob));
        QCOMPARE(dJob->m_status, Qt3DRender::QSceneLoader::None);
        QVERIFY(dJob->m_sceneSubtree == nullptr);
    }

    void checkRunValidSourceUnsupportedFormat()
    {
        // no data is loaded so...
        QSKIP("Can't differentiate between no data and unsupported data");

        // GIVEN
        const QUrl url(QStringLiteral("file:///URL"));
        TestArbiter arbiter;
        Qt3DRender::Render::NodeManagers nodeManagers;
        TestSceneImporter fakeImporter(false, false);
        Qt3DCore::QNodeId sceneId = Qt3DCore::QNodeId::createId();
        Qt3DRender::Render::Scene *scene = nodeManagers.sceneManager()->getOrCreateResource(sceneId);

        // THEN
        QVERIFY(scene != nullptr);

        // WHEN
        Qt3DRender::Render::LoadSceneJob loadSceneJob(url, sceneId);
        loadSceneJob.setNodeManagers(&nodeManagers);
        loadSceneJob.setSceneImporters(QList<Qt3DRender::QSceneImporter *>() << &fakeImporter);
        loadSceneJob.run();

        // THEN
        Qt3DRender::Render::LoadSceneJobPrivate *dJob = static_cast<decltype(dJob)>(Qt3DCore::QAspectJobPrivate::get(&loadSceneJob));
        QCOMPARE(dJob->m_status, Qt3DRender::QSceneLoader::Error);
        QVERIFY(dJob->m_sceneSubtree == nullptr);
    }

    void checkRunErrorAtLoading()
    {
        // GIVEN
        const QUrl url(QStringLiteral("file:///URL"));
        Qt3DRender::Render::NodeManagers nodeManagers;
        TestSceneImporter fakeImporter(true, true);
        Qt3DCore::QNodeId sceneId = Qt3DCore::QNodeId::createId();
        Qt3DRender::Render::Scene *scene = nodeManagers.sceneManager()->getOrCreateResource(sceneId);

        // THEN
        QVERIFY(scene != nullptr);

        // WHEN
        Qt3DRender::Render::LoadSceneJob loadSceneJob(url, sceneId);
        loadSceneJob.setNodeManagers(&nodeManagers);
        loadSceneJob.setSceneImporters(QList<Qt3DRender::QSceneImporter *>() << &fakeImporter);
        loadSceneJob.run();

        // THEN
        // THEN
        Qt3DRender::Render::LoadSceneJobPrivate *dJob = static_cast<decltype(dJob)>(Qt3DCore::QAspectJobPrivate::get(&loadSceneJob));
        QCOMPARE(dJob->m_status, Qt3DRender::QSceneLoader::Error);
        QVERIFY(dJob->m_sceneSubtree == nullptr);
    }
};

QTEST_MAIN(tst_LoadSceneJob)

#include "tst_loadscenejob.moc"
