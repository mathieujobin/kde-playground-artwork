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

#include <QtDebug>
#include <QStack>
#include <QColor>

#include "bridges.h"
#include "expression_p.h"

namespace Cokoon {

class ExpressionNode
{
    public:
        ExpressionNode() {};
        virtual ~ExpressionNode() {};
};

class ExpressionConstant;

class ExpressionValue : public ExpressionNode
{
    public:
        ExpressionValue() {};
        virtual ~ExpressionValue() {};

        virtual QVariant val() const = 0;
};

class ExpressionConstant : public ExpressionValue
{
    public:
        ExpressionConstant()
        {
        };
        ExpressionConstant(const QVariant &v)
        {
            set(v);
        };

        void set(const QVariant &v)
        {
            m_value = v;
        };

        virtual QVariant val() const
        {
            return m_value;
        };

    private:
        QVariant m_value;
};

class VariableNode : public ExpressionValue
{
    public:
        VariableNode(int varIdx, Expression *e) : m_idx(varIdx), m_expr(e)
        {
        };
        virtual ~VariableNode()
        {
        };

        int index() const
        {
            return m_idx;
        };

        virtual QVariant val() const
        {
            if (m_expr && m_expr->m_vars) {
                return m_expr->m_vars->getVariableValue(m_idx);
            }

            return QVariant();
        };

    private:
        int m_idx;
        Expression *m_expr;
};

class ReferenceNode : public ExpressionValue
{
public:
    ReferenceNode(ExpressionReference *ref, Expression *e) : m_ref(ref), m_expr( e )
    {
    };
    virtual ~ReferenceNode()
    {
        delete m_ref;
    };

    QString ref() const
    {
        if ( m_ref )
            return m_ref->ref();

        return QString();
    };

    virtual QVariant val() const
    {
        if ( m_ref )
            return m_ref->getReferenceValue( m_expr->m_vars );

        return QVariant();
    };

private:
    ExpressionReference *m_ref;
    Expression *m_expr;
};


class OperationNode : public ExpressionNode
{
    public:
        OperationNode(int opIndex) : m_op(opIndex)
        {
        }

        int op() const
        {
            return m_op;
        }

        bool isValid() const
        {
            return Expression::isOperator(m_op) || Expression::isFunction(m_op);
        }
    private:
        int m_op;
};

QVariant operator+ (const QVariant &v1, const QVariant &v2)
{
    if (v1.type() == QVariant::Int && v2.type() == QVariant::Int) {
        return QVariant(v1.toInt() + v2.toInt() );
    } else if ( (v1.type() == QVariant::Int || v1.type() == QVariant::Double) &&
                 (v2.type() == QVariant::Int || v2.type() == QVariant::Double) ) {
        return QVariant(v1.toDouble() + v2.toDouble() );
    }

    return QVariant(); // Invalid
}

QVariant operator- (const QVariant &v1, const QVariant &v2)
{
    if (v1.type() == QVariant::Int && v2.type() == QVariant::Int) {
        return QVariant(v1.toInt() - v2.toInt() );
    } else if ( (v1.type() == QVariant::Int || v1.type() == QVariant::Double) &&
                 (v2.type() == QVariant::Int || v2.type() == QVariant::Double) ) {
        return QVariant(v1.toDouble() - v2.toDouble() );
    }

    return QVariant(); // Invalid
}

QVariant operator* (const QVariant &v1, const QVariant &v2)
{
    if (v1.type() == QVariant::Int && v2.type() == QVariant::Int) {
        return QVariant(v1.toInt() * v2.toInt() );
    } else if ( (v1.type() == QVariant::Int || v1.type() == QVariant::Double) &&
                 (v2.type() == QVariant::Int || v2.type() == QVariant::Double) ) {
        return QVariant(v1.toDouble() * v2.toDouble() );
    }

    return QVariant(); // Invalid
}

QVariant operator/ (const QVariant &v1, const QVariant &v2)
{
    if (v1.type() == QVariant::Int && v2.type() == QVariant::Int && v2.toInt() != 0) {
        if (v1.toInt() % v2.toInt() == 0)
            return QVariant(v1.toInt() / v2.toInt() );
        else
            return QVariant(v1.toDouble() / v2.toDouble() );
    } else if ( (v1.type() == QVariant::Int || v1.type() == QVariant::Double) &&
                 (v2.type() == QVariant::Int || v2.type() == QVariant::Double) &&
                         v2.toDouble() != 0) {
        return QVariant(v1.toDouble() / v2.toDouble() );
    }

    return QVariant(); // Invalid
}

QVariant operator% (const QVariant &v1, const QVariant &v2)
{
    if (v1.type() == QVariant::Int && v2.type() == QVariant::Int && v2.toInt() != 0) {
        return QVariant(v1.toInt() % v2.toInt() );
    }

    return QVariant(); // Invalid
}

QVariant max(const QVariant &v1, const QVariant &v2)
{
    if (v1.type() == QVariant::Int && v2.type() == QVariant::Int) {
        int v1i = v1.toInt();
        int v2i = v2.toInt();
        return QVariant((v1i > v2i) ? v1i : v2i);
    } else if ( (v1.type() == QVariant::Int || v1.type() == QVariant::Double) &&
                 (v2.type() == QVariant::Int || v2.type() == QVariant::Double) ) {
        double v1f = v1.toDouble();
        double v2f = v2.toDouble();
        return QVariant((v1f > v2f) ? v1f : v2f);
    }

    return QVariant(); // Invalid
}

QVariant min(const QVariant &v1, const QVariant &v2)
{
    if (v1.type() == QVariant::Int && v2.type() == QVariant::Int) {
        int v1i = v1.toInt();
        int v2i = v2.toInt();
        return QVariant((v1i < v2i) ? v1i : v2i);
    } else if ( (v1.type() == QVariant::Int || v1.type() == QVariant::Double) &&
                 (v2.type() == QVariant::Int || v2.type() == QVariant::Double) ) {
        double v1f = v1.toDouble();
        double v2f = v2.toDouble();
        return QVariant((v1f < v2f) ? v1f : v2f);
    }

    return QVariant(); // Invalid
}


QVariant rgb(const QVariant &v1, const QVariant &v2, const QVariant &v3)
{
    int r, g, b;
    if (v1.type() == QVariant::Int) {
        r = v1.toInt();
    } else if (v1.type() == QVariant::Double) {
        r = (int) v1.toDouble();
    } else {
        return QVariant();
    }
    if (v2.type() == QVariant::Int) {
        g = v2.toInt();
    } else if (v2.type() == QVariant::Double) {
        g = (int) v2.toDouble();
    } else {
        return QVariant();
    }
    if (v3.type() == QVariant::Int) {
        b = v3.toInt();
    } else if (v3.type() == QVariant::Double) {
        b = (int) v3.toDouble();
    } else {
        return QVariant();
    }

    return QVariant(QColor(r,g,b));
}

QVariant rgba(const QVariant &v1, const QVariant &v2, const QVariant &v3, const QVariant &v4)
{
    int r, g, b, a;
    if (v1.type() == QVariant::Int) {
        r = v1.toInt();
    } else if (v1.type() == QVariant::Double) {
        r = (int) v1.toDouble();
    } else {
        return QVariant();
    }
    if (v2.type() == QVariant::Int) {
        g = v2.toInt();
    } else if (v2.type() == QVariant::Double) {
        g = (int) v2.toDouble();
    } else {
        return QVariant();
    }
    if (v3.type() == QVariant::Int) {
        b = v3.toInt();
    } else if (v3.type() == QVariant::Double) {
        b = (int) v3.toDouble();
    } else {
        return QVariant();
    }
    if (v4.type() == QVariant::Int) {
        a = v4.toInt();
    } else if (v4.type() == QVariant::Double) {
        a = (int) v4.toDouble();
    } else {
        return QVariant();
    }

    return QVariant(QColor(r,g,b,a));
}

QVariant blend(const QVariant &v1, const QVariant &v2, const QVariant &v3)
{
    if (v1.type() != QVariant::Color || v2.type() != QVariant::Color) {
        return QVariant();
    }

    return QVariant(); // TODO
}

QVariant adjustHsv(const QVariant &v1, const QVariant &v2, const QVariant &v3, const QVariant &v4)
{
    if (v1.type() != QVariant::Color) {
        return QVariant();
    }

    int adjH, adjS, adjV;
    if (v2.type() == QVariant::Int || v2.type() == QVariant::Double) {
        adjH = v2.toInt();
    } else {
        return QVariant();
    }
    if (v3.type() == QVariant::Int || v3.type() == QVariant::Double) {
        adjS = v3.toInt();
    } else {
        return QVariant();
    }
    if (v4.type() == QVariant::Int || v4.type() == QVariant::Double) {
        adjV = v4.toInt();
    } else {
        return QVariant();
    }

    int h,s,v,a;
    v1.value<QColor>().getHsv(&h,&s,&v,&a);
    QColor c;
    c.setHsv(h+adjH,s+adjS,v+adjV,a);
    return QVariant(c);
}

QVariant dark(const QVariant &v1, const QVariant &v2)
{
    if (v1.type() != QVariant::Color) {
        return QVariant();
    }

    int dark;
    if (v2.type() == QVariant::Int) {
        dark = v2.toInt();
    } else if (v2.type() == QVariant::Double) {
        dark = (int) v2.toDouble();
    } else {
        return QVariant();
    }

    return QVariant(v1.value<QColor>().dark(dark) );
}

QVariant light(const QVariant &v1, const QVariant &v2)
{
    if (v1.type() != QVariant::Color) {
        return QVariant();
    }

    int light;
    if (v2.type() == QVariant::Int) {
        light = v2.toInt();
    } else if (v2.type() == QVariant::Double) {
        light = (int) v2.toDouble();
    } else {
        return QVariant();
    }

    return QVariant(v1.value<QColor>().light(light) );
}


Expression::Expression()
    : m_const( false ), m_valid( false ), m_vars( 0 ), m_evalLock( false )
{
}

Expression::~Expression()
{
    clearExpression();
}

void Expression::clearExpression()
{
    ExpressionNode *n;
    foreach (n, m_expr) {
        delete n;
    }
    m_expr.clear();
    m_postfixExpStr = QString();
    m_const = true;
    m_valid = false;
}

bool Expression::isConstant() const
{
    return m_const;
}

bool Expression::isValid() const
{
    return m_valid;
}

QVariant Expression::evaluate(const ExpressionVariableBridge *vars) const
{
    if (!isValid() || m_expr.size()<=0 || m_evalLock)
        return QVariant(); // invalid result

    // if the expression consists of only one constant, then return the value directly
    const ExpressionValue *v;
    if (m_const && m_expr.size() == 1 && (v = dynamic_cast<ExpressionConstant*>(m_expr.at(0)) ) ) {
        return v->val();
    }

    m_evalLock = true;

    m_vars = vars;

    bool evalError = false;

    QStack<QVariant> stack;

    ExpressionNode *n;
    foreach (n, m_expr) {
        if (OperationNode *on = dynamic_cast<OperationNode*>(n) ) {
            int o = on->op();
            switch (o) {
                case oAdd:
                case oSub:
                case oMult:
                case oDiv:
                case oMod:
                case fMin:
                case fMax:
                case fLight:
                case fDark:
                {
                    if (!stack.count() >= 2) {
                        qCritical("Two arguments expected for operation %s",
                                  qPrintable(operationStr(o)) );
                        evalError = true;
                        break;
                    }
                    QVariant c2 = stack.pop();
                    QVariant c1 = stack.pop();

                    if (o==oAdd)
                        stack.push(c1 + c2);
                    else if (o==oSub)
                        stack.push(c1 - c2);
                    else if (o==oMult)
                        stack.push(c1 * c2);
                    else if (o==oDiv)
                        stack.push(c1 / c2);
                    else if (o==oMod)
                        stack.push(c1 % c2);
                    else if (o==fMin)
                        stack.push(min(c1, c2) );
                    else if (o==fMax)
                        stack.push(max(c1, c2) );
                    else if (o==fLight)
                        stack.push(light(c1, c2) );
                    else if (o==fDark)
                        stack.push(dark(c1, c2) );

                    break;
                }
                case fRgb:
                case fBlend:
                {
                    if (!stack.count() >= 3) {
                        qCritical("Three arguments expected for operation %s",
                                  qPrintable(operationStr(o)) );
                        evalError = true;
                        break;
                    }
                    QVariant c3 = stack.pop();
                    QVariant c2 = stack.pop();
                    QVariant c1 = stack.pop();

                    if (o==fRgb)
                        stack.push(rgb(c1, c2, c3) );
                    else if (o==fBlend)
                        stack.push(blend(c1, c2, c3) );

                    break;
                }
                case fRgba:
                case fAdjustHsv:
                {
                    if (!stack.count() >= 4) {
                        qCritical("Four arguments expected for operation %s",
                                  qPrintable(operationStr(o)) );
                        evalError = true;
                        break;
                    }
                    QVariant c4 = stack.pop();
                    QVariant c3 = stack.pop();
                    QVariant c2 = stack.pop();
                    QVariant c1 = stack.pop();

                    if (o==fRgba)
                        stack.push(rgba(c1, c2, c3, c4) );
                    else if (o==fAdjustHsv)
                        stack.push(adjustHsv(c1, c2, c3, c4) );

                    break;
                }
                default:
                    qCritical("Unexpected operation index %d", o);
                    evalError = true;
                    break;
            }
        } else if (ExpressionValue *v = dynamic_cast<ExpressionValue*>(n) ) {
            stack.push(v->val() );
        } else {
            qCritical("Unexpected expression node");
            evalError = true;
        }
    }

    m_vars = 0;

    m_evalLock = false;

    if (evalError) {
        qCritical("Evaluation error(s) in '%s'!", qPrintable(m_postfixExpStr) );
        return QVariant();
    }

    return stack.pop();
}

Expression &Expression::parse(const QString &expr,
                              const ExpressionVariableIndexFactory *varIdxs)
{
    clearExpression();

    // used for expression validation
    ParseContext nodeExpect = ValueNode;

    // whether the expression contains no variables/references. in this case
    // it's enough to calculate it after parsing. otherwise it's going to
    // be evaluated at 'run time'.
    m_const = true;

    int len = expr.length();
    int parsePos = 0;
    bool parseError = false;

    QStack<int> stack;

    // Create an expression in Postfix notation so that it will be easy to
    // evaluate it later
    while (!parseError && parsePos <= (len-1) ) {

        QChar c = expr[parsePos];

        // Skip white space characters
        if (c.isSpace() ) {
            ++parsePos;
            continue;
        }

        if (c.isDigit() || ((nodeExpect==ValueNode) && (c == QChar('+') || c == QChar('-')) ) ) {
            // check for valid position
            if (nodeExpect != ValueNode) {
                qCritical("No value expected at pos %d", parsePos );
                parseError = true;
                continue;
            }

            QString number;
            bool fl = false;
            number += c;
            while (parsePos < (len-1) && ((c=expr[parsePos+1]).isDigit() || c == QChar('.')) ) {
                ++parsePos;
                if (c == QChar('.') )
                    fl = true;
                number += c;
            }

            bool ok;
            ExpressionNode *node = 0;
            if (fl) {
                float v = number.toFloat(&ok);
                if (ok)
                    node = new ExpressionConstant(v);
            } else {
                int v = number.toInt(&ok);
                if (ok)
                    node = new ExpressionConstant(v);
            }
            if (ok) {
                m_expr.append(node);
                nodeExpect = OperatorNode;
            } else {
                qCritical("Unable to convert '%s' to number at pos %d",
                          qPrintable(number), parsePos );
                parseError = true;
                continue;
            }

        } else if (c == '#') {
            // check for valid position
            if (nodeExpect != ValueNode) {
                qCritical("No value expected at pos %d", parsePos );
                parseError = true;
                continue;
            }

            QString color;
            color += c;
            while (parsePos < (len-1)
                   && ((c=expr[parsePos+1]).isLetterOrNumber()) ) {
                ++parsePos;
                color += c;
            }

            QColor c(color);
            if (c.isValid() ) {
                m_expr.append(new ExpressionConstant(c) );
                nodeExpect = OperatorNode;
            } else {
                qCritical("Unable to convert '%s' to color at pos %d",
                          qPrintable(color), parsePos );
                parseError = true;
                continue;
            }

        } else if (c.isLetter() || c=='.' ) {
            // check for valid position
            if (nodeExpect != ValueNode) {
                qCritical("No value expected at pos %d", parsePos );
                parseError = true;
                continue;
            }

            bool isReference = false;

            QString identifier;
            identifier += c;
            if (c == '.')
                isReference = true;
            while (parsePos < (len-1)
                   && ((c=expr[parsePos+1]).isLetterOrNumber() || c=='.') ) {
                ++parsePos;
                identifier += c;
                if (c == '.')
                    isReference = true;
            }

            if (isReference) {
                m_const = false;
                ExpressionReference *ref = getReference( identifier );
                if ( ref ) {
                    m_expr.append(new ReferenceNode(ref, this) );
                    nodeExpect = OperatorNode;
                } else {
                    qCritical( "Invalid reference identifier '%s' at pos %d.",
                               qPrintable( identifier ), parsePos );
                    parseError = true;
                    continue;
                }
            } else if ( getFunction(identifier) != -1) {
                stack.push(getFunction(identifier) );
                nodeExpect = ParenthesisOpenNode;
            } else {
                m_const = false;
                int idx = -1;
                if (varIdxs) {
                    idx = varIdxs->getVariableIndex(identifier);
                }
                if (idx == -1) {
                    qCritical("Invalid variable identifier '%s' at pos %d.",
                              qPrintable(identifier), parsePos );
                    parseError = true;
                    continue;
                }
                m_expr.append(new VariableNode(idx, this) );
                nodeExpect = OperatorNode;
            }

        } else if (c == ',') {
            // check for valid position
            if (nodeExpect != OperatorNode) {
                qCritical("No function argument separator expected at pos %d",
                          parsePos );
                parseError = true;
                continue;
            }

            while (!stack.isEmpty() &&
                    stack.top() != pLeftPar) {
                if (isOperator(stack.top()) ||
                    isFunction(stack.top()) ) {
                    m_expr.append(new OperationNode(stack.pop()) );
                } else {
                    qCritical("Operator or function expected while pop'ing items back from the stack." );
                    parseError = true;
                    continue;
                }
            }

            if (stack.isEmpty() ) {
                qCritical("No '(' found; function separator was misplaced or parentheses were mismatched at pos %d",
                          parsePos );
                parseError = true;
                continue;
            }

            nodeExpect = ValueNode;

        }else if (getOperator(c) != -1) {
            // check for valid position
            if (nodeExpect != OperatorNode) {
                qCritical("No operator expected at pos %d", parsePos );
                parseError = true;
                continue;
            }

            int o = getOperator(c);

            while (!stack.isEmpty() &&
                    isOperator(stack.top()) &&
                    operatorPrecedence(stack.top()) <= operatorPrecedence(o) ) {
                m_expr.append(new OperationNode(stack.pop() ) );
            }

            stack.push(o);

            nodeExpect = ValueNode;

        } else if (c == '(') {
                // check for valid position
            if (nodeExpect != ValueNode && nodeExpect != ParenthesisOpenNode) {
                    qCritical("No '(' expected at pos %d", parsePos );
                    parseError = true;
                    continue;
                }

                stack.push(pLeftPar);
                nodeExpect = ValueNode;
        } else if (c == ')') {
                // check for valid position
            if (nodeExpect != OperatorNode) {
                    qCritical("No ')' expected at pos %d", parsePos);
                    parseError = true;
                    continue;
                }

                while (!stack.isEmpty() &&
                        stack.top() != pLeftPar) {
                    if ( isOperator(stack.top()) ||
                         isFunction(stack.top()) ) {
                        m_expr.append(new OperationNode(stack.pop()) );
                    } else {
                        qCritical("Operator or function expected while pop'ing items back from the stack." );
                        parseError = true;
                        continue;
                    }
                }

                if (!stack.isEmpty() && stack.top() == pLeftPar) {
                    stack.pop();
                } else {
                    qCritical("No '(' found; parentheses mismatched at pos %d",
                              parsePos);
                    parseError = true;
                    continue;
                }

                nodeExpect = OperatorNode;

        } else {
            qCritical("Unrecognized character at pos %d",
                      parsePos);
            parseError = true;
            continue;
        }

        ++parsePos;
    }

    while (!parseError && !stack.isEmpty() ) {
        if (isOperator(stack.top()) ||
            isFunction(stack.top()) ) {
            m_expr.append(new OperationNode(stack.pop() ) );
        } else if (stack.top() == pLeftPar) {
            qCritical("Parenthesis mismatch; superfluous '('!");
            parseError = true;
            continue;
        } else {
            qCritical("Unexpected object while pop'ing items back from the stack'!");
            parseError = true;
            continue;
        }
    }

    if (parseError) {
        qCritical("Parse error(s) in '%s'!", qPrintable(expr) );

        // cleanup
        stack.clear();

        m_valid = false;
    } else {
        m_valid = true;
    }

    m_postfixExpStr = postfixNotation();

    if (m_const) {
        QVariant constValue = evaluate();

        // clear the non-evaluated expression and replace it with the
        // constant value
        ExpressionNode *n;
        foreach (n, m_expr) {
            delete n;
        }
        m_expr.clear();

        m_expr.append(new ExpressionConstant(constValue) );
    }

    return *this;
}

const QString &Expression::expression() const
{
    return m_postfixExpStr;
}

QString Expression::postfixNotation() const
{
    // transform nodes to string
    QString post;
    ExpressionNode *n;
    foreach (n, m_expr) {
        if (ExpressionConstant* v = dynamic_cast<ExpressionConstant*>(n) ) {
            if (v->val().type() == QVariant::Int)
                post += QString(" %1").arg(v->val().toInt() );
            else if (v->val().type() == QVariant::Double)
                post += QString(" %1").arg(v->val().toDouble() );
            else if (v->val().type() == QVariant::Color)
                post += QString(" %1").arg(v->val().value<QColor>().name() );
        } else if (OperationNode* o = dynamic_cast<OperationNode*>(n) ) {
            post += " " + operationStr(o->op());
        } else if (VariableNode* v = dynamic_cast<VariableNode*>(n) ) {
            post += QString(" v%1").arg(v->index() );
        }
    }
    return post.trimmed();
}

uint Expression::operatorPrecedence(int o)
{
    if (o==oAdd || o==oSub)
        return 4;
    else if (o==oMult || o==oDiv || o==oMod)
        return 3;
    else
        return 99;
}

int Expression::getOperator(const QChar &c)
{
    if (c=='+')
        return oAdd;
    else if (c=='-')
        return oSub;
    else if (c=='*')
        return oMult;
    else if (c=='/')
        return oDiv;
    else if (c=='%')
        return oMod;
    else
        return -1;
}

bool Expression::isOperator(int index)
{
    return index >= oAdd && index < oLast;
}

QString Expression::operationStr(int index)
{
    switch (index) {
        case oAdd:       return "+";
        case oSub:       return "-";
        case oMult:      return "*";
        case oDiv:       return "/";
        case oMod:       return "%";
        case fMax:       return "max";
        case fMin:       return "min";
        case fRgb:       return "rgb";
        case fRgba:      return "rgba";
        case fBlend:     return "blend";
        case fAdjustHsv: return "adjustHsv";
        case fDark:      return "dark";
        case fLight:     return "light";
        default:         break;
    }
    return "[no operation!]";
}

int Expression::getFunction(const QString &str)
{
    if (str == "max")
        return fMax;
    else if (str == "min")
        return fMin;
    else if (str == "rgb")
        return fRgb;
    else if (str == "rgba")
        return fRgba;
    else if (str == "blend")
        return fBlend;
    else if (str == "adjustHsv")
        return fAdjustHsv;
    else if (str == "dark")
        return fDark;
    else if (str == "light")
        return fLight;
    else
        return -1;
}

bool Expression::isFunction(int index)
{
    return index >= fMin && index < fLast;
}

ExpressionReference *Expression::getReference(const QString &ref) const
{
    return 0;
};


ExpressionVariableIndexFactory::ExpressionVariableIndexFactory() {}
ExpressionVariableIndexFactory::~ExpressionVariableIndexFactory() {}

ExpressionReference::ExpressionReference() {}
ExpressionReference::~ExpressionReference() {}

}

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
