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
#include <Qt3DCore/QAspectFactory>
#include <Qt3DCore/QAbstractAspect>

using namespace QT_PREPEND_NAMESPACE(Qt3D);

#define FAKE_ASPECT(ClassName) \
class ClassName : public QAbstractAspect \
{ \
    Q_OBJECT \
public: \
    explicit ClassName(QObject *parent = 0) \
        : QAbstractAspect(QAbstractAspect::AspectOther, parent) {} \
\
private: \
    void setRootEntity(QEntity *) Q_DECL_OVERRIDE {} \
    void onInitialize(const QVariantMap &) Q_DECL_OVERRIDE {} \
    void onStartup() Q_DECL_OVERRIDE {} \
    void onShutdown() Q_DECL_OVERRIDE {} \
    void onCleanup() Q_DECL_OVERRIDE {} \
    void sceneNodeAdded(QSceneChangePtr &) Q_DECL_OVERRIDE {} \
    void sceneNodeRemoved(QSceneChangePtr &) Q_DECL_OVERRIDE {} \
\
    QVector<QAspectJobPtr> jobsToExecute(qint64) Q_DECL_OVERRIDE \
    { \
        return QVector<QAspectJobPtr>(); \
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
        QCOMPARE(factory.availableFactories().size(), 1);
        QCOMPARE(factory.availableFactories().first(), QString("default"));

        // WHEN
        QAbstractAspect *aspect = factory.createAspect(QStringLiteral("default"));

        // THEN
        QVERIFY(qobject_cast<DefaultFakeAspect*>(aspect) != Q_NULLPTR);
        QVERIFY(aspect->parent() == Q_NULLPTR);
    }

    void shouldRegisterFactories()
    {
        // GIVEN
        QAspectFactory factory;

        // WHEN
        factory.addFactory(QStringLiteral("another"),
                           QAspectFactory::functionHelper<AnotherFakeAspect>);

        // THEN
        QCOMPARE(factory.availableFactories().size(), 2);
        QVERIFY(factory.availableFactories().contains(QStringLiteral("another")));

        // WHEN
        QAbstractAspect *aspect = factory.createAspect(QStringLiteral("another"), this);

        // THEN
        QVERIFY(qobject_cast<AnotherFakeAspect*>(aspect) != Q_NULLPTR);
        QCOMPARE(aspect->parent(), this);
    }

    void shouldGracefulyHandleMissingFactories()
    {
        // GIVEN
        QAspectFactory factory;

        // WHEN
        QAbstractAspect *aspect = factory.createAspect(QStringLiteral("missing"), this);

        // THEN
        QVERIFY(qobject_cast<AnotherFakeAspect*>(aspect) == Q_NULLPTR);
    }
};

QTEST_MAIN(tst_QAspectFactory)

#include "tst_qaspectfactory.moc"
