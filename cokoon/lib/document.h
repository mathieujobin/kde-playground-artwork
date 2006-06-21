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

#ifndef COKOON_THEMEDOCUMENT_H
#define COKOON_THEMEDOCUMENT_H

#include <QString>
#include <QHash>

class QPainter;

#include <kdelibs_export.h>
#include "bridges.h"

/**
 * \brief The namespace in which all cokoon theme library related classes are contained.
 * CoKoon's aims are:
 * @li Combine the advantages of SVG (scalable vector graphics) with layout
 *   capabilities as known from pixmap theme engines.
 * @li Make it usable in a variety of applications (such as KDE styles,
 *   KWin decorations, Custom widgets, Application themes...)
 * @li Keep the theme file format simple enough to make future GUI theme editor
 *   applications possible,
 * @li but powerful enough to be able to achieve tasks which would only have been
 *   possible with programmatically coded themes previously.
 */
namespace Cokoon {

class SpecialCellBridge;
class Object;

class DocumentPrivate;

/**
 * The core of the theme engine. This class stores "theme objects" which
 * are accessible with obj().
 *
 * Theme specification is done using declareObject().
 *
 * Document is mainly designed to be created first and then be read -
 * modifications (e.g. deletion of individual objects) can only be achieved by
 * clearing the current document and creating a new, updated, one.
 * TODO: Overview of the document structure, object inheritance, ...
 *
 * @author Sandro Giessl <giessl@kde.org>
 */
class KDEFX_EXPORT Document
{
    public:
        /**
         * Construct.
         */
        Document();
        /**
         * Destruct.
         */
        virtual ~Document();

        /**
         * @brief Used to 'declare' items and states.
         *
         * A theme specification can declare several 'items' that a theme
         * should define. An item can have several 'states'. States are separated
         * in 'state levels', so the actual state of an item is the combination
         * of one state from each state level. For example, the item 'Button'
         * could have the following states:
         * @verbatim
'item' | 'states 0' | 'states 1'
-------|------------|----------
       |  enabled   |  normal
Button +            +  pressed
       |  disabled  |  hover @endverbatim
         *
         * This method normally doesn't need to be called manually. Usually a
         * theme specification is defined in an XML file, which is then
         * converted to an include file (using tools/gen-themespec-include.rb).
         * In this file, a function SpecNameSpace::setupItems() is defined, which
         * takes care of the theme specification.
         * 
         * This is used to provide initial
         * states for object id autocompletion in themes.
         * Additionally, it offers access to object based on int ids.
         * @param objName The id of the "root item".
         * @param objStates A QList of a QHash with several state levels. The hash
         * key are all states of a specific state level. The hash value are the
         * ids of the state.
         * @return the int id of the "root item".
         * @sa obj(int)
         */
        int declareObject(const QString &objName, QList<QHash<QString,int> > objStates);

        /**
         * Used to 'declare' identifiers.
         *
         * In order to use variables in expressions from a theme, they need
         * to be declared first. This is usually done automatically by
         * the auto-generated theme specification in SpecNameSpace::setupItems().
         * @param varId The identifier of the variable.
         * @param varIndex The index used to access the variable value.
         * @sa declareObject()
         * @sa ExpressionVariableBridge
         */
        void declareVariable(const QString &varId, int varIndex);

        /**
         * Used to 'declare' identifiers.
         *
         * In order to access object properties by id, they need to be
         * declared before loading a theme. This is usually done
         * automatically by the auto-generated theme specification in
         * SpecNameSpace::setupItems().
         *
         * @param id The identifier of the expression.
         * @param index The index used to access the expression. If -1, an
         * id will be chosen automatically.
         * @return The index of the newly declared identifier.
         * 
         */
        int declareIdentifier(const QString &id, int index = -1);

        /**
         * @return the index of variable @p id, or -1 if the
         * variable has not been declared yet.
         */
        int getVariableIndex(const QString &id) const;

        /**
         * @return The index of identifier @p id , or -1 if the identifier has not
         * been declared yet.
         */
        int getIdentifierIndex(const QString &id) const;

        /**
         * @returns The id of the "root item", or -1 if it has not been declared.
         * @sa declareObject()
         */
        int declarationItemId(const QString &objName) const;
	/**
	 * @return the count of state levels for item @p itemId
	 */
	int declarationItemStateLevels(int itemId) const;
        /**
         * @param objId The id of the "root item"
         * @param level The state level.
         * @returns The list of states of a specific state level.
         * @sa declareObject()
         */
        const QHash<QString,int> declarationStates(int objId, int level);

        /**
         * Read file @p themeFile and construct a theme document from it.
         *
         * Be sure to make the theme specification objects and states available
         * first, using declareObject() ; otherwise, e.g. the id autocompletion
         * for definition of multiple spec objects at once will not
         * work - it's likely that this will break most themes!
         */
        void loadTheme(const QString &themeFile);

        /**
         * Deletes all objects.
         */
        void clear();

        /**
         * Paints all the layouts of Object @p objId which are declared
         * as 'paint layers'.
	 * @param objId the paint operation will happen in this
	 * object's context
         * @param p used to paint the layers
         * @param left the area in which the layers are painted in
         * @param left the area in which the layers are painted in
         * @param left the area in which the layers are painted in
         * @param left the area in which the layers are painted in
         * @param vars used to make variables accessible in
         * expressions which need to be evaluated within this paint operation
         */
        void drawLayers(int objId,
			QPainter *p, int left, int top, int width, int height,
			const ExpressionVariableBridge *vars = 0,
			const SpecialCellBridge *cells = 0) const;

        /**
         * Paints the layout @p layoutId.
	 * @param objId the paint operation will happen in this
	 * object's context
	 * @param layoutId the layout which will be painted
         * @param p used to paint the layout
         * @param left the area in which the layout is painted in
         * @param left the area in which the layout is painted in
         * @param left the area in which the layout is painted in
         * @param left the area in which the layout is painted in
         * @param vars used to make variables accessible in
         * expressions which need to be evaluated within this paint operation
         */
        void drawLayout(int objId, int layoutId,
			QPainter *p, int left, int top, int width, int height,
			const ExpressionVariableBridge *vars = 0,
			const SpecialCellBridge *cells = 0) const;

	/**
	 * Gives access to the tile @p tileId.
	 * @param objId the process of extracting the tile from the
	 * tile source will happen in this object's context
	 * @param tileId the identifier of the layout
         * @param vars used to make variables accessible in
         * expressions which need to be evaluated
	 * @retun tile pixmap
	 */
	QPixmap getTilePixmap(int objId, int tileId,
			      const ExpressionVariableBridge *vars = 0) const;

	/**
	 * Gives access to the value of the expression @p
	 * expressionId.
	 * @param objId expression evaluation will happen in this
	 * object's context
	 * @param expressionId the identifier of the expression
         * @param vars used to make variables accessible in
         * expression(s) which need to be evaluated
	 * @return expression value
	 */
	QVariant getExpValue(int objId, int expressionId,
			     const ExpressionVariableBridge *vars = 0) const;

    private:
        friend class Object;
	friend class DocumentHandler;
	friend class ObjectRefBridge;
	friend class CustomExpBase;

        /**
         * Insert an object into the document. The document will take care of
         * memory management -- if everything works as expected. If the object
         * could not be inserted (e.g. because the @p objId already exists),
         * it will do nothing, so be sure to check Object::docRefCount() after
         * insertion and free memory yourself.
         * @param objId The absolute object identification. It is expected to
         * have the following form (Button example from declareObject() ): @verbatim
'item'.'state 0'.'state 1'
Button.enabled.normal
Button.disabled.pressed
etc. @endverbatim
         * @param obj The actual object.
         */
        void insertObject(const QString &objId, Object *obj);

        /**
         * @param id The absolute id of the object.
         * @return The object or 0 if it doesn't exist.
         * @sa insertObject()
         */
        const Object *obj(const QString &id) const;

        /**
         * This is the preferred access method for clients using the theme library.
         * @param id The id of the object. It is calculated from enum values
         * which have been generated from the theme specification. Calculation
         * is done like e.g.: @verbatim
Button + Button0_enabled  + Button1_normal
Button + Button0_disabled + Button1_pressed
etc. @endverbatim
         * @return The object or 0 if it doesn't exist.
         * @sa declareObject()
         */
        const Object *obj(int id) const;

	void refObj(Object *o);
	void unrefObj(Object *o);

        DocumentPrivate * const d;
};

}

#endif

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
