/* -*- C++ -*-

   This file is part of ThreadWeaver, a KDE framework.

   $ Author: Mirko Boehm $
   $ Copyright: (C) 2013 Mirko Boehm $
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

*/

#include "queuesignals.h"

namespace ThreadWeaver
{

/** @brief Construct a QueueSignals object, passing the QObject parent. */
QueueSignals::QueueSignals(QObject *parent)
    : QObject(parent)
{
}

}
