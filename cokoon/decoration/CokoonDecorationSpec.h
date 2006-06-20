#ifndef CokoonDecorationSpec_H
#define CokoonDecorationSpec_H
namespace CokoonDecorationSpec {
namespace Var {
enum Variables {
  ColorHandle,
  ButtonGroupLeftWidth,
  ColorTitleBar,
  WindowTitleWidth,
  ColorFrame,
  ButtonGroupRightWidth,
  ColorFont,
  ColorButtonBg,
  WindowTitleHeight,
  ColorTitleBlend,
  BorderSizeFactor,
  Variables__count
};
}
enum Identifiers {
  Exp_ExplicitButtonSpacer,
  Exp_TitleEdgeTop,
  Exp_BorderBottom,
  Exp_BorderLeft,
  Exp_ButtonWidth,
  Exp_TitleBorderRight,
  Exp_TitleHeight,
  Exp_ButtonHeight,
  Exp_TitleEdgeBottom,
  Exp_TitleBorderLeft,
  Exp_MenuDoubleClickClose,
  Exp_BorderRight,
  Exp_ButtonSpacing,
  Exp_TitleEdgeLeft,
  Exp_RespectBorderSizeFactor,
  Exp_TitleEdgeRight,
  Exp_TitleBorders,
  Exp_CustomButtonSize,
  Exp_WindowTitleColor,
  Exp_WindowTitleAlignment,
  Exp_ButtonMarginTop,
  Tile_WindowMaskBottomRight,
  Tile_WindowMaskTopLeft,
  Tile_WindowMaskBottomLeft,
  Tile_WindowMaskTopRight,
  Special_WindowTitle,
  Special_WindowIcon,
  Identifiers__Count
};
enum Items {
  Settings = 1,
  Decoration = 2,
  Button = 3
};
enum DecorationStates0 {
  Decoration0_active = 10,
  Decoration0_inactive = 20
};
enum DecorationStates1 {
  Decoration1_normal = 100,
  Decoration1_max = 200,
  Decoration1_maxHor = 300,
  Decoration1_maxVert = 400
};
enum DecorationStates2 {
  Decoration2_normal = 1000,
  Decoration2_tool = 2000
};
enum DecorationStates3 {
  Decoration3_normal = 10000,
  Decoration3_shaded = 20000
};
enum ButtonStates0 {
  Button0_help = 10,
  Button0_close = 20,
  Button0_minimize = 30,
  Button0_menu = 40,
  Button0_maximize = 50,
  Button0_maxRestore = 60,
  Button0_maxHorRestore = 70,
  Button0_maxVertRestore = 80,
  Button0_shade = 90,
  Button0_shadeRestore = 100,
  Button0_above = 110,
  Button0_aboveRestore = 120,
  Button0_below = 130,
  Button0_belowRestore = 140,
  Button0_allDesktops = 150,
  Button0_allDesktopsRestore = 160
};
enum ButtonStates1 {
  Button1_normal = 1000,
  Button1_pressed = 2000,
  Button1_prelight = 3000
};
enum ButtonStates2 {
  Button2_active = 10000,
  Button2_inactive = 20000
};
enum ButtonStates3 {
  Button3_normal = 100000,
  Button3_tool = 200000
};
void setupItems(Cokoon::Document *doc) {
  QList<QHash<QString,int> > itemStateList;
  QHash<QString,int> itemStateListStates;
  itemStateList.clear();
  doc->declareObject("Settings", itemStateList);
  itemStateList.clear();
  itemStateListStates.clear();
  itemStateListStates.insert("active", Decoration0_active);
  itemStateListStates.insert("inactive", Decoration0_inactive);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("normal", Decoration1_normal);
  itemStateListStates.insert("max", Decoration1_max);
  itemStateListStates.insert("maxHor", Decoration1_maxHor);
  itemStateListStates.insert("maxVert", Decoration1_maxVert);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("normal", Decoration2_normal);
  itemStateListStates.insert("tool", Decoration2_tool);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("normal", Decoration3_normal);
  itemStateListStates.insert("shaded", Decoration3_shaded);
  itemStateList.append(itemStateListStates);
  doc->declareObject("Decoration", itemStateList);
  itemStateList.clear();
  itemStateListStates.clear();
  itemStateListStates.insert("help", Button0_help);
  itemStateListStates.insert("close", Button0_close);
  itemStateListStates.insert("minimize", Button0_minimize);
  itemStateListStates.insert("menu", Button0_menu);
  itemStateListStates.insert("maximize", Button0_maximize);
  itemStateListStates.insert("maxRestore", Button0_maxRestore);
  itemStateListStates.insert("maxHorRestore", Button0_maxHorRestore);
  itemStateListStates.insert("maxVertRestore", Button0_maxVertRestore);
  itemStateListStates.insert("shade", Button0_shade);
  itemStateListStates.insert("shadeRestore", Button0_shadeRestore);
  itemStateListStates.insert("above", Button0_above);
  itemStateListStates.insert("aboveRestore", Button0_aboveRestore);
  itemStateListStates.insert("below", Button0_below);
  itemStateListStates.insert("belowRestore", Button0_belowRestore);
  itemStateListStates.insert("allDesktops", Button0_allDesktops);
  itemStateListStates.insert("allDesktopsRestore", Button0_allDesktopsRestore);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("normal", Button1_normal);
  itemStateListStates.insert("pressed", Button1_pressed);
  itemStateListStates.insert("prelight", Button1_prelight);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("active", Button2_active);
  itemStateListStates.insert("inactive", Button2_inactive);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("normal", Button3_normal);
  itemStateListStates.insert("tool", Button3_tool);
  itemStateList.append(itemStateListStates);
  doc->declareObject("Button", itemStateList);
  doc->declareVariable("ColorHandle", Var::ColorHandle);
  doc->declareVariable("ButtonGroupLeftWidth", Var::ButtonGroupLeftWidth);
  doc->declareVariable("ColorTitleBar", Var::ColorTitleBar);
  doc->declareVariable("WindowTitleWidth", Var::WindowTitleWidth);
  doc->declareVariable("ColorFrame", Var::ColorFrame);
  doc->declareVariable("ButtonGroupRightWidth", Var::ButtonGroupRightWidth);
  doc->declareVariable("ColorFont", Var::ColorFont);
  doc->declareVariable("ColorButtonBg", Var::ColorButtonBg);
  doc->declareVariable("WindowTitleHeight", Var::WindowTitleHeight);
  doc->declareVariable("ColorTitleBlend", Var::ColorTitleBlend);
  doc->declareVariable("BorderSizeFactor", Var::BorderSizeFactor);
  doc->declareIdentifier("ExplicitButtonSpacer", Exp_ExplicitButtonSpacer);
  doc->declareIdentifier("TitleEdgeTop", Exp_TitleEdgeTop);
  doc->declareIdentifier("BorderBottom", Exp_BorderBottom);
  doc->declareIdentifier("BorderLeft", Exp_BorderLeft);
  doc->declareIdentifier("ButtonWidth", Exp_ButtonWidth);
  doc->declareIdentifier("TitleBorderRight", Exp_TitleBorderRight);
  doc->declareIdentifier("TitleHeight", Exp_TitleHeight);
  doc->declareIdentifier("ButtonHeight", Exp_ButtonHeight);
  doc->declareIdentifier("TitleEdgeBottom", Exp_TitleEdgeBottom);
  doc->declareIdentifier("TitleBorderLeft", Exp_TitleBorderLeft);
  doc->declareIdentifier("MenuDoubleClickClose", Exp_MenuDoubleClickClose);
  doc->declareIdentifier("BorderRight", Exp_BorderRight);
  doc->declareIdentifier("ButtonSpacing", Exp_ButtonSpacing);
  doc->declareIdentifier("TitleEdgeLeft", Exp_TitleEdgeLeft);
  doc->declareIdentifier("RespectBorderSizeFactor", Exp_RespectBorderSizeFactor);
  doc->declareIdentifier("TitleEdgeRight", Exp_TitleEdgeRight);
  doc->declareIdentifier("TitleBorders", Exp_TitleBorders);
  doc->declareIdentifier("CustomButtonSize", Exp_CustomButtonSize);
  doc->declareIdentifier("WindowTitleColor", Exp_WindowTitleColor);
  doc->declareIdentifier("WindowTitleAlignment", Exp_WindowTitleAlignment);
  doc->declareIdentifier("ButtonMarginTop", Exp_ButtonMarginTop);
  doc->declareIdentifier("WindowMaskBottomRight", Tile_WindowMaskBottomRight);
  doc->declareIdentifier("WindowMaskTopLeft", Tile_WindowMaskTopLeft);
  doc->declareIdentifier("WindowMaskBottomLeft", Tile_WindowMaskBottomLeft);
  doc->declareIdentifier("WindowMaskTopRight", Tile_WindowMaskTopRight);
  doc->declareIdentifier("WindowTitle", Special_WindowTitle);
  doc->declareIdentifier("WindowIcon", Special_WindowIcon);
}
}
#endif // CokoonDecorationSpec_H
