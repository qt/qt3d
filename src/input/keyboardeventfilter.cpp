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

#include "keyboardeventfilter_p.h"
#include "inputhandler_p.h"
#include <QEvent>
#include <QKeyEvent>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Input {

// The KeyboardEventFilter must be in the same thread as the view it will be monitoring

KeyboardEventFilter::KeyboardEventFilter(QObject *parent)
    : QObject(parent)
{
}

void KeyboardEventFilter::setInputHandler(InputHandler *handler)
{
    m_inputHandler = handler;
}

// Triggered in the view thread (usually the main thread)
bool KeyboardEventFilter::eventFilter(QObject *obj, QEvent *e)
{
    if (e->type() == QEvent::KeyPress || e->type() == QEvent::KeyRelease) {
        // Store event to be processed later on in an InputAspect job
        m_inputHandler->appendKeyEvent(QKeyEvent(*static_cast<QKeyEvent *>(e)));
    }
    return QObject::eventFilter(obj, e);
}

} // Input

} // Qt3D

QT_END_NAMESPACE
