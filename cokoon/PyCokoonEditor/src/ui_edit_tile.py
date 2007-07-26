# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'edit_tile.ui'
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

        self.label_5 = QtGui.QLabel(Form)
        self.label_5.setObjectName("label_5")
        self.gridlayout.addWidget(self.label_5,3,0,1,1)

        self.label_2 = QtGui.QLabel(Form)
        self.label_2.setObjectName("label_2")
        self.gridlayout.addWidget(self.label_2,4,0,1,1)

        self.label = QtGui.QLabel(Form)
        self.label.setObjectName("label")
        self.gridlayout.addWidget(self.label,0,0,1,1)

        self.identifier = QtGui.QLineEdit(Form)
        self.identifier.setObjectName("identifier")
        self.gridlayout.addWidget(self.identifier,0,1,1,1)

        self.horizontal = QtGui.QComboBox(Form)
        self.horizontal.setObjectName("horizontal")
        self.gridlayout.addWidget(self.horizontal,1,1,1,1)

        self.vertical = QtGui.QComboBox(Form)
        self.vertical.setObjectName("vertical")
        self.gridlayout.addWidget(self.vertical,2,1,1,1)

        self.label_3 = QtGui.QLabel(Form)
        self.label_3.setObjectName("label_3")
        self.gridlayout.addWidget(self.label_3,1,0,1,1)

        self.label_4 = QtGui.QLabel(Form)
        self.label_4.setObjectName("label_4")
        self.gridlayout.addWidget(self.label_4,2,0,1,1)

        self.source = QtGui.QComboBox(Form)
        self.source.setObjectName("source")
        self.gridlayout.addWidget(self.source,3,1,1,1)

        self.label_6 = QtGui.QLabel(Form)
        self.label_6.setObjectName("label_6")
        self.gridlayout.addWidget(self.label_6,5,0,1,1)

        self.top = QtGui.QLineEdit(Form)
        self.top.setObjectName("top")
        self.gridlayout.addWidget(self.top,5,1,1,1)

        self.left = QtGui.QLineEdit(Form)
        self.left.setObjectName("left")
        self.gridlayout.addWidget(self.left,4,1,1,1)

        self.label_7 = QtGui.QLabel(Form)
        self.label_7.setObjectName("label_7")
        self.gridlayout.addWidget(self.label_7,6,0,1,1)

        self.label_8 = QtGui.QLabel(Form)
        self.label_8.setObjectName("label_8")
        self.gridlayout.addWidget(self.label_8,7,0,1,1)

        self.height = QtGui.QLineEdit(Form)
        self.height.setObjectName("height")
        self.gridlayout.addWidget(self.height,7,1,1,1)

        self.width = QtGui.QLineEdit(Form)
        self.width.setObjectName("width")
        self.gridlayout.addWidget(self.width,6,1,1,1)

        spacerItem = QtGui.QSpacerItem(20,51,QtGui.QSizePolicy.Minimum,QtGui.QSizePolicy.Expanding)
        self.gridlayout.addItem(spacerItem,8,1,1,1)
        self.label_5.setBuddy(self.source)
        self.label_2.setBuddy(self.left)
        self.label.setBuddy(self.identifier)
        self.label_3.setBuddy(self.horizontal)
        self.label_4.setBuddy(self.vertical)
        self.label_6.setBuddy(self.top)
        self.label_7.setBuddy(self.width)
        self.label_8.setBuddy(self.height)

        self.retranslateUi(Form)
        QtCore.QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        Form.setWindowTitle(QtGui.QApplication.translate("Form", "Form", None, QtGui.QApplication.UnicodeUTF8))
        self.label_5.setText(QtGui.QApplication.translate("Form", "&Source:", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setText(QtGui.QApplication.translate("Form", "&Left:", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("Form", "&Identifier:", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("Form", "&Horizontal:", None, QtGui.QApplication.UnicodeUTF8))
        self.label_4.setText(QtGui.QApplication.translate("Form", "&Vertical:", None, QtGui.QApplication.UnicodeUTF8))
        self.label_6.setText(QtGui.QApplication.translate("Form", "&Top:", None, QtGui.QApplication.UnicodeUTF8))
        self.label_7.setText(QtGui.QApplication.translate("Form", "&Width:", None, QtGui.QApplication.UnicodeUTF8))
        self.label_8.setText(QtGui.QApplication.translate("Form", "H&eight:", None, QtGui.QApplication.UnicodeUTF8))

