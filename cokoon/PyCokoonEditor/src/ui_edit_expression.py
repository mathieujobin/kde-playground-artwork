# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'edit_expression.ui'
#
# Created: Sun Aug  5 15:59:25 2007
#      by: PyQt4 UI code generator 4.1
#
# WARNING! All changes made in this file will be lost!

import sys
from PyQt4 import QtCore, QtGui

class Ui_Form(object):
    def setupUi(self, Form):
        Form.setObjectName("Form")
        Form.resize(QtCore.QSize(QtCore.QRect(0,0,249,328).size()).expandedTo(Form.minimumSizeHint()))

        self.vboxlayout = QtGui.QVBoxLayout(Form)
        self.vboxlayout.setMargin(9)
        self.vboxlayout.setSpacing(6)
        self.vboxlayout.setObjectName("vboxlayout")

        self.label_9 = QtGui.QLabel(Form)
        self.label_9.setObjectName("label_9")
        self.vboxlayout.addWidget(self.label_9)

        self.label = QtGui.QLabel(Form)
        self.label.setObjectName("label")
        self.vboxlayout.addWidget(self.label)

        self.identifier = QtGui.QLineEdit(Form)
        self.identifier.setObjectName("identifier")
        self.vboxlayout.addWidget(self.identifier)

        self.label_3 = QtGui.QLabel(Form)
        self.label_3.setObjectName("label_3")
        self.vboxlayout.addWidget(self.label_3)

        self.value = QtGui.QLineEdit(Form)
        self.value.setObjectName("value")
        self.vboxlayout.addWidget(self.value)

        spacerItem = QtGui.QSpacerItem(166,161,QtGui.QSizePolicy.Minimum,QtGui.QSizePolicy.Expanding)
        self.vboxlayout.addItem(spacerItem)
        self.label.setBuddy(self.identifier)
        self.label_3.setBuddy(self.value)

        self.retranslateUi(Form)
        QtCore.QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        Form.setWindowTitle(QtGui.QApplication.translate("Form", "Form", None, QtGui.QApplication.UnicodeUTF8))
        self.label_9.setText(QtGui.QApplication.translate("Form", "<b>Expression</b>", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("Form", "&Identifier:", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("Form", "&Value:", None, QtGui.QApplication.UnicodeUTF8))

