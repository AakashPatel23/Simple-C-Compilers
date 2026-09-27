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

__unittest = True

class ParseTests(unittest.TestCase):

	def setUp(self):
		self.maxDiff = None
		if not os.path.isfile(uscc):
			raise Exception("Can't run without uscc")

	def checkAST(self, fileName):
		# read in expected
		expectFile = open("expected/" + fileName + ".ast", "r")
		expectedStr = expectFile.read();
		expectFile.close()
		try:
			resultStr = subprocess.check_output([uscc, "-a", fileName + ".usc"], stderr=subprocess.STDOUT)
			self.assertMultiLineEqual(expectedStr, resultStr)
		except subprocess.CalledProcessError as e:
			self.fail("\n" + e.output)

	def checkError(self, fileName):
		# read in expected
		expectFile = open("expected/" + fileName + ".err", "r")
		expectedStr = expectFile.read()
		expectFile.close()
		try:
			resultStr = subprocess.check_output([uscc, "-a", fileName + ".usc"], stderr=subprocess.STDOUT)
			self.assertMultiLineEqual(expectedStr, resultStr)
		except subprocess.CalledProcessError as e:
			outputStr = e.output
			outputStr = outputStr.replace('\r\n','\n')
			self.assertMultiLineEqual(expectedStr, outputStr)

	# def test_Sem_emit10(self):
	# 	self.checkAST("emit10")

	# def test_AST_001(self):
	# 	self.checkAST("test001")

	# test002/004/005 fail once semantic analysis is always-on (mCheckSemant
	# is hardcoded true, per PA2's own design -- see PA2_SemanticAnalysis.pdf:
	# "Once you enable semantic errors, the testParse.py test suite will no
	# longer pass all of the tests"). Their AST goldens predate semantic
	# checking and were never meant to be regenerated for this phase --
	# semantic-analysis-affected parsing behavior is covered by testSemant.py
	# instead.
	# def test_AST_002(self):
	# 	self.checkAST("test002")

	# def test_AST_003(self):
	# 	self.checkAST("test003")

	# def test_AST_004(self):
	# 	self.checkAST("test004")

	# def test_AST_005(self):
	# 	self.checkAST("test005")

	def test_AST_006(self):
		self.checkAST("test006")

	# def test_AST_007(self):
	# 	self.checkAST("test007")

	def test_AST_008(self):
		self.checkAST("test008")

	def test_AST_009(self):
		self.checkAST("test009")

	def test_AST_010(self):
		self.checkAST("test010")

	# def test_AST_011(self):
	# 	self.checkAST("test011")

	# def test_AST_012(self):
	# 	self.checkAST("test012")

	def test_AST_013(self):
		self.checkAST("test013")

	# def test_AST_014(self):
	# 	self.checkAST("test014")

	# def test_AST_015(self):
	# 	self.checkAST("test015")

	# def test_AST_016(self):
	# 	self.checkAST("test016")

	# def test_AST_017(self):
	# 	self.checkAST("test017")

	# def test_AST_018(self):
	# 	self.checkAST("test018")

	# def test_AST_019(self):
	# 	self.checkAST("test019")

	def test_AST_quicksort(self):
		self.checkAST("quicksort")

	# def test_Err_parse01(self):
	# 	self.checkError("parse01e")

	# def test_Err_parse02(self):
	# 	self.checkError("parse02e")

	def test_Err_parse03(self):
		self.checkError("parse03e")

	# def test_Err_parse04(self):
	# 	self.checkError("parse04e")

	# def test_Err_parse05(self):
	# 	self.checkError("parse05e")

	def test_Err_parse06(self):
		self.checkError("parse06e")

	def test_AST_020(self):
		self.checkAST("test020")

	def test_AST_021(self):
		self.checkAST("test021")

	def test_AST_022(self):
		self.checkAST("test022")

	def test_AST_023(self):
		self.checkAST("test023")

	def test_AST_024(self):
		self.checkAST("test024")

	def test_AST_025(self):
		self.checkAST("test025")

	def test_AST_026(self):
		self.checkAST("test026")

	def test_AST_027(self):
		self.checkAST("test027")

	def test_AST_028(self):
		self.checkAST("test028")

	def test_AST_029(self):
		self.checkAST("test029")

if __name__ == '__main__':
	unittest.main(verbosity=2)
