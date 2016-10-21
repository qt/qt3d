/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
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
#include <Qt3DRender/qrenderqmltotexture.h>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/private/qrenderqmltotexture_p.h>
#include <QObject>
#include <QSignalSpy>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/private/qnodecreatedchangegenerator_p.h>
#include <Qt3DCore/qnodecreatedchange.h>
#include "testpostmanarbiter.h"

class tst_QRenderQmlToTexture : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QRenderQmlToTexture renderQmlToTexture;

        // THEN
        QCOMPARE(renderQmlToTexture.texture(), nullptr);
        QCOMPARE(renderQmlToTexture.source(), QUrl(QStringLiteral("")));
        QCOMPARE(renderQmlToTexture.renderOnce(), false);
        QCOMPARE(renderQmlToTexture.loaded(), false);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::QRenderQmlToTexture renderQmlToTexture;
        QScopedPointer<Qt3DRender::QTexture2D> texture(new Qt3DRender::QTexture2D());

        {
            // WHEN
            QSignalSpy spy(&renderQmlToTexture, SIGNAL(textureChanged(Qt3DRender::QAbstractTexture *)));
            Qt3DRender::QAbstractTexture * newValue = texture.data();
            renderQmlToTexture.setTexture(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(renderQmlToTexture.texture(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            renderQmlToTexture.setTexture(newValue);

            // THEN
            QCOMPARE(renderQmlToTexture.texture(), newValue);
            QCOMPARE(spy.count(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&renderQmlToTexture, SIGNAL(sourceChanged(QUrl)));
            const QUrl newValue = QUrl(QStringLiteral("qrc://source.qml"));
            renderQmlToTexture.setSource(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(renderQmlToTexture.source(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            renderQmlToTexture.setSource(newValue);

            // THEN
            QCOMPARE(renderQmlToTexture.source(), newValue);
            QCOMPARE(spy.count(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&renderQmlToTexture, SIGNAL(renderOnceChanged(bool)));
            const bool newValue = true;
            renderQmlToTexture.setRenderOnce(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(renderQmlToTexture.renderOnce(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            renderQmlToTexture.setRenderOnce(newValue);

            // THEN
            QCOMPARE(renderQmlToTexture.renderOnce(), newValue);
            QCOMPARE(spy.count(), 0);
        }
    }

    void checkCreationData()
    {
        // GIVEN
        Qt3DRender::QRenderQmlToTexture renderQmlToTexture;
        QScopedPointer<Qt3DRender::QTexture2D> texture(new Qt3DRender::QTexture2D());

        renderQmlToTexture.setTexture(texture.data());
        renderQmlToTexture.setSource(QUrl(QStringLiteral("qrc://source.qml")));
        renderQmlToTexture.setRenderOnce(true);

        // WHEN
        QVector<Qt3DCore::QNodeCreatedChangeBasePtr> creationChanges;

        {
            Qt3DCore::QNodeCreatedChangeGenerator creationChangeGenerator(&renderQmlToTexture);
            creationChanges = creationChangeGenerator.creationChanges();
        }

        // THEN
        {
            QCOMPARE(creationChanges.size(), 1);

            const auto creationChangeData = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<Qt3DRender::QRenderQmlToTextureData>>(creationChanges.first());
            const Qt3DRender::QRenderQmlToTextureData cloneData = creationChangeData->data;

            QCOMPARE(renderQmlToTexture.texture()->id(), cloneData.textureId);
            QCOMPARE(renderQmlToTexture.renderOnce(), cloneData.renderOnce);
            QCOMPARE(renderQmlToTexture.id(), creationChangeData->subjectId());
            QCOMPARE(renderQmlToTexture.isEnabled(), true);
            QCOMPARE(renderQmlToTexture.isEnabled(), creationChangeData->isNodeEnabled());
            QCOMPARE(renderQmlToTexture.metaObject(), creationChangeData->metaObject());
        }

        // WHEN
        renderQmlToTexture.setEnabled(false);

        {
            Qt3DCore::QNodeCreatedChangeGenerator creationChangeGenerator(&renderQmlToTexture);
            creationChanges = creationChangeGenerator.creationChanges();
        }

        // THEN
        {
            QCOMPARE(creationChanges.size(), 1);

            const auto creationChangeData = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<Qt3DRender::QRenderQmlToTextureData>>(creationChanges.first());
            const Qt3DRender::QRenderQmlToTextureData cloneData = creationChangeData->data;

            QCOMPARE(renderQmlToTexture.texture()->id(), cloneData.textureId);
            QCOMPARE(renderQmlToTexture.renderOnce(), cloneData.renderOnce);
            QCOMPARE(renderQmlToTexture.id(), creationChangeData->subjectId());
            QCOMPARE(renderQmlToTexture.isEnabled(), false);
            QCOMPARE(renderQmlToTexture.isEnabled(), creationChangeData->isNodeEnabled());
            QCOMPARE(renderQmlToTexture.metaObject(), creationChangeData->metaObject());
        }
    }

    void checkTextureUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QRenderQmlToTexture renderQmlToTexture;
        arbiter.setArbiterOnNode(&renderQmlToTexture);
        QScopedPointer<Qt3DRender::QTexture2D> texture(new Qt3DRender::QTexture2D());

        {
            // WHEN
            renderQmlToTexture.setTexture(texture.data());
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 1);
            auto change = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
            QCOMPARE(change->propertyName(), "texture");
            QCOMPARE(change->value().value<Qt3DRender::QAbstractTexture *>(), renderQmlToTexture.texture());
            QCOMPARE(change->type(), Qt3DCore::PropertyUpdated);

            arbiter.events.clear();
        }

        {
            // WHEN
            renderQmlToTexture.setTexture(texture.data());
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
        }

    }

    void checkSourceUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QRenderQmlToTexture renderQmlToTexture;
        arbiter.setArbiterOnNode(&renderQmlToTexture);

        {
            // WHEN
            renderQmlToTexture.setSource(QUrl(QStringLiteral("qrc://source.qml")));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 1);
            auto change = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
            QCOMPARE(change->propertyName(), "source");
            QCOMPARE(change->value().value<QUrl>(), renderQmlToTexture.source());
            QCOMPARE(change->type(), Qt3DCore::PropertyUpdated);

            arbiter.events.clear();
        }

        {
            // WHEN
            renderQmlToTexture.setSource(QUrl(QStringLiteral("qrc://source.qml")));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
        }

    }

    void checkRenderOnceUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QRenderQmlToTexture renderQmlToTexture;
        arbiter.setArbiterOnNode(&renderQmlToTexture);

        {
            // WHEN
            renderQmlToTexture.setRenderOnce(true);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 1);
            auto change = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
            QCOMPARE(change->propertyName(), "renderOnce");
            QCOMPARE(change->value().value<bool>(), renderQmlToTexture.renderOnce());
            QCOMPARE(change->type(), Qt3DCore::PropertyUpdated);

            arbiter.events.clear();
        }

        {
            // WHEN
            renderQmlToTexture.setRenderOnce(true);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
        }

    }

};

QTEST_MAIN(tst_QRenderQmlToTexture)

#include "tst_qrenderqmltotexture.moc"
