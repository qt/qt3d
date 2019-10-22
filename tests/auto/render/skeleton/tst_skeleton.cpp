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
#include <Qt3DRender/private/skeleton_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>
#include <Qt3DCore/qjoint.h>
#include <Qt3DCore/qskeleton.h>
#include <Qt3DCore/qskeletonloader.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/qbackendnode_p.h>
#include <qbackendnodetester.h>
#include <testpostmanarbiter.h>
#include <testrenderer.h>

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DRender::Render;

Q_DECLARE_METATYPE(Qt3DRender::Render::JointInfo)
Q_DECLARE_METATYPE(Qt3DRender::Render::SkeletonData)
Q_DECLARE_METATYPE(Qt3DCore::Sqt)

class tst_Skeleton : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:
    void checkPeerPropertyMirroring()
    {
        // GIVEN
        TestRenderer renderer;
        NodeManagers nodeManagers;
        renderer.setNodeManagers(&nodeManagers);
        Skeleton backendSkeleton;
        backendSkeleton.setRenderer(&renderer);
        backendSkeleton.setSkeletonManager(nodeManagers.skeletonManager());
        QSkeletonLoader skeleton;

        skeleton.setSource(QUrl::fromLocalFile("funnybones.json"));

        // WHEN
        simulateInitializationSync(&skeleton, &backendSkeleton);

        // THEN
        QCOMPARE(backendSkeleton.peerId(), skeleton.id());
        QCOMPARE(backendSkeleton.isEnabled(), skeleton.isEnabled());
        QCOMPARE(backendSkeleton.source(), skeleton.source());
        QCOMPARE(backendSkeleton.rootJointId(), QNodeId());

        // GIVEN
        Skeleton backendSkeleton2;
        backendSkeleton2.setRenderer(&renderer);
        backendSkeleton2.setSkeletonManager(nodeManagers.skeletonManager());
        QSkeleton skeleton2;

        QJoint *joint = new QJoint();
        skeleton2.setRootJoint(joint);

        // WHEN
        simulateInitializationSync(&skeleton2, &backendSkeleton2);

        // THEN
        QCOMPARE(backendSkeleton2.peerId(), skeleton2.id());
        QCOMPARE(backendSkeleton2.isEnabled(), skeleton2.isEnabled());
        QCOMPARE(backendSkeleton2.source(), QUrl());
        QCOMPARE(backendSkeleton2.rootJointId(), joint->id());
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        TestRenderer renderer;
        NodeManagers nodeManagers;
        renderer.setNodeManagers(&nodeManagers);
        Skeleton backendSkeleton;
        backendSkeleton.setRenderer(&renderer);
        backendSkeleton.setSkeletonManager(nodeManagers.skeletonManager());

        // THEN
        QVERIFY(backendSkeleton.peerId().isNull());
        QCOMPARE(backendSkeleton.isEnabled(), false);
        QCOMPARE(backendSkeleton.source(), QUrl());
        QCOMPARE(backendSkeleton.status(), QSkeletonLoader::NotReady);
        QCOMPARE(backendSkeleton.rootJointId(), QNodeId());

        // GIVEN
        QSkeletonLoader skeleton;
        skeleton.setSource(QUrl::fromLocalFile("skeleton1.json"));

        // WHEN
        simulateInitializationSync(&skeleton, &backendSkeleton);
        backendSkeleton.cleanup();

        // THEN
        QCOMPARE(backendSkeleton.source(), QUrl());
        QCOMPARE(backendSkeleton.isEnabled(), false);
        QCOMPARE(backendSkeleton.status(), QSkeletonLoader::NotReady);
        QCOMPARE(backendSkeleton.rootJointId(), QNodeId());
    }

    void checkDirectPropertyChanges()
    {
        // GIVEN
        TestRenderer renderer;
        NodeManagers nodeManagers;
        renderer.setNodeManagers(&nodeManagers);
        Skeleton backendSkeleton;
        backendSkeleton.setRenderer(&renderer);
        backendSkeleton.setSkeletonManager(nodeManagers.skeletonManager());
        backendSkeleton.setDataType(Skeleton::File);

        // Initialize to ensure skeleton manager is set
        QSkeletonLoader skeleton;
        skeleton.setSource(QUrl::fromLocalFile("skeleton1.json"));
        simulateInitializationSync(&skeleton, &backendSkeleton);

        // WHEN
        skeleton.setEnabled(false);
        backendSkeleton.syncFromFrontEnd(&skeleton, false);

        // THEN
        QCOMPARE(backendSkeleton.isEnabled(), false);

        // WHEN
        const QUrl newSource = QUrl::fromLocalFile("terminator.json");
        skeleton.setSource(newSource);
        backendSkeleton.syncFromFrontEnd(&skeleton, false);

        // THEN
        QCOMPARE(backendSkeleton.source(), newSource);
    }

    void checkCreateFrontendJoint_data()
    {
        QTest::addColumn<QMatrix4x4>("inverseBindMatrix");
        QTest::addColumn<Qt3DCore::Sqt>("localPose");
        QTest::addColumn<QString>("jointName");
        QTest::addColumn<QJoint *>("expectedJoint");

        QMatrix4x4 m;
        Qt3DCore::Sqt localPose;
        QTest::newRow("default") << m << localPose << QString() << new QJoint();

        const QVector3D t(1.0f, 2.0f, 3.0f);
        const QQuaternion r = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, 45.0f);
        const QVector3D s(1.5f, 2.5f, 3.5f);
        localPose.scale = s;
        localPose.rotation = r;
        localPose.translation = t;


        QString name = QLatin1String("Foo");
        QJoint *joint = new QJoint();
        joint->setTranslation(t);
        joint->setRotation(r);
        joint->setScale(s);
        joint->setName(name);
        joint->setInverseBindMatrix(m);
        QTest::newRow("localPose") << m << localPose << name << joint;

        m.rotate(r);
        m.scale(QVector3D(1.0f, 1.0f, 1.0f) / s);
        m.translate(-t);
        name = QLatin1String("Bar");

        joint = new QJoint();
        joint->setTranslation(t);
        joint->setRotation(r);
        joint->setScale(s);
        joint->setInverseBindMatrix(m);
        joint->setName(name);
        QTest::newRow("inverseBind") << m << localPose << name << joint;
    }
};

QTEST_APPLESS_MAIN(tst_Skeleton)

#include "tst_skeleton.moc"
