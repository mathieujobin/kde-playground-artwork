#ifndef CokoonStyleSpec_H
#define CokoonStyleSpec_H
namespace CokoonStyleSpec {
namespace Var {
enum Variables {
  Variables__count
};
}
enum Identifiers {
  Exp_Window_ButtonToTextSpace,
  Exp_DockWidget_SeparatorExtent,
  Exp_MenuItem_IconWidth,
  Exp_ProgressBar_SideText,
  Exp_TabBarTab_TextToIconSpace,
  Exp_CheckBox_Size,
  Exp_Window_TitleMargin,
  Exp_DockWidget_FrameWidth,
  Exp_MenuItemSeparator_Height,
  Exp_MenuItem_MinHeight,
  Exp_ToolBar_ItemSpacing,
  Exp_Header_TextToIconSpace,
  Exp_ProgressBar_BusyIndicatorSize,
  Exp_TabBarTab_FocusMargin,
  Exp_SpinBox_ButtonWidth,
  Exp_MenuItem_ArrowWidth,
  Exp_MenuItem_ArrowSpace,
  Exp_MenuFrame_Width,
  Exp_TabBar_BaseHeight,
  Exp_MenuItem_AccelSpace,
  Exp_MenuItem_IconSpace,
  Exp_MenuItem_CheckSpace,
  Exp_ToolBoxTab_Margin,
  Exp_Window_ButtonWidth,
  Exp_Window_TitleHeight,
  Exp_MenuItem_Margin,
  Exp_ToolBar_ItemMargin,
  Exp_ToolBar_ExtensionExtent,
  Exp_ProgressBar_GrooveMargin,
  Exp_ScrollBar_BarWidth,
  Exp_ComboBox_FrameWidth,
  Exp_MenuBarItem_Margin,
  Exp_ToolBarHandle_Extent,
  Exp_SliderHandle_Length,
  Exp_Splitter_Width,
  Exp_ToolButton_ContentsMargin,
  Exp_ScrollBar_ButtonHeight,
  Exp_ComboBox_ButtonWidth,
  Exp_Button_PressedShiftVertical,
  Exp_Button_FocusMargin,
  Exp_Generic_DefaultFrameWidth,
  Exp_Header_ContentsMargin,
  Exp_SliderHandle_Thickness,
  Exp_SpinBox_FrameWidth,
  Exp_ComboBox_FocusMargin,
  Exp_RadioButton_Size,
  Exp_Window_ButtonSpace,
  Exp_Window_NoTitleFrame,
  Exp_ToolBarSeparator_Extent,
  Exp_TabBarTab_ContentsMargin,
  Exp_Button_DefaultIndicatorMargin,
  Exp_ToolButton_FocusMargin,
  Exp_TabBar_ScrollButtonWidth,
  Exp_TabBar_BaseOverlap,
  Exp_Button_PressedShiftHorizontal,
  Exp_Button_ContentsMargin,
  Identifiers__Count
};
enum Items {
  LayoutProperties = 1,
  GenericFrame = 2,
  Button = 3,
  CheckBox = 4,
  RadioButton = 5,
  LineEdit = 6,
  ComboBox = 7,
  SpinBox = 8,
  ScrollBarButton = 9,
  ScrollBarGroove = 10,
  ScrollBarHandle = 11,
  SliderGroove = 12,
  SliderHandle = 13,
  ProgressBarGroove = 14,
  ProgressBarBar = 15,
  TabBarTab = 16,
  Header = 17,
  ToolBoxTab = 18,
  ToolBarHandle = 19,
  ToolBarSeparator = 20,
  MenuBarItem = 21,
  MenuFrame = 22,
  MenuItem = 23,
  MenuItemSeparator = 24,
  ToolButton = 25,
  Splitter = 26,
  DockWidgetFrame = 27,
  DockWidgetTitle = 28,
  WindowFrame = 29,
  WindowTitle = 30,
  WindowButton = 31
};
enum GenericFrameStates0 {
  GenericFrame0_raised = 100,
  GenericFrame0_sunken = 200,
  GenericFrame0_plain = 300
};
enum GenericFrameStates1 {
  GenericFrame1_normal = 1000,
  GenericFrame1_focused = 2000
};
enum ButtonStates0 {
  Button0_normal = 100,
  Button0_default = 200
};
enum ButtonStates1 {
  Button1_normal = 1000,
  Button1_pressed = 2000,
  Button1_prelight = 3000,
  Button1_disabled = 4000
};
enum CheckBoxStates0 {
  CheckBox0_normal = 100,
  CheckBox0_pressed = 200,
  CheckBox0_prelight = 300,
  CheckBox0_disabled = 400
};
enum CheckBoxStates1 {
  CheckBox1_on = 1000,
  CheckBox1_off = 2000,
  CheckBox1_tristate = 3000
};
enum RadioButtonStates0 {
  RadioButton0_normal = 100,
  RadioButton0_pressed = 200,
  RadioButton0_prelight = 300,
  RadioButton0_disabled = 400
};
enum RadioButtonStates1 {
  RadioButton1_on = 1000,
  RadioButton1_off = 2000
};
enum LineEditStates0 {
  LineEdit0_normal = 100,
  LineEdit0_focused = 200,
  LineEdit0_disabled = 300
};
enum ComboBoxStates0 {
  ComboBox0_normal = 100,
  ComboBox0_pressed = 200,
  ComboBox0_prelight = 300,
  ComboBox0_disabled = 400
};
enum ComboBoxStates1 {
  ComboBox1_normal = 1000,
  ComboBox1_focused = 2000
};
enum SpinBoxStates0 {
  SpinBox0_normal = 100,
  SpinBox0_pressedUp = 200,
  SpinBox0_pressedDown = 300,
  SpinBox0_prelightUp = 400,
  SpinBox0_prelightDown = 500,
  SpinBox0_disabled = 600
};
enum SpinBoxStates1 {
  SpinBox1_normal = 1000,
  SpinBox1_focused = 2000
};
enum ScrollBarButtonStates0 {
  ScrollBarButton0_normal = 100,
  ScrollBarButton0_pressed = 200,
  ScrollBarButton0_disabled = 300
};
enum ScrollBarButtonStates1 {
  ScrollBarButton1_left = 1000,
  ScrollBarButton1_right = 2000,
  ScrollBarButton1_up = 3000,
  ScrollBarButton1_down = 4000
};
enum ScrollBarGrooveStates0 {
  ScrollBarGroove0_normal = 100,
  ScrollBarGroove0_pressed = 200,
  ScrollBarGroove0_disabled = 300
};
enum ScrollBarGrooveStates1 {
  ScrollBarGroove1_horizontal = 1000,
  ScrollBarGroove1_vertical = 2000
};
enum ScrollBarHandleStates0 {
  ScrollBarHandle0_normal = 100,
  ScrollBarHandle0_pressed = 200,
  ScrollBarHandle0_disabled = 300
};
enum ScrollBarHandleStates1 {
  ScrollBarHandle1_horizontal = 1000,
  ScrollBarHandle1_vertical = 2000
};
enum SliderGrooveStates0 {
  SliderGroove0_normal = 100,
  SliderGroove0_disabled = 200
};
enum SliderGrooveStates1 {
  SliderGroove1_horizontal = 1000,
  SliderGroove1_vertical = 2000
};
enum SliderGrooveStates2 {
  SliderGroove2_notFocused = 10000,
  SliderGroove2_focused = 20000
};
enum SliderHandleStates0 {
  SliderHandle0_normal = 100,
  SliderHandle0_prelight = 200,
  SliderHandle0_pressed = 300,
  SliderHandle0_disabled = 400
};
enum SliderHandleStates1 {
  SliderHandle1_horizontal = 1000,
  SliderHandle1_vertical = 2000
};
enum SliderHandleStates2 {
  SliderHandle2_notFocused = 10000,
  SliderHandle2_focused = 20000
};
enum ProgressBarGrooveStates0 {
  ProgressBarGroove0_normal = 100,
  ProgressBarGroove0_disabled = 200
};
enum ProgressBarGrooveStates1 {
  ProgressBarGroove1_horizontal = 1000,
  ProgressBarGroove1_vertical = 2000
};
enum ProgressBarBarStates0 {
  ProgressBarBar0_normal = 100,
  ProgressBarBar0_disabled = 200
};
enum ProgressBarBarStates1 {
  ProgressBarBar1_horizontal = 1000,
  ProgressBarBar1_vertical = 2000
};
enum TabBarTabStates0 {
  TabBarTab0_normal = 100,
  TabBarTab0_prelight = 200,
  TabBarTab0_disabled = 300
};
enum TabBarTabStates1 {
  TabBarTab1_selected = 1000,
  TabBarTab1_nextIsSelected = 2000,
  TabBarTab1_previousIsSelected = 3000,
  TabBarTab1_notAdjacent = 4000
};
enum TabBarTabStates2 {
  TabBarTab2_beginning = 10000,
  TabBarTab2_middle = 20000,
  TabBarTab2_end = 30000,
  TabBarTab2_onlyOne = 40000
};
enum TabBarTabStates3 {
  TabBarTab3_north = 100000,
  TabBarTab3_east = 200000,
  TabBarTab3_south = 300000,
  TabBarTab3_west = 400000
};
enum TabBarTabStates4 {
  TabBarTab4_notFocused = 1000000,
  TabBarTab4_focused = 2000000
};
enum TabBarTabStates5 {
  TabBarTab5_rounded = 10000000,
  TabBarTab5_triangular = 20000000
};
enum HeaderStates0 {
  Header0_normal = 100,
  Header0_prelight = 200,
  Header0_disabled = 300
};
enum HeaderStates1 {
  Header1_selected = 1000,
  Header1_nextIsSelected = 2000,
  Header1_previousIsSelected = 3000,
  Header1_nextAndPreviousAreSelected = 4000,
  Header1_notAdjacent = 5000
};
enum HeaderStates2 {
  Header2_beginning = 10000,
  Header2_middle = 20000,
  Header2_end = 30000,
  Header2_onlyOne = 40000
};
enum HeaderStates3 {
  Header3_horizontal = 100000,
  Header3_vertical = 200000
};
enum ToolBoxTabStates0 {
  ToolBoxTab0_normal = 100,
  ToolBoxTab0_pressed = 200,
  ToolBoxTab0_disabled = 300
};
enum ToolBoxTabStates1 {
  ToolBoxTab1_selected = 1000,
  ToolBoxTab1_notSelected = 2000
};
enum ToolBarHandleStates0 {
  ToolBarHandle0_normal = 100,
  ToolBarHandle0_pressed = 200,
  ToolBarHandle0_prelight = 300
};
enum ToolBarHandleStates1 {
  ToolBarHandle1_horizontal = 1000,
  ToolBarHandle1_vertical = 2000
};
enum ToolBarSeparatorStates0 {
  ToolBarSeparator0_horizontal = 100,
  ToolBarSeparator0_vertical = 200
};
enum MenuBarItemStates0 {
  MenuBarItem0_normal = 100,
  MenuBarItem0_pressed = 200,
  MenuBarItem0_prelight = 300
};
enum MenuItemStates0 {
  MenuItem0_prelight = 100
};
enum ToolButtonStates0 {
  ToolButton0_normal = 100,
  ToolButton0_pressed = 200,
  ToolButton0_prelight = 300,
  ToolButton0_disabled = 400
};
enum SplitterStates0 {
  Splitter0_normal = 100,
  Splitter0_pressed = 200,
  Splitter0_prelight = 300
};
enum SplitterStates1 {
  Splitter1_horizontal = 1000,
  Splitter1_vertical = 2000
};
enum WindowFrameStates0 {
  WindowFrame0_active = 100,
  WindowFrame0_inactive = 200
};
enum WindowTitleStates0 {
  WindowTitle0_active = 100,
  WindowTitle0_inactive = 200
};
enum WindowButtonStates0 {
  WindowButton0_active = 100,
  WindowButton0_inactive = 200
};
enum WindowButtonStates1 {
  WindowButton1_normal = 1000,
  WindowButton1_pressed = 2000
};
enum WindowButtonStates2 {
  WindowButton2_minimize = 10000,
  WindowButton2_maximize = 20000,
  WindowButton2_restore = 30000,
  WindowButton2_close = 40000,
  WindowButton2_shade = 50000,
  WindowButton2_unshade = 60000,
  WindowButton2_help = 70000
};
void setupItems(Cokoon::Document *doc) {
  QList<QHash<QString,int> > itemStateList;
  QHash<QString,int> itemStateListStates;
  itemStateList.clear();
  doc->declareObject("LayoutProperties", itemStateList);
  itemStateList.clear();
  itemStateListStates.clear();
  itemStateListStates.insert("raised", GenericFrame0_raised);
  itemStateListStates.insert("sunken", GenericFrame0_sunken);
  itemStateListStates.insert("plain", GenericFrame0_plain);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("normal", GenericFrame1_normal);
  itemStateListStates.insert("focused", GenericFrame1_focused);
  itemStateList.append(itemStateListStates);
  doc->declareObject("GenericFrame", itemStateList);
  itemStateList.clear();
  itemStateListStates.clear();
  itemStateListStates.insert("normal", Button0_normal);
  itemStateListStates.insert("default", Button0_default);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("normal", Button1_normal);
  itemStateListStates.insert("pressed", Button1_pressed);
  itemStateListStates.insert("prelight", Button1_prelight);
  itemStateListStates.insert("disabled", Button1_disabled);
  itemStateList.append(itemStateListStates);
  doc->declareObject("Button", itemStateList);
  itemStateList.clear();
  itemStateListStates.clear();
  itemStateListStates.insert("normal", CheckBox0_normal);
  itemStateListStates.insert("pressed", CheckBox0_pressed);
  itemStateListStates.insert("prelight", CheckBox0_prelight);
  itemStateListStates.insert("disabled", CheckBox0_disabled);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("on", CheckBox1_on);
  itemStateListStates.insert("off", CheckBox1_off);
  itemStateListStates.insert("tristate", CheckBox1_tristate);
  itemStateList.append(itemStateListStates);
  doc->declareObject("CheckBox", itemStateList);
  itemStateList.clear();
  itemStateListStates.clear();
  itemStateListStates.insert("normal", RadioButton0_normal);
  itemStateListStates.insert("pressed", RadioButton0_pressed);
  itemStateListStates.insert("prelight", RadioButton0_prelight);
  itemStateListStates.insert("disabled", RadioButton0_disabled);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("on", RadioButton1_on);
  itemStateListStates.insert("off", RadioButton1_off);
  itemStateList.append(itemStateListStates);
  doc->declareObject("RadioButton", itemStateList);
  itemStateList.clear();
  itemStateListStates.clear();
  itemStateListStates.insert("normal", LineEdit0_normal);
  itemStateListStates.insert("focused", LineEdit0_focused);
  itemStateListStates.insert("disabled", LineEdit0_disabled);
  itemStateList.append(itemStateListStates);
  doc->declareObject("LineEdit", itemStateList);
  itemStateList.clear();
  itemStateListStates.clear();
  itemStateListStates.insert("normal", ComboBox0_normal);
  itemStateListStates.insert("pressed", ComboBox0_pressed);
  itemStateListStates.insert("prelight", ComboBox0_prelight);
  itemStateListStates.insert("disabled", ComboBox0_disabled);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("normal", ComboBox1_normal);
  itemStateListStates.insert("focused", ComboBox1_focused);
  itemStateList.append(itemStateListStates);
  doc->declareObject("ComboBox", itemStateList);
  itemStateList.clear();
  itemStateListStates.clear();
  itemStateListStates.insert("normal", SpinBox0_normal);
  itemStateListStates.insert("pressedUp", SpinBox0_pressedUp);
  itemStateListStates.insert("pressedDown", SpinBox0_pressedDown);
  itemStateListStates.insert("prelightUp", SpinBox0_prelightUp);
  itemStateListStates.insert("prelightDown", SpinBox0_prelightDown);
  itemStateListStates.insert("disabled", SpinBox0_disabled);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("normal", SpinBox1_normal);
  itemStateListStates.insert("focused", SpinBox1_focused);
  itemStateList.append(itemStateListStates);
  doc->declareObject("SpinBox", itemStateList);
  itemStateList.clear();
  itemStateListStates.clear();
  itemStateListStates.insert("normal", ScrollBarButton0_normal);
  itemStateListStates.insert("pressed", ScrollBarButton0_pressed);
  itemStateListStates.insert("disabled", ScrollBarButton0_disabled);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("left", ScrollBarButton1_left);
  itemStateListStates.insert("right", ScrollBarButton1_right);
  itemStateListStates.insert("up", ScrollBarButton1_up);
  itemStateListStates.insert("down", ScrollBarButton1_down);
  itemStateList.append(itemStateListStates);
  doc->declareObject("ScrollBarButton", itemStateList);
  itemStateList.clear();
  itemStateListStates.clear();
  itemStateListStates.insert("normal", ScrollBarGroove0_normal);
  itemStateListStates.insert("pressed", ScrollBarGroove0_pressed);
  itemStateListStates.insert("disabled", ScrollBarGroove0_disabled);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("horizontal", ScrollBarGroove1_horizontal);
  itemStateListStates.insert("vertical", ScrollBarGroove1_vertical);
  itemStateList.append(itemStateListStates);
  doc->declareObject("ScrollBarGroove", itemStateList);
  itemStateList.clear();
  itemStateListStates.clear();
  itemStateListStates.insert("normal", ScrollBarHandle0_normal);
  itemStateListStates.insert("pressed", ScrollBarHandle0_pressed);
  itemStateListStates.insert("disabled", ScrollBarHandle0_disabled);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("horizontal", ScrollBarHandle1_horizontal);
  itemStateListStates.insert("vertical", ScrollBarHandle1_vertical);
  itemStateList.append(itemStateListStates);
  doc->declareObject("ScrollBarHandle", itemStateList);
  itemStateList.clear();
  itemStateListStates.clear();
  itemStateListStates.insert("normal", SliderGroove0_normal);
  itemStateListStates.insert("disabled", SliderGroove0_disabled);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("horizontal", SliderGroove1_horizontal);
  itemStateListStates.insert("vertical", SliderGroove1_vertical);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("notFocused", SliderGroove2_notFocused);
  itemStateListStates.insert("focused", SliderGroove2_focused);
  itemStateList.append(itemStateListStates);
  doc->declareObject("SliderGroove", itemStateList);
  itemStateList.clear();
  itemStateListStates.clear();
  itemStateListStates.insert("normal", SliderHandle0_normal);
  itemStateListStates.insert("prelight", SliderHandle0_prelight);
  itemStateListStates.insert("pressed", SliderHandle0_pressed);
  itemStateListStates.insert("disabled", SliderHandle0_disabled);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("horizontal", SliderHandle1_horizontal);
  itemStateListStates.insert("vertical", SliderHandle1_vertical);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("notFocused", SliderHandle2_notFocused);
  itemStateListStates.insert("focused", SliderHandle2_focused);
  itemStateList.append(itemStateListStates);
  doc->declareObject("SliderHandle", itemStateList);
  itemStateList.clear();
  itemStateListStates.clear();
  itemStateListStates.insert("normal", ProgressBarGroove0_normal);
  itemStateListStates.insert("disabled", ProgressBarGroove0_disabled);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("horizontal", ProgressBarGroove1_horizontal);
  itemStateListStates.insert("vertical", ProgressBarGroove1_vertical);
  itemStateList.append(itemStateListStates);
  doc->declareObject("ProgressBarGroove", itemStateList);
  itemStateList.clear();
  itemStateListStates.clear();
  itemStateListStates.insert("normal", ProgressBarBar0_normal);
  itemStateListStates.insert("disabled", ProgressBarBar0_disabled);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("horizontal", ProgressBarBar1_horizontal);
  itemStateListStates.insert("vertical", ProgressBarBar1_vertical);
  itemStateList.append(itemStateListStates);
  doc->declareObject("ProgressBarBar", itemStateList);
  itemStateList.clear();
  itemStateListStates.clear();
  itemStateListStates.insert("normal", TabBarTab0_normal);
  itemStateListStates.insert("prelight", TabBarTab0_prelight);
  itemStateListStates.insert("disabled", TabBarTab0_disabled);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("selected", TabBarTab1_selected);
  itemStateListStates.insert("nextIsSelected", TabBarTab1_nextIsSelected);
  itemStateListStates.insert("previousIsSelected", TabBarTab1_previousIsSelected);
  itemStateListStates.insert("notAdjacent", TabBarTab1_notAdjacent);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("beginning", TabBarTab2_beginning);
  itemStateListStates.insert("middle", TabBarTab2_middle);
  itemStateListStates.insert("end", TabBarTab2_end);
  itemStateListStates.insert("onlyOne", TabBarTab2_onlyOne);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("north", TabBarTab3_north);
  itemStateListStates.insert("east", TabBarTab3_east);
  itemStateListStates.insert("south", TabBarTab3_south);
  itemStateListStates.insert("west", TabBarTab3_west);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("notFocused", TabBarTab4_notFocused);
  itemStateListStates.insert("focused", TabBarTab4_focused);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("rounded", TabBarTab5_rounded);
  itemStateListStates.insert("triangular", TabBarTab5_triangular);
  itemStateList.append(itemStateListStates);
  doc->declareObject("TabBarTab", itemStateList);
  itemStateList.clear();
  itemStateListStates.clear();
  itemStateListStates.insert("normal", Header0_normal);
  itemStateListStates.insert("prelight", Header0_prelight);
  itemStateListStates.insert("disabled", Header0_disabled);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("selected", Header1_selected);
  itemStateListStates.insert("nextIsSelected", Header1_nextIsSelected);
  itemStateListStates.insert("previousIsSelected", Header1_previousIsSelected);
  itemStateListStates.insert("nextAndPreviousAreSelected", Header1_nextAndPreviousAreSelected);
  itemStateListStates.insert("notAdjacent", Header1_notAdjacent);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("beginning", Header2_beginning);
  itemStateListStates.insert("middle", Header2_middle);
  itemStateListStates.insert("end", Header2_end);
  itemStateListStates.insert("onlyOne", Header2_onlyOne);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("horizontal", Header3_horizontal);
  itemStateListStates.insert("vertical", Header3_vertical);
  itemStateList.append(itemStateListStates);
  doc->declareObject("Header", itemStateList);
  itemStateList.clear();
  itemStateListStates.clear();
  itemStateListStates.insert("normal", ToolBoxTab0_normal);
  itemStateListStates.insert("pressed", ToolBoxTab0_pressed);
  itemStateListStates.insert("disabled", ToolBoxTab0_disabled);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("selected", ToolBoxTab1_selected);
  itemStateListStates.insert("notSelected", ToolBoxTab1_notSelected);
  itemStateList.append(itemStateListStates);
  doc->declareObject("ToolBoxTab", itemStateList);
  itemStateList.clear();
  itemStateListStates.clear();
  itemStateListStates.insert("normal", ToolBarHandle0_normal);
  itemStateListStates.insert("pressed", ToolBarHandle0_pressed);
  itemStateListStates.insert("prelight", ToolBarHandle0_prelight);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("horizontal", ToolBarHandle1_horizontal);
  itemStateListStates.insert("vertical", ToolBarHandle1_vertical);
  itemStateList.append(itemStateListStates);
  doc->declareObject("ToolBarHandle", itemStateList);
  itemStateList.clear();
  itemStateListStates.clear();
  itemStateListStates.insert("horizontal", ToolBarSeparator0_horizontal);
  itemStateListStates.insert("vertical", ToolBarSeparator0_vertical);
  itemStateList.append(itemStateListStates);
  doc->declareObject("ToolBarSeparator", itemStateList);
  itemStateList.clear();
  itemStateListStates.clear();
  itemStateListStates.insert("normal", MenuBarItem0_normal);
  itemStateListStates.insert("pressed", MenuBarItem0_pressed);
  itemStateListStates.insert("prelight", MenuBarItem0_prelight);
  itemStateList.append(itemStateListStates);
  doc->declareObject("MenuBarItem", itemStateList);
  itemStateList.clear();
  doc->declareObject("MenuFrame", itemStateList);
  itemStateList.clear();
  itemStateListStates.clear();
  itemStateListStates.insert("prelight", MenuItem0_prelight);
  itemStateList.append(itemStateListStates);
  doc->declareObject("MenuItem", itemStateList);
  itemStateList.clear();
  doc->declareObject("MenuItemSeparator", itemStateList);
  itemStateList.clear();
  itemStateListStates.clear();
  itemStateListStates.insert("normal", ToolButton0_normal);
  itemStateListStates.insert("pressed", ToolButton0_pressed);
  itemStateListStates.insert("prelight", ToolButton0_prelight);
  itemStateListStates.insert("disabled", ToolButton0_disabled);
  itemStateList.append(itemStateListStates);
  doc->declareObject("ToolButton", itemStateList);
  itemStateList.clear();
  itemStateListStates.clear();
  itemStateListStates.insert("normal", Splitter0_normal);
  itemStateListStates.insert("pressed", Splitter0_pressed);
  itemStateListStates.insert("prelight", Splitter0_prelight);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("horizontal", Splitter1_horizontal);
  itemStateListStates.insert("vertical", Splitter1_vertical);
  itemStateList.append(itemStateListStates);
  doc->declareObject("Splitter", itemStateList);
  itemStateList.clear();
  doc->declareObject("DockWidgetFrame", itemStateList);
  itemStateList.clear();
  doc->declareObject("DockWidgetTitle", itemStateList);
  itemStateList.clear();
  itemStateListStates.clear();
  itemStateListStates.insert("active", WindowFrame0_active);
  itemStateListStates.insert("inactive", WindowFrame0_inactive);
  itemStateList.append(itemStateListStates);
  doc->declareObject("WindowFrame", itemStateList);
  itemStateList.clear();
  itemStateListStates.clear();
  itemStateListStates.insert("active", WindowTitle0_active);
  itemStateListStates.insert("inactive", WindowTitle0_inactive);
  itemStateList.append(itemStateListStates);
  doc->declareObject("WindowTitle", itemStateList);
  itemStateList.clear();
  itemStateListStates.clear();
  itemStateListStates.insert("active", WindowButton0_active);
  itemStateListStates.insert("inactive", WindowButton0_inactive);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("normal", WindowButton1_normal);
  itemStateListStates.insert("pressed", WindowButton1_pressed);
  itemStateList.append(itemStateListStates);
  itemStateListStates.clear();
  itemStateListStates.insert("minimize", WindowButton2_minimize);
  itemStateListStates.insert("maximize", WindowButton2_maximize);
  itemStateListStates.insert("restore", WindowButton2_restore);
  itemStateListStates.insert("close", WindowButton2_close);
  itemStateListStates.insert("shade", WindowButton2_shade);
  itemStateListStates.insert("unshade", WindowButton2_unshade);
  itemStateListStates.insert("help", WindowButton2_help);
  itemStateList.append(itemStateListStates);
  doc->declareObject("WindowButton", itemStateList);
  doc->declareIdentifier("Window_ButtonToTextSpace", Exp_Window_ButtonToTextSpace);
  doc->declareIdentifier("DockWidget_SeparatorExtent", Exp_DockWidget_SeparatorExtent);
  doc->declareIdentifier("MenuItem_IconWidth", Exp_MenuItem_IconWidth);
  doc->declareIdentifier("ProgressBar_SideText", Exp_ProgressBar_SideText);
  doc->declareIdentifier("TabBarTab_TextToIconSpace", Exp_TabBarTab_TextToIconSpace);
  doc->declareIdentifier("CheckBox_Size", Exp_CheckBox_Size);
  doc->declareIdentifier("Window_TitleMargin", Exp_Window_TitleMargin);
  doc->declareIdentifier("DockWidget_FrameWidth", Exp_DockWidget_FrameWidth);
  doc->declareIdentifier("MenuItemSeparator_Height", Exp_MenuItemSeparator_Height);
  doc->declareIdentifier("MenuItem_MinHeight", Exp_MenuItem_MinHeight);
  doc->declareIdentifier("ToolBar_ItemSpacing", Exp_ToolBar_ItemSpacing);
  doc->declareIdentifier("Header_TextToIconSpace", Exp_Header_TextToIconSpace);
  doc->declareIdentifier("ProgressBar_BusyIndicatorSize", Exp_ProgressBar_BusyIndicatorSize);
  doc->declareIdentifier("TabBarTab_FocusMargin", Exp_TabBarTab_FocusMargin);
  doc->declareIdentifier("SpinBox_ButtonWidth", Exp_SpinBox_ButtonWidth);
  doc->declareIdentifier("MenuItem_ArrowWidth", Exp_MenuItem_ArrowWidth);
  doc->declareIdentifier("MenuItem_ArrowSpace", Exp_MenuItem_ArrowSpace);
  doc->declareIdentifier("MenuFrame_Width", Exp_MenuFrame_Width);
  doc->declareIdentifier("TabBar_BaseHeight", Exp_TabBar_BaseHeight);
  doc->declareIdentifier("MenuItem_AccelSpace", Exp_MenuItem_AccelSpace);
  doc->declareIdentifier("MenuItem_IconSpace", Exp_MenuItem_IconSpace);
  doc->declareIdentifier("MenuItem_CheckSpace", Exp_MenuItem_CheckSpace);
  doc->declareIdentifier("ToolBoxTab_Margin", Exp_ToolBoxTab_Margin);
  doc->declareIdentifier("Window_ButtonWidth", Exp_Window_ButtonWidth);
  doc->declareIdentifier("Window_TitleHeight", Exp_Window_TitleHeight);
  doc->declareIdentifier("MenuItem_Margin", Exp_MenuItem_Margin);
  doc->declareIdentifier("ToolBar_ItemMargin", Exp_ToolBar_ItemMargin);
  doc->declareIdentifier("ToolBar_ExtensionExtent", Exp_ToolBar_ExtensionExtent);
  doc->declareIdentifier("ProgressBar_GrooveMargin", Exp_ProgressBar_GrooveMargin);
  doc->declareIdentifier("ScrollBar_BarWidth", Exp_ScrollBar_BarWidth);
  doc->declareIdentifier("ComboBox_FrameWidth", Exp_ComboBox_FrameWidth);
  doc->declareIdentifier("MenuBarItem_Margin", Exp_MenuBarItem_Margin);
  doc->declareIdentifier("ToolBarHandle_Extent", Exp_ToolBarHandle_Extent);
  doc->declareIdentifier("SliderHandle_Length", Exp_SliderHandle_Length);
  doc->declareIdentifier("Splitter_Width", Exp_Splitter_Width);
  doc->declareIdentifier("ToolButton_ContentsMargin", Exp_ToolButton_ContentsMargin);
  doc->declareIdentifier("ScrollBar_ButtonHeight", Exp_ScrollBar_ButtonHeight);
  doc->declareIdentifier("ComboBox_ButtonWidth", Exp_ComboBox_ButtonWidth);
  doc->declareIdentifier("Button_PressedShiftVertical", Exp_Button_PressedShiftVertical);
  doc->declareIdentifier("Button_FocusMargin", Exp_Button_FocusMargin);
  doc->declareIdentifier("Generic_DefaultFrameWidth", Exp_Generic_DefaultFrameWidth);
  doc->declareIdentifier("Header_ContentsMargin", Exp_Header_ContentsMargin);
  doc->declareIdentifier("SliderHandle_Thickness", Exp_SliderHandle_Thickness);
  doc->declareIdentifier("SpinBox_FrameWidth", Exp_SpinBox_FrameWidth);
  doc->declareIdentifier("ComboBox_FocusMargin", Exp_ComboBox_FocusMargin);
  doc->declareIdentifier("RadioButton_Size", Exp_RadioButton_Size);
  doc->declareIdentifier("Window_ButtonSpace", Exp_Window_ButtonSpace);
  doc->declareIdentifier("Window_NoTitleFrame", Exp_Window_NoTitleFrame);
  doc->declareIdentifier("ToolBarSeparator_Extent", Exp_ToolBarSeparator_Extent);
  doc->declareIdentifier("TabBarTab_ContentsMargin", Exp_TabBarTab_ContentsMargin);
  doc->declareIdentifier("Button_DefaultIndicatorMargin", Exp_Button_DefaultIndicatorMargin);
  doc->declareIdentifier("ToolButton_FocusMargin", Exp_ToolButton_FocusMargin);
  doc->declareIdentifier("TabBar_ScrollButtonWidth", Exp_TabBar_ScrollButtonWidth);
  doc->declareIdentifier("TabBar_BaseOverlap", Exp_TabBar_BaseOverlap);
  doc->declareIdentifier("Button_PressedShiftHorizontal", Exp_Button_PressedShiftHorizontal);
  doc->declareIdentifier("Button_ContentsMargin", Exp_Button_ContentsMargin);
}
}
#endif // CokoonStyleSpec_H
