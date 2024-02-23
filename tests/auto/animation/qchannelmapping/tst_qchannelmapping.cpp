// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DAnimation/qchannelmapping.h>
#include <Qt3DAnimation/private/qabstractchannelmapping_p.h>
#include <Qt3DAnimation/private/qchannelmapping_p.h>
#include <Qt3DCore/qentity.h>
#include <QObject>
#include <QSignalSpy>
#include <QQuaternion>
#include <testarbiter.h>

class tst_QTargetEntity : public Qt3DCore::QEntity
{
    Q_OBJECT
    Q_PROPERTY(QQuaternion rotation MEMBER m_rotation NOTIFY rotationChanged)
    Q_PROPERTY(QVector3D translation MEMBER m_translation NOTIFY translationChanged)
    Q_PROPERTY(QVector3D scale MEMBER m_scale NOTIFY scaleChanged)
    Q_PROPERTY(float floatProperty MEMBER m_floatProperty NOTIFY floatPropertyChanged)
    Q_PROPERTY(QVector2D vec2Property MEMBER m_vec2Property NOTIFY vec2PropertyChanged)
    Q_PROPERTY(QVector3D vec3Property MEMBER m_vec3Property NOTIFY vec3PropertyChanged)
    Q_PROPERTY(QVector4D vec4Property MEMBER m_vec4Property NOTIFY vec4PropertyChanged)
    Q_PROPERTY(QQuaternion quaternionProperty MEMBER m_quaternionProperty NOTIFY quaternionPropertyChanged)
    Q_PROPERTY(QVariantList listProperty MEMBER m_listProperty NOTIFY listPropertyChanged)
    Q_PROPERTY(QList<float> vecProperty MEMBER m_vecProperty NOTIFY vecPropertyChanged)

signals:
    void rotationChanged();
    void translationChanged();
    void scaleChanged();
    void floatPropertyChanged();
    void vec2PropertyChanged();
    void vec3PropertyChanged();
    void vec4PropertyChanged();
    void quaternionPropertyChanged();
    void listPropertyChanged();
    void vecPropertyChanged();

private:
    QQuaternion m_rotation;
    QVector3D m_translation;
    QVector3D m_scale;
    float m_floatProperty;
    QVector2D m_vec2Property;
    QVector3D m_vec3Property;
    QVector4D m_vec4Property;
    QQuaternion m_quaternionProperty;
    QVariantList m_listProperty;
    QList<float> m_vecProperty;
};


class tst_QChannelMapping : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DAnimation::QChannelMapping mapping;

        // THEN
        QCOMPARE(mapping.channelName(), QString());
        QCOMPARE(mapping.target(), static_cast<Qt3DCore::QNode *>(nullptr));
        QCOMPARE(mapping.property(), QString());

        const Qt3DAnimation::QChannelMappingPrivate *d =
            static_cast<const Qt3DAnimation::QChannelMappingPrivate *>(
                Qt3DAnimation::QChannelMappingPrivate::get(&mapping));

        QCOMPARE(d->m_type, static_cast<int>(QMetaType::UnknownType));
        QCOMPARE(d->m_componentCount, 0);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DAnimation::QChannelMapping mapping;

        {
            // WHEN
            QSignalSpy spy(&mapping, SIGNAL(channelNameChanged(QString)));
            const QString newValue(QStringLiteral("Rotation"));
            mapping.setChannelName(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(mapping.channelName(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            mapping.setChannelName(newValue);

            // THEN
            QCOMPARE(mapping.channelName(), newValue);
            QCOMPARE(spy.size(), 0);
        }

        {
            // WHEN
            QSignalSpy spy(&mapping, SIGNAL(targetChanged(Qt3DCore::QNode*)));
            auto newValue = new Qt3DCore::QEntity();
            mapping.setTarget(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(mapping.target(), newValue);
            QCOMPARE(newValue->parent(), &mapping);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            mapping.setTarget(newValue);

            // THEN
            QCOMPARE(mapping.target(), newValue);
            QCOMPARE(spy.size(), 0);
        }

        {
            // WHEN
            QSignalSpy spy(&mapping, SIGNAL(propertyChanged(QString)));
            const QString newValue(QStringLiteral("rotation"));
            mapping.setProperty(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(mapping.property(), newValue);
            QCOMPARE(spy.size(), 1);

            // WHEN
            spy.clear();
            mapping.setProperty(newValue);

            // THEN
            QCOMPARE(mapping.property(), newValue);
            QCOMPARE(spy.size(), 0);
        }
    }

    void checkPropertyUpdateChanges()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DAnimation::QChannelMapping mapping;
        QScopedPointer<Qt3DCore::QEntity> target(new tst_QTargetEntity());
        arbiter.setArbiterOnNode(&mapping);

        {
            // WHEN
            mapping.setChannelName(QStringLiteral("Scale"));

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &mapping);

            arbiter.clear();

            // WHEN
            mapping.setChannelName(QStringLiteral("Scale"));

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

        {
            // WHEN
            mapping.setTarget(target.data());

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &mapping);

            arbiter.clear();

            // WHEN
            mapping.setTarget(target.data());

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }

        {
            // WHEN
            target->setProperty("scale", QVector3D(1.0f, 0.0f, 0.0f));
            mapping.setProperty(QStringLiteral("scale"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), &mapping);

            arbiter.clear();

            // WHEN
            mapping.setProperty(QStringLiteral("scale"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 0);
        }
    }

    void checkPropertyUpdateNameTypeAndComponentCount_data()
    {
        QTest::addColumn<QByteArray>("propertyName");
        QTest::addColumn<QVariant>("value");
        QTest::addColumn<int>("expectedType");
        QTest::addColumn<int>("expectedComponentCount");

        QTest::newRow("float") << QByteArrayLiteral("floatProperty") << QVariant(1.0f) << static_cast<int>(QMetaType::Float) << 1;
        QTest::newRow("vec2") << QByteArrayLiteral("vec2Property") << QVariant(QVector2D(1.0f, 1.0f)) << static_cast<int>(QMetaType::QVector2D) << 2;
        QTest::newRow("vec3") << QByteArrayLiteral("vec3Property") << QVariant(QVector3D(1.0f, 1.0f, 1.0f)) << static_cast<int>(QMetaType::QVector3D) << 3;
        QTest::newRow("vec4") << QByteArrayLiteral("vec4Property") << QVariant(QVector4D(1.0f, 1.0f, 1.0f, 1.0f)) << static_cast<int>(QMetaType::QVector4D) << 4;
        QTest::newRow("quaternion") << QByteArrayLiteral("quaternionProperty") << QVariant(QQuaternion(1.0f, 1.0f, 1.0f, 1.0f)) << static_cast<int>(QMetaType::QQuaternion) << 4;

        QVariantList list = QVariantList() << QVariant(1.0f) << QVariant(1.0) << QVariant(1.0f) << QVariant(1.0f) << QVariant(1.0f);
        QTest::newRow("variantlist") << QByteArrayLiteral("listProperty") << QVariant::fromValue(list) << static_cast<int>(QMetaType::QVariantList) << 5;

        QVector<float> vec(8);
        QTest::newRow("vector") << QByteArrayLiteral("vecProperty") << QVariant::fromValue(vec) << qMetaTypeId<decltype(vec)>() << 8;
    }

    void checkPropertyUpdateNameTypeAndComponentCount()
    {
        // GIVEN
        QFETCH(QByteArray, propertyName);
        QFETCH(QVariant, value);
        QFETCH(int, expectedType);
        QFETCH(int, expectedComponentCount);

        Q_UNUSED(expectedType);
        Q_UNUSED(expectedComponentCount);

        TestArbiter arbiter;
        Qt3DAnimation::QChannelMapping mapping;
        QScopedPointer<Qt3DCore::QEntity> target(new tst_QTargetEntity());
        mapping.setTarget(target.data());
        arbiter.setArbiterOnNode(&mapping);

        {
            // WHEN
            target->setProperty(propertyName.constData(), value);

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
            QCOMPARE(arbiter.dirtyNodes().front(), target.data());

            arbiter.clear();

            // THEN
            mapping.setProperty(QString::fromLatin1(propertyName));

            // THEN
            QCOMPARE(arbiter.dirtyNodes().size(), 1);
        }
    }

};

QTEST_MAIN(tst_QChannelMapping)

#include "tst_qchannelmapping.moc"
