#ifndef DecorationThemeSpec_H
#define DecorationThemeSpec_H
namespace DecorationThemeSpec {
namespace Var {
enum Variables {
  testVar,
  Variables__count
};
}
enum Identifiers {
  Exp_objectExp,
  Exp_recursiveLoopTest1,
  Exp_objectExp2,
  Exp_recursiveLoopTest2,
  Exp_objectExp3,
  Exp_recursiveLoopTest3,
  Exp_oneplusone,
  Exp_variableExpression,
  Identifiers__Count
};
enum Items {
  Decoration = 1,
  Button = 2
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
  Button0_close = 10,
  Button0_minimize = 20,
  Button0_menu = 30,
  Button0_maximize = 40,
  Button0_maxRestore = 50,
  Button0_maxHorRestore = 60,
  Button0_maxVertRestore = 70,
  Button0_shade = 80,
  Button0_shadeRestore = 90,
  Button0_above = 100,
  Button0_aboveRestore = 110,
  Button0_below = 120,
  Button0_belowRestore = 130,
  Button0_allDesktops = 140,
  Button0_allDesktopsRestore = 150
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
  doc->declareVariable("testVar", Var::testVar);
  doc->declareIdentifier("objectExp", Exp_objectExp);
  doc->declareIdentifier("recursiveLoopTest1", Exp_recursiveLoopTest1);
  doc->declareIdentifier("objectExp2", Exp_objectExp2);
  doc->declareIdentifier("recursiveLoopTest2", Exp_recursiveLoopTest2);
  doc->declareIdentifier("objectExp3", Exp_objectExp3);
  doc->declareIdentifier("recursiveLoopTest3", Exp_recursiveLoopTest3);
  doc->declareIdentifier("oneplusone", Exp_oneplusone);
  doc->declareIdentifier("variableExpression", Exp_variableExpression);
}
}
#endif // DecorationThemeSpec_H
