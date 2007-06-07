#ifndef ThemeWidgetSpec_H
#define ThemeWidgetSpec_H
namespace ThemeWidgetSpec {
namespace Var {
enum Variables {
  Variables__Count
};
}
enum Identifiers {
  Identifiers__Count
};
enum Items {
  Widget = 0,
  Items__Count
};
class SpecDocument : public Cokoon::Document {
protected:
int mapToId(Cokoon::Document::DeclarationType type,const QString &str) const {
  switch(type) {
  case Cokoon::Document::ObjectNameDecl:
    if (false) { return -1; }
    else if (str == "Widget") { return Widget; }
    break;
  case Cokoon::Document::VariableDecl:
    if (false) { return -1; }
    break;
  case Cokoon::Document::IdentifierDecl:
    if (false) { return -1; }
    break;
  }
return Cokoon::Document::mapToId(type,str); }
int customIdMappingBase(Cokoon::Document::DeclarationType type) const {
  switch(type) {
  case Cokoon::Document::ObjectNameDecl: return Items__Count;
  case Cokoon::Document::VariableDecl:   return Var::Variables__Count;
  case Cokoon::Document::IdentifierDecl: return Identifiers__Count;
  }
return Cokoon::Document::customIdMappingBase(type); }
    int objectStateLevels(int objId) const {
        switch(objId) {
        case Widget:
            return 0;
        default: return -1;
        }
    }
    int objectStateLevelStates(int itemId, int stateLevel) const {
        switch(itemId) {
        case Widget:
            switch(stateLevel) {
            default: return -1;
            }
        default: return -1;
        }
    }
    int mapObjectStateToId(int itemId, int stateLevel, const QString &stateName) const {
        switch(itemId) {
        case Widget:
            switch(stateLevel) {
            }
            break;
        default: return -1;
        }
    }
    int mapObjectStateToId(int itemId, int stateLevel, int stateNumber) const {
        switch(itemId) {
        case Widget:
            switch(stateLevel) {
            }
        default: return -1;
        }
    }
};
}
#endif // ThemeWidgetSpec_H
