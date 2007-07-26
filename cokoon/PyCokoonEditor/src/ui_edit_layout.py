# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'edit_layout.ui'
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

        spacerItem = QtGui.QSpacerItem(20,51,QtGui.QSizePolicy.Minimum,QtGui.QSizePolicy.Expanding)
        self.gridlayout.addItem(spacerItem,1,1,1,1)

        self.layout = QtGui.QComboBox(Form)
        self.layout.setObjectName("layout")
        self.gridlayout.addWidget(self.layout,0,1,1,1)

        self.label = QtGui.QLabel(Form)

        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Policy(4),QtGui.QSizePolicy.Policy(5))
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label.sizePolicy().hasHeightForWidth())
        self.label.setSizePolicy(sizePolicy)
        self.label.setObjectName("label")
        self.gridlayout.addWidget(self.label,0,0,1,1)
        self.label.setBuddy(self.layout)

        self.retranslateUi(Form)
        QtCore.QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        Form.setWindowTitle(QtGui.QApplication.translate("Form", "Form", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("Form", "&Layout:", None, QtGui.QApplication.UnicodeUTF8))

