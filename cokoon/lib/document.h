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
class Q_DECL_EXPORT Document
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
         * Read file @p themeFile and construct a theme document from it.
         *
         * Be sure to provide a proper theme specification by subclassing
         * this class.
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

        /**
         * @see mapToId()
         */
        enum DeclarationType {
            ObjectNameDecl, ///< 
            VariableDecl,
            IdentifierDecl
        };

protected:

        /**
         * Cokoon themes specify various items using string identifiers. In order
         * to provide faster access times, they are mapped to integer identifers
         * when loading a theme.
         *
         * Reimplement this Theme Specification method.
         *
         * @param type specifies what kind of identifier should be mapped.
         * @returns the Id of the declaration, or -1 if the identifier @p str has not been declared yet.
         *
         * @sa declareIdMapping()
         *
         */
        virtual int mapToId(DeclarationType type, const QString &str) const;
	/**
         * Reimplement this Theme Specification method.
         *
	 * @returns the count of state levels for object @p objId
         * @sa mapObjectStateToId()
	 */
        virtual int objectStateLevels(int objId) const;
        /**
         * Reimplement this Theme Specification method.
         *
         * @returns the number of states in a state level.
         * @sa mapObjectStateToId()
	 */
        virtual int objectStateLevelStates(int objId, int stateLevel) const;
        /**
         * @brief Theme Specification of possible states for a given object.
         *
         * Every object can have several states. These states can be
         * exclusive (e.g. enabled or disabled) and supplemental
         * (e.g. enabled and normal, or enabled and pressed). This
         * concept is central to Cokoon themes.
         *
         * Exclusive states can be thought of being in the same @p
         * stateLevel, or column, while supplemental states are kept
         * in a separate state level.
         *
         * For example, an object 'Button' could have the following states:
@verbatim
object | level 0    | level 1
-------|------------|----------
       |  enabled   |  normal
Button +            +  pressed
       |  disabled  |  hover @endverbatim
         * In a theme these would expand into six object definitions
         * (Button.enabled.normal, Button.disabled.normal,
         * Button.enabled.pressed, ...). (Note that in theme
         * definitions Button.*.normal would expand into
         * Button.enabled.normal and Button.disabled.normal)
         *
         *Theme Engines are required to implement this method, mapping
         *a specific state to a unique integer ID. This is necessary
         *in order to access specific object states later. It is
         *essential to keep in mind the following:
         *@li Those 'specific
         *object states' are built by adding the values of the object
         *ID, and one state ID from each state
         *level. e.g. "Button.disabled.pressed" would be calculated
         *using
@code
int objId = mapToId(Document::ObjectName,"Button");
int actualObject = objId + mapObjectStateToId(objId,0,"enabled")+
                              mapObjectStateToId(objId,1,"pressed")@endcode
         *@li All object and states expanded, the IDs of every
         *'specific object state' must be unique
         * @li A simple example of valid IDs:
@verbatim
Button   := 0
enabled  := 0*2^1
disabled := 1*2^1
normal   := 0*2^2
pressed  := 1*2^2
hover    := 2*2^2@endverbatim

         * Usually it is not necessary to write the implementation
         * manually, since it can be generated automatically from an
         * XML specification file (e.g. using
         * tools/gen-themespec-include.rb)
         * 
         * @param objId The id of the object.
         * @param stateLevel The state level.
         * @param stateName The name that should be mapped to an ID.
         * @return the int id of the state item, or -1 if the state has
         * not been specified.
         *
         * @sa obj(int)
         */
        virtual int mapObjectStateToId(int objId, int stateLevel, const QString &stateName) const = 0;
        /**
         * Like mapObjectStateToId above, mapping from the state number
         * (within one stateLevel) instead of name string to the absolute ID.
         *
         * @param objId The id of the object.
         * @param stateLevel The state level.
         * @param stateNumber The number (counting from 0) that should
         * be mapped to an ID.
         * @return the int id of the state item, or -1 if the state has
         * not been specified.
         *
         * @sa obj(int)
         */ 
        virtual int mapObjectStateToId(int objId,int stateLevel, int stateNumber) const = 0;

        /**
         * When sublassing Document and overwriting mapToId(), it is usually
         * also necessary to overwrite this method. It will be used
         * in declareIdMapping() to create new unique Ids for custom items
         * declared in themes.
         *
         * @param type specifies the what kind of identifier base is requested.
         *
         * @sa mapToId()
         */
        virtual int customIdMappingBase(DeclarationType type) const;

        /**
         * Used to 'declare' custom identifiers from the Cokoon Theme.
         * Usually one doesn't need to touch this method, when loading
         * themes using loadTheme(), since it will take care of declaring
         * new identifiers.
         *
         * It's not possible to declare identifiers of type VariableDecl,
         * since it only makes sense to declare variables in the
         * theme specification (overwriting mapToId() and customIdMappingBase() ).
         *
         * @param type specifies what kind of identifier should be mapped.
         * @param str The identifier of the expression.
         * @return The index of the newly declared identifier.
         */
        int declareIdMapping(DeclarationType type, const QString &str);

    private:
        friend class Object;
	friend class DocumentHandler;
	friend class ObjectRefBridge;
	friend class CustomExpBase;
        friend class DocumentPrivate;

        /**
         * Insert an object into the document. The document will take care of
         * memory management -- if everything works as expected. If the object
         * could not be inserted (e.g. because the @p objId already exists),
         * it will do nothing, so be sure to check Object::docRefCount() after
         * insertion and free memory yourself.
         * @param objId object ID as declared in mapToId()
         * @param objStates object states as specified in mapObjectStatesToId()
         * @param obj The actual object.
         * @return The new ID used to access the specific object state (0 if objStates have been expanded into multiple states), or -1 in case of an error.
         */
        int insertObject(int objId, const QList<int> &objStates,Object *obj);

        /**
         * @param id The absolute id of the object.
         * @return The object or 0 if it doesn't exist.
         * @sa insertObject()
         */
        const Object *obj(const QString &id) const;

        /**
         * This is the preferred access method for clients using the theme library.
         * @param id The id of the object. It is calculated from enum values
         * which have been gathered from mapToId() and mapObjectStatesToId().
         * @return The object or 0 if it doesn't exist.
         * @sa mapObjectStatesToId()
         */
        const Object *obj(int id) const;

	void refObj(Object *o);
	void unrefObj(Object *o);

        DocumentPrivate * const d;
};

}

#endif

// kate: space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
