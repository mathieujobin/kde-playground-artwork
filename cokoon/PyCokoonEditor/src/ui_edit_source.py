# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'edit_source.ui'
#
# Created: Thu Jul 26 23:13:54 2007
#      by: PyQt4 UI code generator 4.1
#
# WARNING! All changes made in this file will be lost!

import sys
from PyQt4 import QtCore, QtGui

class Ui_Form(object):
    def setupUi(self, Form):
        Form.setObjectName("Form")
        Form.resize(QtCore.QSize(QtCore.QRect(0,0,249,368).size()).expandedTo(Form.minimumSizeHint()))

        self.gridlayout = QtGui.QGridLayout(Form)
        self.gridlayout.setMargin(9)
        self.gridlayout.setSpacing(6)
        self.gridlayout.setObjectName("gridlayout")

        self.label = QtGui.QLabel(Form)
        self.label.setObjectName("label")
        self.gridlayout.addWidget(self.label,0,0,1,1)

        self.identifier = QtGui.QLineEdit(Form)
        self.identifier.setObjectName("identifier")
        self.gridlayout.addWidget(self.identifier,0,1,1,1)

        self.label_2 = QtGui.QLabel(Form)
        self.label_2.setObjectName("label_2")
        self.gridlayout.addWidget(self.label_2,1,0,1,1)

        self.label_3 = QtGui.QLabel(Form)
        self.label_3.setObjectName("label_3")
        self.gridlayout.addWidget(self.label_3,2,0,1,1)

        self.file = QtGui.QLineEdit(Form)
        self.file.setObjectName("file")
        self.gridlayout.addWidget(self.file,2,1,1,1)

        spacerItem = QtGui.QSpacerItem(20,231,QtGui.QSizePolicy.Minimum,QtGui.QSizePolicy.Expanding)
        self.gridlayout.addItem(spacerItem,3,1,1,1)

        self.type = QtGui.QComboBox(Form)
        self.type.setObjectName("type")
        self.gridlayout.addWidget(self.type,1,1,1,1)
        self.label.setBuddy(self.identifier)
        self.label_2.setBuddy(self.type)
        self.label_3.setBuddy(self.file)

        self.retranslateUi(Form)
        QtCore.QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        Form.setWindowTitle(QtGui.QApplication.translate("Form", "Form", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("Form", "&Identifier:", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setText(QtGui.QApplication.translate("Form", "&Type:", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("Form", "&File:", None, QtGui.QApplication.UnicodeUTF8))

