"""unit test for themespec.py"""

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
