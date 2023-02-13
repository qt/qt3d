// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QtTest/QTest>
#include <Qt3DExtras/QText2DEntity>
#include <Qt3DQuickExtras/qt3dquickwindow.h>
#include <Qt3DQuick/QQmlAspectEngine>
#include <Qt3DCore/private/qnode_p.h>

QList<Qt3DCore::QNode *>lookupNodeByClassName(Qt3DCore::QNode *root, const QString &className)
{
    auto children = root->childNodes();
    QList<Qt3DCore::QNode *> childrenList;
    for (auto i : children) {
        if (i->metaObject()->className() == className)
            childrenList << i;
        else
            childrenList << lookupNodeByClassName(i, className);
    }
    return childrenList;
}

class tst_qtext2dentity : public QObject
{
    Q_OBJECT

private slots:
    void checkChangeArbiter();
};

void tst_qtext2dentity::checkChangeArbiter()
{
    QSKIP("Skipping for now as creating windows with RHI on cross compiled targets seems to fail");

    // GIVEN
    Qt3DExtras::Quick::Qt3DQuickWindow view;
    view.setSource(QUrl("qrc:/qtext2dentity.qml"));
    view.show();
    QCoreApplication::processEvents();

    // THEN
    auto rootEntity = view.engine()->aspectEngine()->rootEntity();
    QVERIFY(rootEntity != nullptr);
    auto atlases = lookupNodeByClassName(rootEntity.data(), "Qt3DExtras::QTextureAtlas");
    QVERIFY(atlases.size() == 1);
    auto atlas = atlases[0];
    QVERIFY(Qt3DCore::QNodePrivate::get(atlas)->m_changeArbiter);
}

QTEST_MAIN(tst_qtext2dentity)
#include "tst_qtext2dentity.moc"
