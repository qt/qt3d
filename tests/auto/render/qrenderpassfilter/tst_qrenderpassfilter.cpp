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
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/qentity.h>

#include <Qt3DRender/qrenderpassfilter.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qfilterkey.h>

#include "testpostmanarbiter.h"

// We need to call QNode::clone which is protected
// So we sublcass QNode instead of QObject
class tst_QRenderPassFilter: public Qt3DCore::QNode
{
    Q_OBJECT
public:
    ~tst_QRenderPassFilter()
    {
        QNode::cleanup();
    }

private Q_SLOTS:

    void checkSaneDefaults()
    {
        QScopedPointer<Qt3DRender::QRenderPassFilter> defaultRenderPassFilter(new Qt3DRender::QRenderPassFilter);

        QCOMPARE(defaultRenderPassFilter->matchAny().count(), 0);
        QCOMPARE(defaultRenderPassFilter->parameters().count(), 0);
    }

    void checkCloning_data()
    {
        QTest::addColumn<Qt3DRender::QRenderPassFilter *>("renderPassFilter");
        QTest::addColumn<QList<Qt3DRender::QParameter *> >("parameters");
        QTest::addColumn<QList<Qt3DRender::QFilterKey *> >("filterKeys");

        Qt3DRender::QRenderPassFilter *defaultConstructed = new Qt3DRender::QRenderPassFilter();
        QTest::newRow("defaultConstructed") << defaultConstructed << QList<Qt3DRender::QParameter *>() << QList<Qt3DRender::QFilterKey *>();

        Qt3DRender::QRenderPassFilter *renderPassFilterWithParams = new Qt3DRender::QRenderPassFilter();
        Qt3DRender::QParameter *parameter1 = new Qt3DRender::QParameter(QStringLiteral("displacement"), 454.0f);
        Qt3DRender::QParameter *parameter2 = new Qt3DRender::QParameter(QStringLiteral("torque"), 650);
        QList<Qt3DRender::QParameter *> params1 = QList<Qt3DRender::QParameter *>() << parameter1 << parameter2;
        renderPassFilterWithParams->addParameter(parameter1);
        renderPassFilterWithParams->addParameter(parameter2);
        QTest::newRow("renderPassFilterWithParams") << renderPassFilterWithParams << params1 << QList<Qt3DRender::QFilterKey *>();

        Qt3DRender::QRenderPassFilter *renderPassFilterWithAnnotations = new Qt3DRender::QRenderPassFilter();
        Qt3DRender::QFilterKey *filterKey1 = new Qt3DRender::QFilterKey();
        Qt3DRender::QFilterKey *filterKey2 = new Qt3DRender::QFilterKey();
        filterKey1->setName(QStringLiteral("hasSuperCharger"));
        filterKey1->setValue(true);
        filterKey1->setName(QStringLiteral("hasNitroKit"));
        filterKey1->setValue(false);
        QList<Qt3DRender::QFilterKey *> filterKeys1 = QList<Qt3DRender::QFilterKey *>() << filterKey1 << filterKey2;
        renderPassFilterWithAnnotations->addMatch(filterKey1);
        renderPassFilterWithAnnotations->addMatch(filterKey2);
        QTest::newRow("renderPassFilterWithAnnotations") << renderPassFilterWithAnnotations << QList<Qt3DRender::QParameter *>() << filterKeys1;

        Qt3DRender::QRenderPassFilter *renderPassFilterWithParamsAndAnnotations = new Qt3DRender::QRenderPassFilter();
        Qt3DRender::QParameter *parameter3 = new Qt3DRender::QParameter(QStringLiteral("displacement"), 383.0f);
        Qt3DRender::QParameter *parameter4 = new Qt3DRender::QParameter(QStringLiteral("torque"), 555);
        Qt3DRender::QFilterKey *filterKey3 = new Qt3DRender::QFilterKey();
        Qt3DRender::QFilterKey *filterKey4 = new Qt3DRender::QFilterKey();
        filterKey3->setName(QStringLiteral("hasSuperCharger"));
        filterKey3->setValue(false);
        filterKey4->setName(QStringLiteral("hasNitroKit"));
        filterKey4->setValue(true);
        QList<Qt3DRender::QParameter *> params2 = QList<Qt3DRender::QParameter *>() << parameter3 << parameter4;
        QList<Qt3DRender::QFilterKey *> filterKeys2 = QList<Qt3DRender::QFilterKey *>() << filterKey3 << filterKey4;
        renderPassFilterWithParamsAndAnnotations->addParameter(parameter3);
        renderPassFilterWithParamsAndAnnotations->addParameter(parameter4);
        renderPassFilterWithParamsAndAnnotations->addMatch(filterKey3);
        renderPassFilterWithParamsAndAnnotations->addMatch(filterKey4);
        QTest::newRow("renderPassFilterWithParamsAndAnnotations") << renderPassFilterWithParamsAndAnnotations << params2 << filterKeys2 ;
    }

    void checkCloning()
    {
        // GIVEN
        QFETCH(Qt3DRender::QRenderPassFilter*, renderPassFilter);
        QFETCH(QList<Qt3DRender::QParameter *>, parameters);
        QFETCH(QList<Qt3DRender::QFilterKey *>, filterKeys);

        // THEN
        QCOMPARE(renderPassFilter->parameters(), parameters);
        QCOMPARE(renderPassFilter->matchAny(), filterKeys);

        // WHEN
        Qt3DRender::QRenderPassFilter *clone = static_cast<Qt3DRender::QRenderPassFilter *>(QNode::clone(renderPassFilter));

        // THEN
        QVERIFY(clone != Q_NULLPTR);
        QCOMPARE(renderPassFilter->id(), clone->id());

        QCOMPARE(renderPassFilter->matchAny().count(), clone->matchAny().count());
        QCOMPARE(renderPassFilter->parameters().count(), clone->parameters().count());

        for (int i = 0, m = parameters.count(); i < m; ++i) {
            Qt3DRender::QParameter *pClone = clone->parameters().at(i);
            Qt3DRender::QParameter *pOrig = parameters.at(i);
            QCOMPARE(pOrig->id(),pClone->id());
            QCOMPARE(pOrig->name(), pClone->name());
            QCOMPARE(pOrig->value(), pClone->value());
            QVERIFY(pClone->parent() == clone);
            QVERIFY(pOrig->parent() == renderPassFilter);
        }

        for (int i = 0, m = filterKeys.count(); i < m; ++i) {
            Qt3DRender::QFilterKey *aClone = clone->matchAny().at(i);
            Qt3DRender::QFilterKey *aOrig = filterKeys.at(i);
            QCOMPARE(aOrig->id(),aClone->id());
            QCOMPARE(aOrig->name(), aClone->name());
            QCOMPARE(aOrig->value(), aClone->value());
            QVERIFY(aClone->parent() == clone);
            QVERIFY(aOrig->parent() == renderPassFilter);
        }

        delete renderPassFilter;
        delete clone;
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QRenderPassFilter> renderPassFilter(new Qt3DRender::QRenderPassFilter());
        TestArbiter arbiter(renderPassFilter.data());

        // WHEN
        Qt3DRender::QParameter *param1 = new Qt3DRender::QParameter();
        renderPassFilter->addParameter(param1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        Qt3DCore::QScenePropertyChangePtr change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "parameter");
        QCOMPARE(change->subjectId(),renderPassFilter->id());
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), param1->id());
        QCOMPARE(change->type(), Qt3DCore::NodeAdded);

        arbiter.events.clear();

        // WHEN
        renderPassFilter->addParameter(param1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 0);

        // WHEN
        renderPassFilter->removeParameter(param1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "parameter");
        QCOMPARE(change->subjectId(), renderPassFilter->id());
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), param1->id());
        QCOMPARE(change->type(), Qt3DCore::NodeRemoved);

        arbiter.events.clear();

        // WHEN
        Qt3DRender::QFilterKey *filterKey1 = new Qt3DRender::QFilterKey();
        renderPassFilter->addMatch(filterKey1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "match");
        QCOMPARE(change->subjectId(),renderPassFilter->id());
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), filterKey1->id());
        QCOMPARE(change->type(), Qt3DCore::NodeAdded);

        arbiter.events.clear();

        // WHEN
        renderPassFilter->addMatch(filterKey1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 0);

        // WHEN
        renderPassFilter->removeMatch(filterKey1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "match");
        QCOMPARE(change->subjectId(), renderPassFilter->id());
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), filterKey1->id());
        QCOMPARE(change->type(), Qt3DCore::NodeRemoved);

        arbiter.events.clear();
    }

protected:
    Qt3DCore::QNode *doClone() const Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }

};

QTEST_MAIN(tst_QRenderPassFilter)

#include "tst_qrenderpassfilter.moc"
