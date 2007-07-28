'''
 unit test for themespec.py
 Cokoon theme editor
 Copyright (C) 2007 Sandro Giessl <giessl@kde.org>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public
 License as published by the Free Software Foundation; either
 version 2 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; see the file COPYING.  If not, write to
 the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 Boston, MA 02110-1301, USA.
'''

import unittest
from themespec import ThemeSpec
from themespec import ThemeSpecDocument

class ReadSpecCheck(unittest.TestCase):
  def setUp(self):
    self.spec = ThemeSpec("Tests/CokoonStyleSpec.xml")

  def tearDown(self):
    pass

  def testSpecRead(self):
    """Check that the specification file is loaded correctly"""
    assert self.spec.name=="CokoonStyleSpec", "wrong name"
    assert len(self.spec.items)==31, "wrong number of items"
    assert len(self.spec.specObjNameToId)==31, "wrong length of object name map"
    assert len(self.spec.specVariableToId)==0, "wrong number of variables"
    assert len(self.spec.specIdentifierToId)==56, "wrong number of identifiers"
    assert self.spec.specObjIdBase==40815, "wrong object base ID"

if __name__=="__main__":
  unittest.main()
