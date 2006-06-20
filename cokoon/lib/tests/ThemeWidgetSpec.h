#ifndef ThemeWidgetSpec_H
#define ThemeWidgetSpec_H
namespace ThemeWidgetSpec {
enum Items {
  Widget = 1
};
void setupItems(Cokoon::Document *doc) {
  QList<QHash<QString,int> > itemStateList;
  QHash<QString,int> itemStateListStates;
  itemStateList.clear();
  doc->declareObject("Widget", itemStateList);
}
}
#endif // ThemeWidgetSpec_H
