#ifndef CokoonStyleSpec_H
#define CokoonStyleSpec_H
namespace CokoonStyleSpec {
namespace Var {
enum Variables {
  Variables__count
};
}
enum Identifiers {
  Exp_ContentsMargin,
  Exp_PressedShiftVertical,
  Exp_PressedShiftHorizontal,
  Exp_FocusMargin,
  Exp_DefaultIndicatorMargin,
  Identifiers__Count
};
enum Items {
  Settings = 1,
  ButtonSettings = 2,
  Button = 3
};
enum ButtonStates0 {
  Button0_normal = 10,
  Button0_default = 20
};
enum ButtonStates1 {
  Button1_normal = 100,
  Button1_pressed = 200,
  Button1_prelight = 300,
  Button1_disabled = 400
};
void setupItems(Cokoon::Document *doc) {
  QList<QHash<QString,int> > itemStateList;
  QHash<QString,int> itemStateListStates;
  itemStateList.clear();
  doc->declareObject("Settings", itemStateList);
  itemStateList.clear();
  doc->declareObject("ButtonSettings", itemStateList);
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
  doc->declareIdentifier("ContentsMargin", Exp_ContentsMargin);
  doc->declareIdentifier("PressedShiftVertical", Exp_PressedShiftVertical);
  doc->declareIdentifier("PressedShiftHorizontal", Exp_PressedShiftHorizontal);
  doc->declareIdentifier("FocusMargin", Exp_FocusMargin);
  doc->declareIdentifier("DefaultIndicatorMargin", Exp_DefaultIndicatorMargin);
}
}
#endif // CokoonStyleSpec_H
