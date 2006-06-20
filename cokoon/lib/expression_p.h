/* This file is part of the coKoon theme library
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

#ifndef COKOON_EXPRESSION_H
#define COKOON_EXPRESSION_H

#include <QString>
#include <QHash>
#include <QVariant>

class QColor;

namespace Cokoon {

class ExpressionNode;
class ExpressionVariableBridge;
class ExpressionVariableIndexFactory;
class ExpressionReference;

/**
 * This class parses and evaluates an expression. Expressions
 * are used in themes (Object, Tile, TileLayout, TileSource) to provide various
 * data (Int, Double, Color) to the theme engine.
 *
 * @author Sandro Giessl <giessl@kde.org>
 */
class Expression
{
    public:
        /**
         * Construct.
         */
	Expression();
        /**
         * Destruct.
         */
        virtual ~Expression();

        /**
         * Parse expression string and prepare it for evaluation.
         *
         * Color values can be set with the notation '\#RRGGBB' or with the function rgb/rgba
         *
         * Supported operators are '+', '-', '*', '/', '%'. '(', ')' are used for
         * precedence grouping.
         *
         * Supported functions are max, min, rgb, rgba, blend, adjustHsv, light, dark
         *
         * Variables are identifiers starting with a letter and containing numbers
         * or letters. They need to be declared (@p varIdxs ).
         *
         * References are like variables, but containing '.' characters. See Object,
         * Tile, TileLayout, TileSource classes what references are allowed exactly.
         * @param expr Expression string in infix notation.
         * @param varIdxs Provides the indizes of 'declared'
         * variables.
	 * @param refs provides objects of type ExpressionReference
         */
        Expression &parse(const QString &expr,
                          const ExpressionVariableIndexFactory *varIdxs = 0);

        /**
         * @returns The expression that has been set (postfix notation).
         * @sa parse()
         */
        const QString &expression() const;
        /**
         * @returns Whether the expression is constant (i.e. the expression does not
         * contain variables or references and thus the value won't change).
         */
        bool isConstant() const;
        /**
         * Clear the expression, making it invalid.
         * @sa isValid()
         */
        void clearExpression();

        /**
         * @returns Whether the expression has successfully been parsed.
         */
        bool isValid() const;

        /**
         * Calculates the expression.
         * @param vars Variable values which can be used in the calculation.
         * @param refs Expression references which can be used in the calculation.
         * @returns The result of the evaluation. Can be of type Int, Double and QColor, or invalid if the calculation hasn't been successful.
         */
        QVariant evaluate(const ExpressionVariableBridge *vars = 0) const;

protected:
    /**
     * @return a reference for a given reference string @p ref, or 0
     * if the reference is invalid
     */
    virtual ExpressionReference *getReference(const QString &ref) const;

    private:
        friend class OperationNode;
        friend class VariableNode;
        friend class ReferenceNode;

        QString m_postfixExpStr;
        QList<ExpressionNode*> m_expr;
        bool m_const;
        bool m_valid;

        /**
         * @returns The expression in postfix notation, mainly intended for
         * debugging and unit testing.
         */
        QString postfixNotation() const;

        /**
         * Only accessible while in evaluate(); accessed by VariableNode.
         */
        mutable const ExpressionVariableBridge *m_vars;
        /**
         * To prevent recursion loop (e.g. caused by reference loop or badly
         * implemented ExpressionVariableBridge): cancel evaluate() if it
         * has been entered twice before exiting.
         */
        mutable bool m_evalLock;

        QVariant m_cachedResult;
//         MetricMap m_cachedVars;

        /**
         * used for expression validation
         */
        enum ParseContext {
            ValueNode,
            OperatorNode,
            ParenthesisOpenNode
        };

        /**
         * Parse Items: The list of supported operators
         */
        enum Operators {
            oAdd,       ///< '+'
            oSub,       ///< '-'
            oMult,      ///< '*'
            oDiv,       ///< '/'
            oMod,       ///< '%'
            oLast
        };
        /**
         * Parse Items: The list of supported functions
         */
        enum Functions {
            fMin=oLast, ///< min(num,num)
            fMax,       ///< max(num,num)
            fRgb,       ///< rgb(num[0-255],num[0-255],num[0-255])
            fRgba,      ///< rgba(num[0-255],num[0-255],num[0-255],num[0-255])
            fBlend,     ///< blend(col,col,num[0-100])
            fAdjustHsv, ///< adjustHsv(col,num[0-255],num[0-255],num[0-255])
            fDark,      ///< dark(col,[0-100-200])
            fLight,     ///< light(col,[0-100-200])
            fLast
        };
        /**
         * Parse Items: Other things that need to be put onto the infix notation
         * to postfix notation translation stack.
         */
        enum ParseItems {
            pLeftPar=fLast ///< '('
        };

        /**
         * @returns The precedence of the operator @p o . 0 means highest.
         */
        static uint operatorPrecedence(int o);

        /**
         * @returns The index of the operator @p c or -1 if @p c is no operator
         */
        static int getOperator(const QChar &c);
        /**
         * @returns Whether @p index is the index of an operator
         */
        static bool isOperator(int index);
        /**
         * @returns The string representation of the operation (operator or
         * function) with index @p index
         */
        static QString operationStr(int index);

        /**
         * @returns The index of the function identifier @p str
         * or -1 if @p str is no function
         */
        static int getFunction(const QString &str);
        /**
         * @returns Whether @p index is the index of a function
         */
        static bool isFunction(int index);

        class ExpressionPrivate * d;
};

/**
 * This class is used as bridge between the theme engine and expressions. It
 * is used to provide indizes of variables.
 * @sa Expression::evaluate()
 *
 * @author Sandro Giessl <giessl@kde.org>
 */
class ExpressionVariableIndexFactory
{
public:
    ExpressionVariableIndexFactory();
    virtual ~ExpressionVariableIndexFactory();

    /**
     * @returns The index of the variable which has the identifier @p id . -1
     * if the variable does not exist.
     */
    virtual int getVariableIndex(const QString &id) const = 0;
};

/**
 * This class is used as bridge between the theme engine and expressions. It
 * is used to provide references to other expressions.
 * @sa Expression::evaluate()
 */
class ExpressionReference
{
public:
    ExpressionReference();
    virtual ~ExpressionReference();

    /**
     * @returns string representation of the reference. Mainly for
     * debugging purposes.
     * @sa Expression::expression()
     */
    virtual QString ref() const = 0;

    /**
     * @returns the value this reference points to, might be invalid
     */
    virtual QVariant getReferenceValue(const ExpressionVariableBridge *vars) const = 0;
};

}

#endif

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
