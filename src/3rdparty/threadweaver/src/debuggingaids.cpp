/* -*- C++ -*-

   This file implements debugging aids for multithreaded applications.

   $ Author: Mirko Boehm $
   $ Copyright: (C) 2004-2013 Mirko Boehm $
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

   $Id: DebuggingAids.cpp 20 2005-08-08 21:02:51Z mirko $
*/

#include "debuggingaids.h"

#include <QtCore/QMutex>

#include <threadweaver_export.h>

/** A global mutex for the ThreadWeaver objects.
    Generally, you should not use it in your own code. */
THREADWEAVER_EXPORT QMutex ThreadWeaver::GlobalMutex;
THREADWEAVER_EXPORT bool ThreadWeaver::Debug = true;
THREADWEAVER_EXPORT int ThreadWeaver::DebugLevel = 01;
