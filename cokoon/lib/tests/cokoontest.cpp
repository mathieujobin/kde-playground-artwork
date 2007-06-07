/* This file is part of the coKoon test suite
 * Copyright (C) 2006 Sandro Giessl <giessl@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <qtest_kde.h>

#include <QColor>
#include <QDebug>

#include <QPainter>

#include "../bridges.h"
#include "../expression_p.h"
#include "../document.h"
#include "../object_p.h"
#include "../tile_p.h"
#include "../tilesource_p.h"
#include "../tilelayout_p.h"

#include "DecorationThemeSpec.h"

#include "cokoontest.h"

QTEST_KDEMAIN(CokoonTest, GUI)

/**
 * This class is a specialization of ExpressionVariableBridge and
 * ExpressionVariableIndexBridge. It can store
 * values of variables and provides the according indizes.
 */
class ExpressionVariableHash : public Cokoon::ExpressionVariableBridge,
                               public Cokoon::ExpressionVariableIndexFactory
{
    public:
        ExpressionVariableHash()
        {
        };
        virtual ~ExpressionVariableHash()
        {
        };

        /**
         * Set the value of the variable @p id to @p val
         */
        void setValue(const QString &id, const QVariant &val)
        {
            if (m_varIndex.contains(id) ) {
                int idx = m_varIndex.value(id);
                m_vars.replace(idx, val);
            } else {
                int idx = m_vars.size();
                m_varIndex.insert(id, idx);
                m_vars.append(val);
            }
        };

        virtual int getVariableIndex(const QString &id) const
        {
            if (m_varIndex.contains(id) )
                return m_varIndex.value(id);
            else
                return -1;
        };


    virtual QVariant getVariableValue(int idx) const
        {
            if (m_vars.size() > idx)
                return m_vars.at(idx);
            else
                return QVariant();
        };

    private:
        QHash<QString, int> m_varIndex;
        QList<QVariant> m_vars;
};


void CokoonTest::evaluate()
{
    Cokoon::Expression e;

    ExpressionVariableHash vars;
    vars.setValue("variable1", 4);
    vars.setValue("testVar", 6);
    vars.setValue("testvar", 1);

    QCOMPARE( e.evaluate(), QVariant() );

    QCOMPARE( e.parse("1+5").expression(), QString("1 5 +") );
    QCOMPARE( e.parse("3*7").expression(), QString("3 7 *") );
    QCOMPARE( e.parse("2+4*7").expression(), QString("2 4 7 * +") );
    QCOMPARE( e.parse("(2+4)*7").expression(), QString("2 4 + 7 *") );
    QCOMPARE( e.parse("2*4+7").expression(), QString("2 4 * 7 +") );
    QCOMPARE( e.parse("2*(4+7)").expression(), QString("2 4 7 + *") );

    e.parse("1+5");
    QCOMPARE( e.evaluate(), QVariant(6) );

    e.parse("3*7");
    QVERIFY( e.evaluate() == 21 );

    e.parse("14/2");
    QVERIFY( e.evaluate() == 7 );

    QCOMPARE( e.parse("3.5").expression(), QString("3.5") );
    QVERIFY( e.evaluate() == 3.5f );

    e.parse("(7/2)");
    QVERIFY( e.evaluate() == 3.5f );

    e.parse("(7+4)/(2)");
    QVERIFY( e.evaluate() == 5.5f );

    e.parse("7+4/2");
    QVERIFY( e.evaluate() == 9 );

    e.parse("3*(2+4)-1");
    QVERIFY( e.isValid() == true );
    QVERIFY( e.evaluate() == 17 );

    e.parse("1+3*2+2");
    QVERIFY( e.evaluate() == 9);

    QCOMPARE(e.parse("1+(3*(2-(6/2)))+3").expression(), QString("1 3 2 6 2 / - * + 3 +") );
    QVERIFY( e.evaluate() == 1);

    e.parse("4+5*2");
    QVERIFY( e.evaluate() == 14);

    QCOMPARE(e.parse("variable1+5*2", &vars).expression(), QString("v0 5 2 * +"));
    QVERIFY( e.isValid() == true );
    QVERIFY( e.evaluate(&vars) == 14);

    QCOMPARE(e.parse("testVar*(testvar-variable1*2)", &vars).expression(), QString("v1 v2 v0 2 * - *") );
    QVERIFY( e.evaluate(&vars) == -42);

    QCOMPARE(e.parse("12%5").expression(), QString("12 5 %") );
    QVERIFY( e.evaluate() == 2);

    e.parse("12%5-5%3*3");
    QVERIFY( e.evaluate() == -4);

    e.parse("2%0");
    QVERIFY( e.evaluate() == QVariant() );

    e.parse("2/0");
    QVERIFY( e.evaluate() == QVariant() );

    QCOMPARE( e.parse("2+max(1+5,2*4)").expression(), QString("2 1 5 + 2 4 * max +") );
    QCOMPARE( e.evaluate(),  QVariant(10) );
    QCOMPARE( e.parse("min(1+5,2*max(2,1))").expression(), QString("1 5 + 2 2 1 max * min") );
    QCOMPARE( e.evaluate(),  QVariant(4) );

    QCOMPARE( e.parse("#AABBCC").expression(), QString("#aabbcc") );
    QCOMPARE( e.evaluate(),  QVariant(QColor("#aaBBcC")) );

    QCOMPARE( e.parse("rgb(255,200+55,254*1+1)").expression(), QString("255 200 55 + 254 1 * 1 + rgb") );
    QCOMPARE( e.evaluate(),  QVariant(QColor("#ffffff")) );

    QCOMPARE( e.parse("rgba(0, 255, 100-50, 2*33)").expression(), QString("0 255 100 50 - 2 33 * rgba") );
    QCOMPARE( e.evaluate(),  QVariant(QColor(0, 255, 50, 66)) );
}

void CokoonTest::loadThemeDocument()
{
    DecorationThemeSpec::SpecDocument doc;
    doc.loadTheme("TestDecoration.xml");

    ExpressionVariableHash vars;
    vars.setValue("testVar", 5);

    QImage test1Orig("testdeco1.xpm");

    QImage test1Theme(test1Orig.size(), QImage::Format_ARGB32_Premultiplied );
    QPainter p(&test1Theme);

    const int objId = DecorationThemeSpec::Decoration + DecorationThemeSpec::Decoration0_active +
                      DecorationThemeSpec::Decoration1_normal + DecorationThemeSpec::Decoration2_normal +
                      DecorationThemeSpec::Decoration3_normal;

    QCOMPARE( doc.getExpValue( objId, DecorationThemeSpec::Exp_variableExpression, &vars ), QVariant(5) );

    doc.drawLayers( objId, &p, 0, 0, test1Orig.size().width(), test1Orig.size().height(), &vars );

    test1Theme.save("temp-testdeco1-result.xpm", "XPM");

    bool equal = true;
    for (int i=0; equal && i < test1Orig.width(); ++i) {
        for (int j=0; equal && j < test1Orig.height(); ++j) {
            if (test1Orig.pixel(i,j) != test1Theme.pixel(i,j) ) {
                qDebug() << "Compared pixel" << i << j << "not the same:" << test1Orig.pixel(i,j) << "!=" << test1Theme.pixel(i,j);
                equal = false;
            }
        }
    }
    QVERIFY(equal);
}

void CokoonTest::themeExpressionReferences()
{
    DecorationThemeSpec::SpecDocument doc;
    doc.loadTheme("TestDecoration.xml");

    const int objId = DecorationThemeSpec::Button + DecorationThemeSpec::Button0_close +
                      DecorationThemeSpec::Button1_normal + DecorationThemeSpec::Button2_active +
                      DecorationThemeSpec::Button3_normal;
    QCOMPARE( doc.getExpValue( objId, DecorationThemeSpec::Exp_objectExp ),  QVariant( 5 ) );
    QCOMPARE( doc.getExpValue( objId, DecorationThemeSpec::Exp_objectExp2 ),  QVariant( 6 ) );
    QCOMPARE( doc.getExpValue( objId, DecorationThemeSpec::Exp_objectExp3 ),  QVariant( 7 ) );
}

#include "cokoontest.moc"

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
