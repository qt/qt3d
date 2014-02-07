#!/usr/bin/python
#
# Helper script to run unit tests repeatedly.
#
# Author: Mirko Boehm
# Copyright: (C) 2005-2013 Mirko Boehm
# Contact: mirko@kde.org
#     http://www.kde.org
#     http://creative-destruction.me $
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public License
# along with this library; see the file COPYING.LIB.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA 02110-1301, USA.

import re
import os
import sys
import time
import platform

def RunTests( Number ):
    # use Linux as the default:
    SuccessCount = 0
    Platform = platform.platform()
#   Command = "./ThreadWeaverJobTests EmitStartedOnFirstElementTest"
#   Command = "./ThreadWeaverJobTests CollectionDependenciesTest"
#   Command = "./ThreadWeaverJobTests JobSignalsDeliveryTest"
#   Command = "./ThreadWeaverJobTests"
    Command = "make test"
#   if re.search( "Darwin", Platform ):
#       Command = "DYLD_LIBRARY_PATH=../Weaver:../Experimental ./JobTests.app/Contents/MacOs/JobTests >/dev/null"
    for count in range( Number ):
	retValue = os.system ( Command)
	resultString = " (FAILED)"
	if retValue == 0:
	    resultString = " (succeeded)"
            SuccessCount = SuccessCount + 1
        else:
            sys.exit(1)
        print "Test run #" + str(count + 1) + resultString
    return SuccessCount

Number = 10

try:
    Number = int ( sys.argv[1] )
except:
    print "No number given, using default of ", Number

FailedCount = 0
SuccessCount = 0

t1 = time.time()
SuccessCount = RunTests ( Number )
t2 = time.time()


elapsed = ((t2 - t1) / 60.0, t2 - t1, (t2 - t1) * 1000.0)

print "RunTests: " + str(Number) + " test runs (" + str(SuccessCount) + " successful, " + str(Number - SuccessCount) + " failed)"
print "RunTests: %0.3fms (%0.3fs)" % ( elapsed[2], elapsed[1] )
print "Averages: %0.3fms per test run (%0.3fs)" % ( elapsed[2] / Number, elapsed[1] / Number )



