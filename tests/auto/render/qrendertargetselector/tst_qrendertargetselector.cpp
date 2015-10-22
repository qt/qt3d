/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include <QtTest/QTest>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/qentity.h>

#include <Qt3DRender/qrendertargetselector.h>
#include <Qt3DRender/qrendertarget.h>
#include "testpostmanarbiter.h"

// We need to call QNode::clone which is protected
// So we sublcass QNode instead of QObject
class tst_QRenderTargetSelector: public Qt3DCore::QNode
{
    Q_OBJECT
public:
    ~tst_QRenderTargetSelector()
    {
        QNode::cleanup();
    }

private Q_SLOTS:

    void checkSaneDefaults()
    {
        QScopedPointer<Qt3DRender::QRenderTargetSelector> defaultTargetSelector(new Qt3DRender::QRenderTargetSelector);

        QVERIFY(defaultTargetSelector->target() == Q_NULLPTR);
        QCOMPARE(defaultTargetSelector->drawBuffers().count(), 0);
    }

    void checkCloning_data()
    {
        QTest::addColumn<Qt3DRender::QRenderTargetSelector *>("renderTargetSelector");
        QTest::addColumn<QList<Qt3DRender::QRenderAttachment::RenderAttachmentType> >("drawBuffers");
        QTest::addColumn<Qt3DRender::QRenderTarget * >("target");

        Qt3DRender::QRenderTargetSelector *defaultConstructed = new Qt3DRender::QRenderTargetSelector();
        QTest::newRow("defaultConstructed") << defaultConstructed << QList<Qt3DRender::QRenderAttachment::RenderAttachmentType>() << static_cast<Qt3DRender::QRenderTarget *>(Q_NULLPTR);

        Qt3DRender::QRenderTargetSelector *renderTargetSelectorWithTarget = new Qt3DRender::QRenderTargetSelector();
        Qt3DRender::QRenderTarget *target1 = new Qt3DRender::QRenderTarget();
        renderTargetSelectorWithTarget->setTarget(target1);
        QTest::newRow("renderTargetSelectorWithTarget") << renderTargetSelectorWithTarget << QList<Qt3DRender::QRenderAttachment::RenderAttachmentType>() << target1;

        Qt3DRender::QRenderTargetSelector *renderTargetSelectorWithTargetAndBuffers = new Qt3DRender::QRenderTargetSelector();
        Qt3DRender::QRenderTarget *target2 = new Qt3DRender::QRenderTarget();
        renderTargetSelectorWithTargetAndBuffers->setTarget(target2);
        QList<Qt3DRender::QRenderAttachment::RenderAttachmentType> attachmentTypes = QList<Qt3DRender::QRenderAttachment::RenderAttachmentType>() << Qt3DRender::QRenderAttachment::ColorAttachment0 << Qt3DRender::QRenderAttachment::DepthAttachment;
        renderTargetSelectorWithTargetAndBuffers->setDrawBuffers(attachmentTypes);
        QTest::newRow("renderTargetSelectorWithTargetAndDrawBuffers") << renderTargetSelectorWithTargetAndBuffers << attachmentTypes << target2;
    }

    void checkCloning()
    {
        // GIVEN
        QFETCH(Qt3DRender::QRenderTargetSelector*, renderTargetSelector);
        QFETCH(QList<Qt3DRender::QRenderAttachment::RenderAttachmentType>, drawBuffers);
        QFETCH(Qt3DRender::QRenderTarget *, target);

        // THEN
        QCOMPARE(renderTargetSelector->drawBuffers(), drawBuffers);
        QCOMPARE(renderTargetSelector->target(), target);

        // WHEN
        Qt3DRender::QRenderTargetSelector *clone = static_cast<Qt3DRender::QRenderTargetSelector *>(QNode::clone(renderTargetSelector));

        // THEN
        QVERIFY(clone != Q_NULLPTR);
        QCOMPARE(renderTargetSelector->id(), clone->id());

        QCOMPARE(renderTargetSelector->drawBuffers(), clone->drawBuffers());

        if (renderTargetSelector->target() != Q_NULLPTR) {
            QVERIFY(clone->target() != Q_NULLPTR);
            QCOMPARE(clone->target()->id(), renderTargetSelector->target()->id());
        }

        delete renderTargetSelector;
        delete clone;
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QRenderTargetSelector> renderTargetSelector(new Qt3DRender::QRenderTargetSelector());
        TestArbiter arbiter(renderTargetSelector.data());

        // WHEN
        Qt3DRender::QRenderTarget *target = new Qt3DRender::QRenderTarget();
        renderTargetSelector->setTarget(target);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        Qt3DCore::QScenePropertyChangePtr change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "target");
        QCOMPARE(change->subjectId(), renderTargetSelector->id());
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), target->id());
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        renderTargetSelector->setTarget(target);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 0);

        // WHEN
        renderTargetSelector->setTarget(Q_NULLPTR);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "target");
        QCOMPARE(change->subjectId(), renderTargetSelector->id());
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), Qt3DCore::QNodeId());
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        QList<Qt3DRender::QRenderAttachment::RenderAttachmentType> drawBuffers;
        drawBuffers << Qt3DRender::QRenderAttachment::ColorAttachment0 << Qt3DRender::QRenderAttachment::DepthAttachment;
        renderTargetSelector->setDrawBuffers(drawBuffers);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "drawBuffers");
        QCOMPARE(change->subjectId(), renderTargetSelector->id());
        QCOMPARE(change->value().value<QList<Qt3DRender::QRenderAttachment::RenderAttachmentType> >(), drawBuffers);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        renderTargetSelector->setDrawBuffers(drawBuffers);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 0);

        // WHEN
        renderTargetSelector->setDrawBuffers(QList<Qt3DRender::QRenderAttachment::RenderAttachmentType>());
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "drawBuffers");
        QCOMPARE(change->subjectId(), renderTargetSelector->id());
        QCOMPARE(change->value().value<QList<Qt3DRender::QRenderAttachment::RenderAttachmentType> >(), QList<Qt3DRender::QRenderAttachment::RenderAttachmentType>());
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);


        arbiter.events.clear();
    }

protected:
    Qt3DCore::QNode *doClone() const Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }

};

QTEST_MAIN(tst_QRenderTargetSelector)

#include "tst_qrendertargetselector.moc"
