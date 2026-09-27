#---------------------------------------------------------
# Copyright (c) 2014, Sanjay Madhav
# All rights reserved.
#
# This file is distributed under the BSD license.
# See LICENSE.TXT for details.
#---------------------------------------------------------
import subprocess
import os
import sys

import unittest
uscc = "../../bin/uscc"
lli = "../../../bin/lli"

__unittest = True

class PhiTests(unittest.TestCase):
	
	def setUp(self):
		self.maxDiff = None
		if not os.path.isfile(uscc):
			raise Exception("Can't run without uscc")
		if not os.path.isfile(lli):
			raise Exception("lli not found at ../../bin/lli")

	def checkPhi(self, fileName):
		# read in expected
		expectFile = open("expected/" + fileName + ".ll", "r")
		expectedStr = expectFile.read()
		expectFile.close()
		# first compile the .bc using uscc
		try:
			result = subprocess.check_output([uscc, "-rpr", fileName + ".ll"])
			self.assertMultiLineEqual(expectedStr, result.decode())
		except subprocess.CalledProcessError as e:
			self.fail("\n" + e.output)
		
		# now run it in lli and compare the output
			
		
	def test_Phi_phi01(self):
		self.checkPhi("phi1")
		
	def test_Phi_phi02(self):
		self.checkPhi("phi2")
	
	def test_Phi_phi03(self):
		self.checkPhi("phi3")		
		
	def test_Phi_phi04(self):
		self.checkPhi("phi4")
	
	def test_Phi_phi05(self):
		self.checkPhi("phi5")
		
	def test_Phi_phi06(self):
		self.checkPhi("phi6")
	
	def test_Phi_phi07(self):
		self.checkPhi("phi7")		
		
	def test_Phi_phi08(self):
		self.checkPhi("phi8")
		
	def test_Phi_phi09(self):
		self.checkPhi("phi9")
		

if __name__ == '__main__':
	unittest.main(verbosity=2)
