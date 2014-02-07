/* -*- C++ -*-

This file implements the state handling in ThreadWeaver.

$ Author: Mirko Boehm $
$ Copyright: (C) 2005-2013 Mirko Boehm $
$ Contact: mirko@kde.org
http://www.kde.org
http://creative-destruction.me $

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.

$Id: State.cpp 20 2005-08-08 21:02:51Z mirko $
*/

#include "state.h"

#include <QtCore/QString>

namespace ThreadWeaver
{

const char *StateNames[NoOfStates] = {
    "InConstruction",
    "WorkingHard",
    "Suspending",
    "Suspended",
    "ShuttingDown",
    "Destructed"
};

class State::Private
{
public:
    Private(QueueInterface *theWeaver)
        : weaver(theWeaver)
    {
        Q_ASSERT_X(sizeof StateNames / sizeof StateNames[0] == NoOfStates, "State::Private ctor",
                   "Make sure to keep StateId and StateNames in sync!");
    }

    /** The Weaver we relate to. */
    QueueInterface *const weaver;
};

State::State(QueueSignals *weaver)
    : d(new Private(weaver))
{
}

State::~State()
{
    delete d;
}

QString State::stateName() const
{
    return QLatin1String(StateNames[stateId()]);
}

void State::activated()
{
}

QueueInterface *State::weaver()
{
    return d->weaver;
}

const QueueInterface *State::weaver() const
{
    return d->weaver;
}

}
