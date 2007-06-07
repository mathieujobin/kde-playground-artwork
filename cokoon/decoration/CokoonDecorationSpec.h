#ifndef CokoonDecorationSpec_H
#define CokoonDecorationSpec_H
namespace CokoonDecorationSpec {
namespace Var {
enum Variables {
  BorderSizeFactor,
  ButtonGroupLeftWidth,
  ButtonGroupRightWidth,
  WindowTitleHeight,
  WindowTitleWidth,
  ColorTitleBar,
  ColorTitleBlend,
  ColorFont,
  ColorButtonBg,
  ColorFrame,
  ColorHandle,
  Variables__Count
};
}
enum Identifiers {
  Exp_RespectBorderSizeFactor,
  Exp_CustomButtonSize,
  Exp_MenuDoubleClickClose,
  Exp_WindowTitleAlignment,
  Exp_WindowTitleColor,
  Exp_BorderLeft,
  Exp_BorderRight,
  Exp_BorderBottom,
  Exp_TitleHeight,
  Exp_TitleBorderLeft,
  Exp_TitleBorderRight,
  Exp_TitleEdgeLeft,
  Exp_TitleEdgeRight,
  Exp_TitleEdgeTop,
  Exp_TitleEdgeBottom,
  Exp_ButtonSpacing,
  Exp_ExplicitButtonSpacer,
  Exp_ButtonMarginTop,
  Exp_TitleBorders,
  Exp_ButtonWidth,
  Exp_ButtonHeight,
  Tile_WindowMaskTopLeft,
  Tile_WindowMaskTopRight,
  Tile_WindowMaskBottomLeft,
  Tile_WindowMaskBottomRight,
  Special_WindowTitle,
  Special_WindowIcon,
  Identifiers__Count
};
enum Items {
  Settings = 0,
  Decoration = 1,
  Button = 2,
  Items__Count
};
enum DecorationStates0 {
  Decoration0_active = 0,
  Decoration0_inactive = 4
};
enum DecorationStates1 {
  Decoration1_normal = 0,
  Decoration1_max = 8,
  Decoration1_maxHor = 16,
  Decoration1_maxVert = 24
};
enum DecorationStates2 {
  Decoration2_normal = 0,
  Decoration2_tool = 32
};
enum DecorationStates3 {
  Decoration3_normal = 0,
  Decoration3_shaded = 64
};
enum ButtonStates0 {
  Button0_help = 0,
  Button0_close = 4,
  Button0_minimize = 8,
  Button0_menu = 12,
  Button0_maximize = 16,
  Button0_maxRestore = 20,
  Button0_maxHorRestore = 24,
  Button0_maxVertRestore = 28,
  Button0_shade = 32,
  Button0_shadeRestore = 36,
  Button0_above = 40,
  Button0_aboveRestore = 44,
  Button0_below = 48,
  Button0_belowRestore = 52,
  Button0_allDesktops = 56,
  Button0_allDesktopsRestore = 60
};
enum ButtonStates1 {
  Button1_normal = 0,
  Button1_pressed = 64,
  Button1_prelight = 128
};
enum ButtonStates2 {
  Button2_active = 0,
  Button2_inactive = 256
};
enum ButtonStates3 {
  Button3_normal = 0,
  Button3_tool = 512
};
class SpecDocument : public Cokoon::Document {
protected:
int mapToId(Cokoon::Document::DeclarationType type,const QString &str) const {
  switch(type) {
  case Cokoon::Document::ObjectNameDecl:
    if (false) { return -1; }
    else if (str == "Settings") { return Settings; }
    else if (str == "Decoration") { return Decoration; }
    else if (str == "Button") { return Button; }
    break;
  case Cokoon::Document::VariableDecl:
    if (false) { return -1; }
    else if (str == "BorderSizeFactor") { return Var::BorderSizeFactor; }
    else if (str == "ButtonGroupLeftWidth") { return Var::ButtonGroupLeftWidth; }
    else if (str == "ButtonGroupRightWidth") { return Var::ButtonGroupRightWidth; }
    else if (str == "WindowTitleHeight") { return Var::WindowTitleHeight; }
    else if (str == "WindowTitleWidth") { return Var::WindowTitleWidth; }
    else if (str == "ColorTitleBar") { return Var::ColorTitleBar; }
    else if (str == "ColorTitleBlend") { return Var::ColorTitleBlend; }
    else if (str == "ColorFont") { return Var::ColorFont; }
    else if (str == "ColorButtonBg") { return Var::ColorButtonBg; }
    else if (str == "ColorFrame") { return Var::ColorFrame; }
    else if (str == "ColorHandle") { return Var::ColorHandle; }
    break;
  case Cokoon::Document::IdentifierDecl:
    if (false) { return -1; }
    else if (str == "RespectBorderSizeFactor") { return Exp_RespectBorderSizeFactor; }
    else if (str == "CustomButtonSize") { return Exp_CustomButtonSize; }
    else if (str == "MenuDoubleClickClose") { return Exp_MenuDoubleClickClose; }
    else if (str == "WindowTitleAlignment") { return Exp_WindowTitleAlignment; }
    else if (str == "WindowTitleColor") { return Exp_WindowTitleColor; }
    else if (str == "BorderLeft") { return Exp_BorderLeft; }
    else if (str == "BorderRight") { return Exp_BorderRight; }
    else if (str == "BorderBottom") { return Exp_BorderBottom; }
    else if (str == "TitleHeight") { return Exp_TitleHeight; }
    else if (str == "TitleBorderLeft") { return Exp_TitleBorderLeft; }
    else if (str == "TitleBorderRight") { return Exp_TitleBorderRight; }
    else if (str == "TitleEdgeLeft") { return Exp_TitleEdgeLeft; }
    else if (str == "TitleEdgeRight") { return Exp_TitleEdgeRight; }
    else if (str == "TitleEdgeTop") { return Exp_TitleEdgeTop; }
    else if (str == "TitleEdgeBottom") { return Exp_TitleEdgeBottom; }
    else if (str == "ButtonSpacing") { return Exp_ButtonSpacing; }
    else if (str == "ExplicitButtonSpacer") { return Exp_ExplicitButtonSpacer; }
    else if (str == "ButtonMarginTop") { return Exp_ButtonMarginTop; }
    else if (str == "TitleBorders") { return Exp_TitleBorders; }
    else if (str == "ButtonWidth") { return Exp_ButtonWidth; }
    else if (str == "ButtonHeight") { return Exp_ButtonHeight; }
    else if (str == "WindowMaskTopLeft") { return Tile_WindowMaskTopLeft; }
    else if (str == "WindowMaskTopRight") { return Tile_WindowMaskTopRight; }
    else if (str == "WindowMaskBottomLeft") { return Tile_WindowMaskBottomLeft; }
    else if (str == "WindowMaskBottomRight") { return Tile_WindowMaskBottomRight; }
    else if (str == "WindowTitle") { return Special_WindowTitle; }
    else if (str == "WindowIcon") { return Special_WindowIcon; }
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
        case Settings:
            return 0;
        case Decoration:
            return 4;
        case Button:
            return 4;
        default: return -1;
        }
    }
    int objectStateLevelStates(int itemId, int stateLevel) const {
        switch(itemId) {
        case Settings:
            switch(stateLevel) {
            default: return -1;
            }
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
            case 0: return 16;
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
        case Settings:
            switch(stateLevel) {
            }
            break;
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
              if (stateName == "help") { return Button0_help; }
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
        case Settings:
            switch(stateLevel) {
            }
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
                case 0: return Button0_help;
                case 1: return Button0_close;
                case 2: return Button0_minimize;
                case 3: return Button0_menu;
                case 4: return Button0_maximize;
                case 5: return Button0_maxRestore;
                case 6: return Button0_maxHorRestore;
                case 7: return Button0_maxVertRestore;
                case 8: return Button0_shade;
                case 9: return Button0_shadeRestore;
                case 10: return Button0_above;
                case 11: return Button0_aboveRestore;
                case 12: return Button0_below;
                case 13: return Button0_belowRestore;
                case 14: return Button0_allDesktops;
                case 15: return Button0_allDesktopsRestore;
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
#endif // CokoonDecorationSpec_H
