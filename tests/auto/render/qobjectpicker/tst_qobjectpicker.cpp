/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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
#include <QtTest/QSignalSpy>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/private/qobjectpicker_p.h>
#include <Qt3DRender/QPickEvent>
#include <Qt3DCore/private/qnodecreatedchangegenerator_p.h>
#include <Qt3DCore/qnodecreatedchange.h>
#include "testpostmanarbiter.h"

class MyObjectPicker : public Qt3DRender::QObjectPicker
{
    Q_OBJECT
public:
    MyObjectPicker(Qt3DCore::QNode *parent = nullptr)
        : Qt3DRender::QObjectPicker(parent)
    {}

    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) final
    {
        Qt3DRender::QObjectPicker::sceneChangeEvent(change);
    }

private:
    friend class tst_ObjectPicker;

};

// We need to call QNode::clone which is protected
// So we sublcass QNode instead of QObject
class tst_QObjectPicker : public Qt3DCore::QNode
{
    Q_OBJECT
public:
    tst_QObjectPicker()
    {
        qRegisterMetaType<Qt3DRender::QPickEvent*>("Qt3DRender::QPickEvent*");
    }

    ~tst_QObjectPicker()
    {
        QMetaObject::invokeMethod(this, "_q_cleanup", Qt::DirectConnection);
    }

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        Qt3DRender::QObjectPicker picker;

        // THEN
        QCOMPARE(picker.priority(), 0);
        QCOMPARE(picker.isDragEnabled(), false);
        QCOMPARE(picker.isHoverEnabled(), false);
    }

    void checkCreationData()
    {
        // GIVEN
        Qt3DRender::QObjectPicker picker;

        picker.setPriority(1584);
        picker.setDragEnabled(true);
        picker.setHoverEnabled(true);

        // WHEN
        QVector<Qt3DCore::QNodeCreatedChangeBasePtr> creationChanges;

        {
            Qt3DCore::QNodeCreatedChangeGenerator creationChangeGenerator(&picker);
            creationChanges = creationChangeGenerator.creationChanges();
        }

        // THEN
        {
            QCOMPARE(creationChanges.size(), 1);

            const auto creationChangeData = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<Qt3DRender::QObjectPickerData>>(creationChanges.first());
            const Qt3DRender::QObjectPickerData cloneData = creationChangeData->data;

            QCOMPARE(cloneData.priority, 1584);
            QCOMPARE(cloneData.hoverEnabled, true);
            QCOMPARE(cloneData.dragEnabled, true);
            QCOMPARE(picker.id(), creationChangeData->subjectId());
            QCOMPARE(picker.isEnabled(), true);
            QCOMPARE(picker.isEnabled(), creationChangeData->isNodeEnabled());
            QCOMPARE(picker.metaObject(), creationChangeData->metaObject());
        }

        // WHEN
        picker.setEnabled(false);

        {
            Qt3DCore::QNodeCreatedChangeGenerator creationChangeGenerator(&picker);
            creationChanges = creationChangeGenerator.creationChanges();
        }

        // THEN
        {
            QCOMPARE(creationChanges.size(), 1);

            const auto creationChangeData = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<Qt3DRender::QObjectPickerData>>(creationChanges.first());
            const Qt3DRender::QObjectPickerData cloneData = creationChangeData->data;

            QCOMPARE(cloneData.priority, 1584);
            QCOMPARE(cloneData.hoverEnabled, true);
            QCOMPARE(cloneData.dragEnabled, true);
            QCOMPARE(picker.id(), creationChangeData->subjectId());
            QCOMPARE(picker.isEnabled(), false);
            QCOMPARE(picker.isEnabled(), creationChangeData->isNodeEnabled());
            QCOMPARE(picker.metaObject(), creationChangeData->metaObject());
        }
    }

    void checkPropertyUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QObjectPicker picker;
        arbiter.setArbiterOnNode(&picker);

        {
            {
                // WHEN
                picker.setPriority(883);
                QCoreApplication::processEvents();

                // THEN
                QCOMPARE(arbiter.events.size(), 0);
                QCOMPARE(arbiter.dirtyNodes.size(), 1);
                QCOMPARE(arbiter.dirtyNodes.front(), &picker);

                arbiter.dirtyNodes.clear();
            }

            {
                // WHEN
                picker.setPriority(883);
                QCoreApplication::processEvents();

                // THEN
                QCOMPARE(arbiter.events.size(), 0);
                QCOMPARE(arbiter.dirtyNodes.size(), 0);
            }
        }
        {
            {
                // WHEN
                picker.setDragEnabled(true);
                QCoreApplication::processEvents();

                // THEN
                QCOMPARE(arbiter.events.size(), 0);
                QCOMPARE(arbiter.dirtyNodes.size(), 1);
                QCOMPARE(arbiter.dirtyNodes.front(), &picker);

                arbiter.dirtyNodes.clear();
            }

            {
                // WHEN
                picker.setDragEnabled(true);
                QCoreApplication::processEvents();

                // THEN
                QCOMPARE(arbiter.events.size(), 0);
                QCOMPARE(arbiter.dirtyNodes.size(), 0);
            }
        }
        {
            {
                // WHEN
                picker.setHoverEnabled(true);
                QCoreApplication::processEvents();

                // THEN
                QCOMPARE(arbiter.events.size(), 0);
                QCOMPARE(arbiter.dirtyNodes.size(), 1);
                QCOMPARE(arbiter.dirtyNodes.front(), &picker);

                arbiter.dirtyNodes.clear();
            }

            {
                // WHEN
                picker.setHoverEnabled(true);
                QCoreApplication::processEvents();

                // THEN
                QCOMPARE(arbiter.events.size(), 0);
                QCOMPARE(arbiter.dirtyNodes.size(), 0);
            }
        }
    }

    void checkCloning_data()
    {
        QTest::addColumn<Qt3DRender::QObjectPicker *>("objectPicker");

        Qt3DRender::QObjectPicker *objectPicker = new Qt3DRender::QObjectPicker();
        QTest::newRow("empty objectPicker") << objectPicker;
        objectPicker = new Qt3DRender::QObjectPicker();
        objectPicker->setHoverEnabled(true);
        QTest::newRow("objectPicker_all_true") << objectPicker;
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DCore::QScene scene;
        QScopedPointer<Qt3DRender::QObjectPicker> objectPicker(new Qt3DRender::QObjectPicker());
        arbiter.setArbiterOnNode(objectPicker.data());
        Qt3DCore::QNodePrivate::get(objectPicker.data())->setScene(&scene);

        // WHEN
        objectPicker->setHoverEnabled(true);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 0);
        QCOMPARE(arbiter.dirtyNodes.size(), 1);
        QCOMPARE(arbiter.dirtyNodes.front(), objectPicker.data());

        arbiter.dirtyNodes.clear();
    }
};

QTEST_MAIN(tst_QObjectPicker)

#include "tst_qobjectpicker.moc"
