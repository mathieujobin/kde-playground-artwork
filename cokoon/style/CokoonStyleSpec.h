#ifndef CokoonStyleSpec_H
#define CokoonStyleSpec_H
namespace CokoonStyleSpec {
namespace Var {
enum Variables {
  Variables__count
};
}
enum Identifiers {
  Exp_CheckBox_Size,
  Exp_SpinBox_ButtonWidth,
  Exp_ScrollBar_BarWidth,
  Exp_ComboBox_FrameWidth,
  Exp_ScrollBar_ButtonHeight,
  Exp_ComboBox_ButtonWidth,
  Exp_Button_PressedShiftVertical,
  Exp_Button_FocusMargin,
  Exp_Generic_DefaultFrameWidth,
  Exp_SpinBox_FrameWidth,
  Exp_ComboBox_FocusMargin,
  Exp_RadioButton_Size,
  Exp_Button_DefaultIndicatorMargin,
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
  ScrollBarHandle = 11
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
  doc->declareIdentifier("CheckBox_Size", Exp_CheckBox_Size);
  doc->declareIdentifier("SpinBox_ButtonWidth", Exp_SpinBox_ButtonWidth);
  doc->declareIdentifier("ScrollBar_BarWidth", Exp_ScrollBar_BarWidth);
  doc->declareIdentifier("ComboBox_FrameWidth", Exp_ComboBox_FrameWidth);
  doc->declareIdentifier("ScrollBar_ButtonHeight", Exp_ScrollBar_ButtonHeight);
  doc->declareIdentifier("ComboBox_ButtonWidth", Exp_ComboBox_ButtonWidth);
  doc->declareIdentifier("Button_PressedShiftVertical", Exp_Button_PressedShiftVertical);
  doc->declareIdentifier("Button_FocusMargin", Exp_Button_FocusMargin);
  doc->declareIdentifier("Generic_DefaultFrameWidth", Exp_Generic_DefaultFrameWidth);
  doc->declareIdentifier("SpinBox_FrameWidth", Exp_SpinBox_FrameWidth);
  doc->declareIdentifier("ComboBox_FocusMargin", Exp_ComboBox_FocusMargin);
  doc->declareIdentifier("RadioButton_Size", Exp_RadioButton_Size);
  doc->declareIdentifier("Button_DefaultIndicatorMargin", Exp_Button_DefaultIndicatorMargin);
  doc->declareIdentifier("Button_PressedShiftHorizontal", Exp_Button_PressedShiftHorizontal);
  doc->declareIdentifier("Button_ContentsMargin", Exp_Button_ContentsMargin);
}
}
#endif // CokoonStyleSpec_H
