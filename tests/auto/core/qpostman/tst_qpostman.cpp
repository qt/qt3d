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
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qpropertyupdatedchangebase_p.h>
#include "testpostmanarbiter.h"

using namespace Qt3DCore;

namespace {

class NodeChangeReceiver: public QNode
{
public:
    NodeChangeReceiver(QNode *parent = nullptr)
        : QNode(parent)
        , m_hasReceivedChange(false)
    {}

    inline bool hasReceivedChange() const { return m_hasReceivedChange; }

protected:
    void sceneChangeEvent(const QSceneChangePtr &) override
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
        QPostman postman;

        // THEN
        QVERIFY(QPostmanPrivate::get(&postman)->m_scene == nullptr);

        // WHEN
        QScene scene;
        postman.setScene(&scene);

        // THEN
        QCOMPARE(QPostmanPrivate::get(&postman)->m_scene, &scene);
    }

};

QTEST_MAIN(tst_QPostman)

#include "tst_qpostman.moc"
