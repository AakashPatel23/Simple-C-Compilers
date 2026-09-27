import subprocess
import os
import sys

import unittest
uscc = "../../bin/uscc"

__unittest = True

class OptIRTests(unittest.TestCase):

    def setUp(self):
        self.maxDiff = None
        if not os.path.isfile(uscc):
            raise Exception("Can't run without uscc")

    def checkIR(self, fileName):
        # read in expected
        expectFile = open("expected/" + fileName + "_IR.output", "r")
        expectedStr = expectFile.read()
        expectFile.close()
        # Check if the output string is expected.
        try:
            resultStr = subprocess.check_output([uscc, "-O", "-p", fileName + ".usc"], stderr=subprocess.STDOUT)
            self.assertMultiLineEqual(expectedStr, resultStr)
        except subprocess.CalledProcessError as e:
            self.fail("\n" + e.output)

    def test_opt_IR_emit02(self):
        self.checkIR("emit02")
    def test_opt_IR_emit03(self):
        self.checkIR("emit03")
    def test_opt_IR_emit04(self):
        self.checkIR("emit04")
    def test_opt_IR_emit05(self):
        self.checkIR("emit05")
    def test_opt_IR_emit06(self):
        self.checkIR("emit06")
    def test_opt_IR_emit07(self):
        self.checkIR("emit07")
    def test_opt_IR_emit08(self):
        self.checkIR("emit08")
    def test_opt_IR_emit09(self):
        self.checkIR("emit09")
    def test_opt_IR_emit10(self):
        self.checkIR("emit10")
    def test_opt_IR_emit11(self):
        self.checkIR("emit11")
    def test_opt_IR_emit12(self):
        self.checkIR("emit12")
    def test_opt_IR_quicksort(self):
        self.checkIR("quicksort")
    def test_opt_IR_015(self):
        self.checkIR("test015")
    def test_opt_IR_016(self):
        self.checkIR("test016")
    def test_opt_IR_01(self):
        self.checkIR("opt01")
    def test_opt_IR_02(self):
        self.checkIR("opt02")
    def test_opt_IR_03(self):
        self.checkIR("opt03")
    def test_opt_IR_04(self):
        self.checkIR("opt04")
    def test_opt_IR_05(self):
        self.checkIR("opt05")
    def test_opt_IR_06(self):
        self.checkIR("opt06")
    def test_opt_IR_07(self):
        self.checkIR("opt07")

if __name__ == '__main__':
    unittest.main(verbosity=2)
