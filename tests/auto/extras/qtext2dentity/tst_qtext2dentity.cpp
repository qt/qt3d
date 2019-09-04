/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QTest>
#include <Qt3DExtras/QText2DEntity>
#include <Qt3DQuickExtras/qt3dquickwindow.h>
#include <Qt3DQuick/QQmlAspectEngine>
#include <Qt3DCore/private/qnode_p.h>

QVector<Qt3DCore::QNode *>lookupNodeByClassName(Qt3DCore::QNode *root, const QString &className)
{
    auto children = root->childNodes();
    QVector<Qt3DCore::QNode *> childrenList;
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
#if !defined(QT_OPENGL_ES_2_ANGLE)
    // GIVEN
    Qt3DExtras::Quick::Qt3DQuickWindow view;
    view.setSource(QUrl("qrc:/qtext2dentity.qml"));
    view.show();
    QCoreApplication::processEvents();

    // THEN
    auto rootEntity = view.engine()->aspectEngine()->rootEntity();
    QVERIFY(rootEntity != nullptr);
    auto atlases = lookupNodeByClassName(rootEntity.data(), "Qt3DExtras::QTextureAtlas");
    QVERIFY(atlases.length() == 1);
    auto atlas = atlases[0];
    QTRY_VERIFY(Qt3DCore::QNodePrivate::get(atlas)->m_changeArbiter);
#endif
}

QTEST_MAIN(tst_qtext2dentity)
#include "tst_qtext2dentity.moc"
