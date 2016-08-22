/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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
#include <qbackendnodetester.h>

#include <Qt3DCore/QPropertyUpdatedChange>
#include <Qt3DCore/QPropertyNodeAddedChange>
#include <Qt3DCore/QPropertyNodeRemovedChange>
#include <Qt3DInput/private/inputchord_p.h>
#include <Qt3DInput/QActionInput>
#include <Qt3DInput/QInputChord>

class tst_InputChord : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT
private Q_SLOTS:
    void shouldMirrorPeerProperties()
    {
        // GIVEN
        Qt3DInput::Input::InputChord backendInputChord;
        Qt3DInput::QInputChord inputChord;
        Qt3DInput::QActionInput actionInput;

        inputChord.setTimeout(250);
        inputChord.addChord(&actionInput);

        // WHEN
        simulateInitialization(&inputChord, &backendInputChord);

        // THEN
        QCOMPARE(backendInputChord.peerId(), inputChord.id());
        QCOMPARE(backendInputChord.isEnabled(), inputChord.isEnabled());
        QCOMPARE(backendInputChord.timeout(), inputChord.timeout());
        QCOMPARE(backendInputChord.chords().size(), inputChord.chords().size());

        const int inputsCount = backendInputChord.chords().size();
        if (inputsCount > 0) {
            for (int i = 0; i < inputsCount; ++i)
                QCOMPARE(backendInputChord.chords().at(i), inputChord.chords().at(i)->id());
        }
    }

    void shouldHaveInitialAndCleanedUpStates()
    {
        // GIVEN
        Qt3DInput::Input::InputChord backendInputChord;

        // THEN
        QVERIFY(backendInputChord.peerId().isNull());
        QCOMPARE(backendInputChord.isEnabled(), false);
        QCOMPARE(backendInputChord.timeout(), 0);
        QCOMPARE(backendInputChord.chords().size(), 0);

        // GIVEN
        Qt3DInput::QInputChord inputChord;
        Qt3DInput::QActionInput actionInput;

        inputChord.setTimeout(250);
        inputChord.addChord(&actionInput);

        // WHEN
        simulateInitialization(&inputChord, &backendInputChord);
        backendInputChord.cleanup();

        // THEN
        QCOMPARE(backendInputChord.isEnabled(), false);
        QCOMPARE(backendInputChord.timeout(), 0);
        QCOMPARE(backendInputChord.chords().size(), 0);
    }

    void shouldHandlePropertyChanges()
    {
        // GIVEN
        Qt3DInput::Input::InputChord backendInputChord;

        // WHEN
        Qt3DCore::QPropertyUpdatedChangePtr updateChange(new Qt3DCore::QPropertyUpdatedChange(Qt3DCore::QNodeId()));
        updateChange->setValue(250);
        updateChange->setPropertyName("timeout");
        backendInputChord.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(backendInputChord.timeout(), 250);

        // WHEN
        updateChange.reset(new Qt3DCore::QPropertyUpdatedChange(Qt3DCore::QNodeId()));
        updateChange->setPropertyName("enabled");
        updateChange->setValue(true);
        backendInputChord.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(backendInputChord.isEnabled(), true);

        // WHEN
        Qt3DInput::QActionInput input;
        const Qt3DCore::QNodeId inputId = input.id();
        const auto nodeAddedChange = Qt3DCore::QPropertyNodeAddedChangePtr::create(Qt3DCore::QNodeId(), &input);
        nodeAddedChange->setPropertyName("chord");
        backendInputChord.sceneChangeEvent(nodeAddedChange);

        // THEN
        QCOMPARE(backendInputChord.chords().size(), 1);
        QCOMPARE(backendInputChord.chords().first(), inputId);

        // WHEN
        const auto nodeRemovedChange = Qt3DCore::QPropertyNodeRemovedChangePtr::create(Qt3DCore::QNodeId(), &input);
        nodeRemovedChange->setPropertyName("chord");
        backendInputChord.sceneChangeEvent(nodeRemovedChange);

        // THEN
        QCOMPARE(backendInputChord.chords().size(), 0);
    }
};

QTEST_APPLESS_MAIN(tst_InputChord)

#include "tst_inputchord.moc"
