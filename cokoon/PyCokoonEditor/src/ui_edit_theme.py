# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'edit_theme.ui'
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
        Form.resize(QtCore.QSize(QtCore.QRect(0,0,220,404).size()).expandedTo(Form.minimumSizeHint()))

        self.vboxlayout = QtGui.QVBoxLayout(Form)
        self.vboxlayout.setMargin(9)
        self.vboxlayout.setSpacing(6)
        self.vboxlayout.setObjectName("vboxlayout")

        self.label_4 = QtGui.QLabel(Form)
        self.label_4.setObjectName("label_4")
        self.vboxlayout.addWidget(self.label_4)

        self.label = QtGui.QLabel(Form)
        self.label.setObjectName("label")
        self.vboxlayout.addWidget(self.label)

        self.name = QtGui.QLineEdit(Form)
        self.name.setObjectName("name")
        self.vboxlayout.addWidget(self.name)

        self.label_5 = QtGui.QLabel(Form)
        self.label_5.setObjectName("label_5")
        self.vboxlayout.addWidget(self.label_5)

        self.version = QtGui.QLineEdit(Form)
        self.version.setObjectName("version")
        self.vboxlayout.addWidget(self.version)

        self.label_3 = QtGui.QLabel(Form)
        self.label_3.setObjectName("label_3")
        self.vboxlayout.addWidget(self.label_3)

        self.spec = QtGui.QLineEdit(Form)
        self.spec.setObjectName("spec")
        self.vboxlayout.addWidget(self.spec)

        spacerItem = QtGui.QSpacerItem(202,191,QtGui.QSizePolicy.Minimum,QtGui.QSizePolicy.Expanding)
        self.vboxlayout.addItem(spacerItem)
        self.label.setBuddy(self.name)
        self.label_5.setBuddy(self.version)
        self.label_3.setBuddy(self.version)

        self.retranslateUi(Form)
        QtCore.QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        Form.setWindowTitle(QtGui.QApplication.translate("Form", "Form", None, QtGui.QApplication.UnicodeUTF8))
        self.label_4.setText(QtGui.QApplication.translate("Form", "<b>Theme</b>", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("Form", "&Name:", None, QtGui.QApplication.UnicodeUTF8))
        self.label_5.setText(QtGui.QApplication.translate("Form", "&Version:", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("Form", "&Specification:", None, QtGui.QApplication.UnicodeUTF8))

