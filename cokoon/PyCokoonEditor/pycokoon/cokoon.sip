// Define the SIP wrapper to the word library.

%Module cokoon 0

// %Import QtCore/QtCoremod.sip
%Import QtGui/QtGuimod.sip

namespace Cokoon {

%TypeHeaderCode
#include <cokoon/bridges.h>
#include <cokoon/document.h>
%End

class ExpressionVariableBridge
{

// %TypeHeaderCode
// #include <cokoon/bridges.h>
// %End

    public:
        ExpressionVariableBridge();
        virtual ~ExpressionVariableBridge();

        virtual QVariant getVariableValue(int idx) const = 0;
};

};

namespace Cokoon {

class SpecialCellBridge
{

// %TypeHeaderCode
// #include <cokoon/bridges.h>
// %End

    public:
        SpecialCellBridge();
        virtual ~SpecialCellBridge();
        virtual void paintCell(int id, QPainter *p,
                               int left, int top, int width, int height) const = 0;
};

};

namespace Cokoon {

class Document {

%TypeHeaderCode
#include <cokoon/document.h>
#include <cokoon/bridges.h>
%End

    public:

        Document();
        virtual ~Document();
        void loadTheme(const QByteArray &data, const QString &path);
        void loadTheme(const QString &data, const QString &path);
        void loadTheme(const QString &themeFile);
        void clear();
        void drawLayers(int objId,
			QPainter *p, int left, int top, int width, int height,
			const Cokoon::ExpressionVariableBridge *vars = 0,
			const Cokoon::SpecialCellBridge *cells = 0) const;
        void drawLayout(int objId, int layoutId,
			QPainter *p, int left, int top, int width, int height,
			const Cokoon::ExpressionVariableBridge *vars = 0,
			const Cokoon::SpecialCellBridge *cells = 0) const;
	QPixmap getTilePixmap(int objId, int tileId,
			      const Cokoon::ExpressionVariableBridge *vars = 0) const;
	QVariant getExpValue(int objId, int expressionId,
			     const Cokoon::ExpressionVariableBridge *vars = 0) const;

        enum DeclarationType {
            ObjectNameDecl,
            ObjectStateDecl,
            VariableDecl,
            IdentifierDecl
        };

protected:

        virtual int mapToId(DeclarationType type, const QString &str) const;
        virtual int objectStateLevels(int objId) const;
        virtual int objectStateLevelStates(int objId, int stateLevel) const;
        virtual int mapObjectStateToId(int objId, int stateLevel, const QString &stateName) const = 0;
        virtual int mapObjectStateToId(int objId,int stateLevel, int stateNumber) const = 0;
        virtual int customIdMappingBase(DeclarationType type) const;
        int declareIdMapping(DeclarationType type, const QString &str);

};


};
