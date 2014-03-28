/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "renderbin.h"

#include "drawable.h"
#include "qgraphicscontext.h"
#include "rendershader.h"
#include "drawstate.h"

//#define QT3D_STATE_CHANGE_DEBUG

namespace Qt3D
{

namespace Render {


void OpaqueBin::sendDrawingCommands(QGraphicsContext *gc)
{
    int shaderChangeCount = 0,
        stateSetChangeCount = 0,
        batchSize = 0;
    bool resetBatch = false;

    RenderShader* sh = NULL;
    DrawStateSet* ss = NULL;

    foreach (Drawable* d, m_drawables) {
        resetBatch = false;

        if (sh != d->shader()) {
            sh = d->shader();
            gc->activateShader(sh);
            ++shaderChangeCount;
        #if defined(QT3D_STATE_CHANGE_DEBUG)
            qDebug() << "did shader change to" << sh->name();
        #endif
            resetBatch = true;
        }

        if (ss != d->stateSet()) {
            ss = d->stateSet();
            gc->setCurrentStateSet(ss);
            ++stateSetChangeCount;
        #if defined(QT3D_STATE_CHANGE_DEBUG)
            qDebug() << "did state change";
        #endif
            resetBatch = true;
        }

        if (resetBatch && (batchSize > 0)) {
        #if defined(QT3D_STATE_CHANGE_DEBUG)
            qDebug() << "previous batch was " << batchSize;
        #endif
            batchSize = 1;
        } else {
            ++batchSize;
        }

        d->sendDrawingCommands(gc);
    }

#if defined(QT3D_STATE_CHANGE_DEBUG)
    qDebug() << "final batch:" << batchSize;
#endif
}

void OpaqueBin::addDrawable(Drawable *dr)
{
    Q_CHECK_PTR(dr);
    RenderShader* sh = dr->shader();
    DrawStateSet* ss = dr->stateSet();

    int shaderStartIndex = 0;
    for (; shaderStartIndex <m_drawables.size(); ++ shaderStartIndex) {
        if (m_drawables.at(shaderStartIndex)->shader() == sh) {
            break;
        }
    }

    if (shaderStartIndex >= m_drawables.size()) {
        // no matching shader found, simply append
        m_drawables.append(dr);
        return;
    }

// find last drawable using the shader. Also check for a matching
// DrawStateSet in case it exists (then we just insert beside)
    int shaderEndIndex = shaderStartIndex;
    QVector<DrawStateSet*> ssList;

    for (; shaderEndIndex < m_drawables.size(); ++shaderEndIndex) {
        if (m_drawables.at(shaderEndIndex)->shader() != sh)
            break; // found first drawable with different shader

        DrawStateSet* currentSS = m_drawables.at(shaderEndIndex)->stateSet();
        // record each distinct state-set applicable to this shader.
        if (ssList.empty() || ssList.back() != currentSS) {
            ssList.append(currentSS);

            // check cost delta is non-zero; if it is, the state-sets are
            // equivalent and we can simply merge
            if (ss->changeCost(currentSS) == 0) {
                m_drawables.insert(shaderEndIndex, dr);
                return;
            }
        }
    } // of same-shader range-iteration

    insertDrawableInRange(dr, shaderStartIndex, shaderEndIndex, ssList);
}

// select position in the (shaderStartIndex .. shaderEndIndex]
// range based on lowest cost StateSet transition. We already know the state-sets
// are distinct since we checked for changeCost() == 0 in the loop above

void OpaqueBin::insertDrawableInRange(Drawable *dr,
                                      int startIndex, int endIndex,
                                      const QVector<DrawStateSet*> ssList)
{
    qDebug() << "inserting drawable with different state-set";

// special case where there is only one existing state set
    if (ssList.size() == 1) {
        m_drawables.insert(endIndex, dr);
        return;
    }

    DrawStateSet* ss = dr->stateSet();
    DrawStateSet* prevSS = ssList.front();
    // default to appending
    int lowestCostIncrease = ss->changeCost(ssList.back());
    DrawStateSet* insertBeforeSS = NULL;

    // see if we can do better than appending
    for (int i=1; i<ssList.size(); ++i) {
        int oldCost = ssList.at(i)->changeCost(prevSS);
        int newCost = ss->changeCost(prevSS) + ssList.at(i)->changeCost(ss);
        int costIncrease = newCost - oldCost;

        if (costIncrease < lowestCostIncrease) {
            lowestCostIncrease = costIncrease;
            insertBeforeSS = ssList.at(i);
        }
    } // of distinct state-sets iteration

    if (insertBeforeSS) {
        for (int i=startIndex; i<endIndex; ++i) {
            if (m_drawables.at(i)->stateSet() == insertBeforeSS) {
                m_drawables.insert(i, dr);
                break;
            }
        }
    } else {
        // simply append
        m_drawables.insert(endIndex, dr);
    }
}

void OpaqueBin::removeDrawable(Drawable *dr)
{
    Q_CHECK_PTR(dr);
    m_drawables.removeOne(dr);
}

} // Render

} // Qt3D of namespace
