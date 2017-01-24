/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QTest>
#include <Qt3DCore/private/qpostman_p.h>
#include <Qt3DCore/private/qpostman_p_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/qpropertynodeaddedchange.h>
#include <Qt3DCore/qpropertynoderemovedchange.h>
#include <Qt3DCore/private/qnodecreatedchangegenerator_p.h>
#include <Qt3DCore/private/qpropertyupdatedchangebase_p.h>
#include "testpostmanarbiter.h"


namespace {

class NodeChangeReceiver: public Qt3DCore::QNode
{
public:
    NodeChangeReceiver(Qt3DCore::QNode *parent = nullptr)
        : Qt3DCore::QNode(parent)
        , m_hasReceivedChange(false)
    {}

    inline bool hasReceivedChange() const { return m_hasReceivedChange; }

protected:
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &) Q_DECL_OVERRIDE
    {
        m_hasReceivedChange = true;
    }

private:
    bool m_hasReceivedChange;
};

} // anonymous

class tst_QPostman : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkSetScene()
    {
        // GIVEN
        Qt3DCore::QPostman postman;

        // THEN
        QVERIFY(Qt3DCore::QPostmanPrivate::get(&postman)->m_scene == nullptr);

        // WHEN
        Qt3DCore::QScene scene;
        postman.setScene(&scene);

        // THEN
        QCOMPARE(Qt3DCore::QPostmanPrivate::get(&postman)->m_scene, &scene);
    }

    void checkSceneChangeEvent()
    {
        // GIVEN
        QScopedPointer<Qt3DCore::QScene> scene(new Qt3DCore::QScene);
        Qt3DCore::QPostman postman;
        TestArbiter arbiter;
        Qt3DCore::QNode rootNode;
        NodeChangeReceiver *receiverNode = new NodeChangeReceiver();

        Qt3DCore::QNodePrivate::get(&rootNode)->m_scene = scene.data();
        scene->setArbiter(&arbiter);
        postman.setScene(scene.data());
        // Setting the parent (which has a scene) adds the node into the observable lookup
        // table of the scene which is needed by the postman to distribute changes
        static_cast<Qt3DCore::QNode *>(receiverNode)->setParent(&rootNode);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(receiverNode->hasReceivedChange(), false);
        QCOMPARE(Qt3DCore::QNodePrivate::get(receiverNode)->m_scene, scene.data());

        // WHEN
        Qt3DCore::QPropertyUpdatedChangePtr updateChange(new Qt3DCore::QPropertyUpdatedChange(receiverNode->id()));
        updateChange->setValue(1584);
        updateChange->setPropertyName("someName");
        postman.sceneChangeEvent(updateChange);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(receiverNode->hasReceivedChange(), true);
    }

    void checkNotifyBackend()
    {
        // GIVEN
        QScopedPointer<Qt3DCore::QScene> scene(new Qt3DCore::QScene);
        Qt3DCore::QPostman postman;
        TestArbiter arbiter;

        scene->setArbiter(&arbiter);
        postman.setScene(scene.data());

        // THEN
        QCOMPARE(arbiter.events.size(), 0);

        // WHEN
        Qt3DCore::QPropertyUpdatedChangePtr updateChange(new Qt3DCore::QPropertyUpdatedChange(Qt3DCore::QNodeId()));
        updateChange->setValue(1584);
        updateChange->setPropertyName("someName");
        postman.notifyBackend(updateChange);

        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
    }

    void checkShouldNotifyFrontend()
    {
        // GIVEN
        QScopedPointer<Qt3DCore::QScene> scene(new Qt3DCore::QScene);
        Qt3DCore::QPostman postman;
        TestArbiter arbiter;
        Qt3DCore::QNode rootNode;
        NodeChangeReceiver *receiverNode = new NodeChangeReceiver();

        Qt3DCore::QNodePrivate::get(&rootNode)->m_scene = scene.data();
        scene->setArbiter(&arbiter);
        postman.setScene(scene.data());
        // Setting the parent (which has a scene) adds the node into the observable lookup
        // table of the scene which is needed by the postman to distribute changes
        static_cast<Qt3DCore::QNode *>(receiverNode)->setParent(&rootNode);
        QCoreApplication::processEvents();

        {
            // WHEN
            Qt3DCore::QPropertyUpdatedChangePtr updateChange(new Qt3DCore::QPropertyUpdatedChange(receiverNode->id()));
            updateChange->setValue(1584);
            updateChange->setPropertyName("someName");

            // THEN -> we don't track properties by default Qt3DCore::QNode::DontTrackProperties
            QCOMPARE(postman.shouldNotifyFrontend(updateChange), false);
        }

        {
            // WHEN
            receiverNode->setPropertyTrackMode(Qt3DCore::QNode::TrackAllPropertiesMode);

            Qt3DCore::QPropertyUpdatedChangePtr updateChange(new Qt3DCore::QPropertyUpdatedChange(receiverNode->id()));
            updateChange->setValue(1584);
            updateChange->setPropertyName("someName");

            // THEN -> we don't track properties by default
            QCOMPARE(postman.shouldNotifyFrontend(updateChange), true);
        }

        {
            // GIVEN
            receiverNode->setPropertyTrackMode(Qt3DCore::QNode::TrackNamedPropertiesMode);
            receiverNode->setTrackedProperties(QStringList() << QStringLiteral("vette"));

            {
                // WHEN
                Qt3DCore::QPropertyUpdatedChangePtr updateChange(new Qt3DCore::QPropertyUpdatedChange(receiverNode->id()));
                updateChange->setValue(1584);
                updateChange->setPropertyName("someName");

                // THEN -> we don't track properties by default
                QCOMPARE(postman.shouldNotifyFrontend(updateChange), false);
            }

            {
                // WHEN
                Qt3DCore::QPropertyUpdatedChangePtr updateChange(new Qt3DCore::QPropertyUpdatedChange(receiverNode->id()));
                updateChange->setValue(1584);
                updateChange->setPropertyName("vette");

                // THEN
                QCOMPARE(postman.shouldNotifyFrontend(updateChange), true);
            }
        }

        {
            // GIVEN
            receiverNode->setTrackedProperties(QStringList() << QStringLiteral("vette"));
            receiverNode->setPropertyTrackMode(Qt3DCore::QNode::TrackAllPropertiesMode);

            {
                // WHEN
                Qt3DCore::QPropertyUpdatedChangePtr updateChange(new Qt3DCore::QPropertyUpdatedChange(receiverNode->id()));
                updateChange->setValue(1584);
                updateChange->setPropertyName("someName");

                // THEN -> we don't track properties by default
                QCOMPARE(postman.shouldNotifyFrontend(updateChange), true);
            }

            {
                // WHEN
                Qt3DCore::QPropertyUpdatedChangePtr updateChange(new Qt3DCore::QPropertyUpdatedChange(receiverNode->id()));
                updateChange->setValue(1584);
                updateChange->setPropertyName("vette");

                // THEN -> we don't track properties by default
                QCOMPARE(postman.shouldNotifyFrontend(updateChange), true);
            }
        }

        {
            // GIVEN
            receiverNode->setTrackedProperties(QStringList());
            receiverNode->setPropertyTrackMode(Qt3DCore::QNode::DefaultTrackMode);

            {
                // WHEN
                Qt3DCore::QPropertyUpdatedChangePtr updateChange(new Qt3DCore::QPropertyUpdatedChange(receiverNode->id()));
                updateChange->setValue(1584);
                updateChange->setPropertyName("someName");

                // THEN -> we don't track properties by default
                QCOMPARE(postman.shouldNotifyFrontend(updateChange), false);
            }

            {
                // WHEN
                Qt3DCore::QPropertyNodeAddedChangePtr addedChange(new Qt3DCore::QPropertyNodeAddedChange(receiverNode->id(), receiverNode));

                // THEN -> only QPropertyUpdatedChangePtr are filtered
                QCOMPARE(postman.shouldNotifyFrontend(addedChange), true);
            }
            {
                // WHEN
                Qt3DCore::QPropertyNodeRemovedChangePtr removedChange(new Qt3DCore::QPropertyNodeRemovedChange(receiverNode->id(), receiverNode));

                // THEN -> only QPropertyUpdatedChangePtr are filtered
                QCOMPARE(postman.shouldNotifyFrontend(removedChange), true);
            }
        }

        {
            // GIVEN
            receiverNode->setTrackedProperties(QStringList());
            receiverNode->setPropertyTrackMode(Qt3DCore::QNode::DefaultTrackMode);

            {
                // WHEN
                Qt3DCore::QPropertyUpdatedChangePtr updateChange(new Qt3DCore::QPropertyUpdatedChange(receiverNode->id()));
                updateChange->setValue(1584);
                updateChange->setPropertyName("someName");

                // THEN -> we don't track properties by default
                QCOMPARE(postman.shouldNotifyFrontend(updateChange), false);
            }

            {
                // WHEN
                Qt3DCore::QPropertyUpdatedChangePtr updateChange(new Qt3DCore::QPropertyUpdatedChange(receiverNode->id()));
                updateChange->setValue(1584);
                updateChange->setPropertyName("someName");
                Qt3DCore::QPropertyUpdatedChangeBasePrivate::get(updateChange.data())->m_isFinal = true;

                // THEN
                QCOMPARE(postman.shouldNotifyFrontend(updateChange), true);
            }

        }
    }

};

QTEST_MAIN(tst_QPostman)

#include "tst_qpostman.moc"
