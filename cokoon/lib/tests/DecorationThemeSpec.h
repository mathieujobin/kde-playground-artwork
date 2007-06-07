#ifndef DecorationThemeSpec_H
#define DecorationThemeSpec_H
namespace DecorationThemeSpec {
namespace Var {
enum Variables {
  testVar,
  Variables__Count
};
}
enum Identifiers {
  Exp_oneplusone,
  Exp_variableExpression,
  Exp_objectExp,
  Exp_objectExp2,
  Exp_objectExp3,
  Exp_recursiveLoopTest1,
  Exp_recursiveLoopTest2,
  Exp_recursiveLoopTest3,
  Identifiers__Count
};
enum Items {
  Decoration = 0,
  Button = 1,
  Items__Count
};
enum DecorationStates0 {
  Decoration0_active = 0,
  Decoration0_inactive = 2
};
enum DecorationStates1 {
  Decoration1_normal = 0,
  Decoration1_max = 4,
  Decoration1_maxHor = 8,
  Decoration1_maxVert = 12
};
enum DecorationStates2 {
  Decoration2_normal = 0,
  Decoration2_tool = 16
};
enum DecorationStates3 {
  Decoration3_normal = 0,
  Decoration3_shaded = 32
};
enum ButtonStates0 {
  Button0_close = 0,
  Button0_minimize = 2,
  Button0_menu = 4,
  Button0_maximize = 6,
  Button0_maxRestore = 8,
  Button0_maxHorRestore = 10,
  Button0_maxVertRestore = 12,
  Button0_shade = 14,
  Button0_shadeRestore = 16,
  Button0_above = 18,
  Button0_aboveRestore = 20,
  Button0_below = 22,
  Button0_belowRestore = 24,
  Button0_allDesktops = 26,
  Button0_allDesktopsRestore = 28
};
enum ButtonStates1 {
  Button1_normal = 0,
  Button1_pressed = 32,
  Button1_prelight = 64
};
enum ButtonStates2 {
  Button2_active = 0,
  Button2_inactive = 128
};
enum ButtonStates3 {
  Button3_normal = 0,
  Button3_tool = 256
};
class SpecDocument : public Cokoon::Document {
protected:
int mapToId(Cokoon::Document::DeclarationType type,const QString &str) const {
  switch(type) {
  case Cokoon::Document::ObjectNameDecl:
    if (false) { return -1; }
    else if (str == "Decoration") { return Decoration; }
    else if (str == "Button") { return Button; }
    break;
  case Cokoon::Document::VariableDecl:
    if (false) { return -1; }
    else if (str == "testVar") { return Var::testVar; }
    break;
  case Cokoon::Document::IdentifierDecl:
    if (false) { return -1; }
    else if (str == "oneplusone") { return Exp_oneplusone; }
    else if (str == "variableExpression") { return Exp_variableExpression; }
    else if (str == "objectExp") { return Exp_objectExp; }
    else if (str == "objectExp2") { return Exp_objectExp2; }
    else if (str == "objectExp3") { return Exp_objectExp3; }
    else if (str == "recursiveLoopTest1") { return Exp_recursiveLoopTest1; }
    else if (str == "recursiveLoopTest2") { return Exp_recursiveLoopTest2; }
    else if (str == "recursiveLoopTest3") { return Exp_recursiveLoopTest3; }
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
        case Decoration:
            return 4;
        case Button:
            return 4;
        default: return -1;
        }
    }
    int objectStateLevelStates(int itemId, int stateLevel) const {
        switch(itemId) {
        case Decoration:
            switch(stateLevel) {
            case 0: return 2;
            case 1: return 4;
            case 2: return 2;
            case 3: return 2;
            default: return -1;
            }
        case Button:
            switch(stateLevel) {
            case 0: return 15;
            case 1: return 3;
            case 2: return 2;
            case 3: return 2;
            default: return -1;
            }
        default: return -1;
        }
    }
    int mapObjectStateToId(int itemId, int stateLevel, const QString &stateName) const {
        switch(itemId) {
        case Decoration:
            switch(stateLevel) {
            case 0:
              if (stateName == "active") { return Decoration0_active; }
              if (stateName == "inactive") { return Decoration0_inactive; }
                else return -1;
             break;
            case 1:
              if (stateName == "normal") { return Decoration1_normal; }
              if (stateName == "max") { return Decoration1_max; }
              if (stateName == "maxHor") { return Decoration1_maxHor; }
              if (stateName == "maxVert") { return Decoration1_maxVert; }
                else return -1;
             break;
            case 2:
              if (stateName == "normal") { return Decoration2_normal; }
              if (stateName == "tool") { return Decoration2_tool; }
                else return -1;
             break;
            case 3:
              if (stateName == "normal") { return Decoration3_normal; }
              if (stateName == "shaded") { return Decoration3_shaded; }
                else return -1;
             break;
            }
            break;
        case Button:
            switch(stateLevel) {
            case 0:
              if (stateName == "close") { return Button0_close; }
              if (stateName == "minimize") { return Button0_minimize; }
              if (stateName == "menu") { return Button0_menu; }
              if (stateName == "maximize") { return Button0_maximize; }
              if (stateName == "maxRestore") { return Button0_maxRestore; }
              if (stateName == "maxHorRestore") { return Button0_maxHorRestore; }
              if (stateName == "maxVertRestore") { return Button0_maxVertRestore; }
              if (stateName == "shade") { return Button0_shade; }
              if (stateName == "shadeRestore") { return Button0_shadeRestore; }
              if (stateName == "above") { return Button0_above; }
              if (stateName == "aboveRestore") { return Button0_aboveRestore; }
              if (stateName == "below") { return Button0_below; }
              if (stateName == "belowRestore") { return Button0_belowRestore; }
              if (stateName == "allDesktops") { return Button0_allDesktops; }
              if (stateName == "allDesktopsRestore") { return Button0_allDesktopsRestore; }
                else return -1;
             break;
            case 1:
              if (stateName == "normal") { return Button1_normal; }
              if (stateName == "pressed") { return Button1_pressed; }
              if (stateName == "prelight") { return Button1_prelight; }
                else return -1;
             break;
            case 2:
              if (stateName == "active") { return Button2_active; }
              if (stateName == "inactive") { return Button2_inactive; }
                else return -1;
             break;
            case 3:
              if (stateName == "normal") { return Button3_normal; }
              if (stateName == "tool") { return Button3_tool; }
                else return -1;
             break;
            }
            break;
        default: return -1;
        }
    }
    int mapObjectStateToId(int itemId, int stateLevel, int stateNumber) const {
        switch(itemId) {
        case Decoration:
            switch(stateLevel) {
            case 0:
                switch(stateNumber) {
                case 0: return Decoration0_active;
                case 1: return Decoration0_inactive;
                default: return -1;
                }
             break;
            case 1:
                switch(stateNumber) {
                case 0: return Decoration1_normal;
                case 1: return Decoration1_max;
                case 2: return Decoration1_maxHor;
                case 3: return Decoration1_maxVert;
                default: return -1;
                }
             break;
            case 2:
                switch(stateNumber) {
                case 0: return Decoration2_normal;
                case 1: return Decoration2_tool;
                default: return -1;
                }
             break;
            case 3:
                switch(stateNumber) {
                case 0: return Decoration3_normal;
                case 1: return Decoration3_shaded;
                default: return -1;
                }
             break;
            }
        case Button:
            switch(stateLevel) {
            case 0:
                switch(stateNumber) {
                case 0: return Button0_close;
                case 1: return Button0_minimize;
                case 2: return Button0_menu;
                case 3: return Button0_maximize;
                case 4: return Button0_maxRestore;
                case 5: return Button0_maxHorRestore;
                case 6: return Button0_maxVertRestore;
                case 7: return Button0_shade;
                case 8: return Button0_shadeRestore;
                case 9: return Button0_above;
                case 10: return Button0_aboveRestore;
                case 11: return Button0_below;
                case 12: return Button0_belowRestore;
                case 13: return Button0_allDesktops;
                case 14: return Button0_allDesktopsRestore;
                default: return -1;
                }
             break;
            case 1:
                switch(stateNumber) {
                case 0: return Button1_normal;
                case 1: return Button1_pressed;
                case 2: return Button1_prelight;
                default: return -1;
                }
             break;
            case 2:
                switch(stateNumber) {
                case 0: return Button2_active;
                case 1: return Button2_inactive;
                default: return -1;
                }
             break;
            case 3:
                switch(stateNumber) {
                case 0: return Button3_normal;
                case 1: return Button3_tool;
                default: return -1;
                }
             break;
            }
        default: return -1;
        }
    }
};
}
#endif // DecorationThemeSpec_H
