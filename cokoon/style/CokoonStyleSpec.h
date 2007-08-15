#ifndef CokoonStyleSpec_H
#define CokoonStyleSpec_H
namespace CokoonStyleSpec {
namespace Var {
enum Variables {
  Variables__Count
};
}
enum Identifiers {
  Exp_Generic_DefaultFrameWidth,
  Exp_Button_DefaultIndicatorMargin,
  Exp_Button_ContentsMargin,
  Exp_Button_FocusMargin,
  Exp_Button_PressedShiftHorizontal,
  Exp_Button_PressedShiftVertical,
  Exp_CheckBox_Size,
  Exp_RadioButton_Size,
  Exp_ComboBox_FrameWidth,
  Exp_ComboBox_ButtonWidth,
  Exp_ComboBox_FocusMargin,
  Exp_SpinBox_FrameWidth,
  Exp_SpinBox_ButtonWidth,
  Exp_ScrollBar_BarWidth,
  Exp_ScrollBar_ButtonHeight,
  Exp_TabBarTab_ContentsMargin,
  Exp_TabBarTab_FocusMargin,
  Exp_TabBarTab_TextToIconSpace,
  Exp_TabBar_BaseHeight,
  Exp_TabBar_BaseOverlap,
  Exp_SliderHandle_Thickness,
  Exp_SliderHandle_Length,
  Exp_ProgressBar_GrooveMargin,
  Exp_ProgressBar_SideText,
  Exp_ProgressBar_BusyIndicatorSize,
  Exp_TabBar_ScrollButtonWidth,
  Exp_Header_ContentsMargin,
  Exp_Header_TextToIconSpace,
  Exp_ToolBoxTab_Margin,
  Exp_ToolBarHandle_Extent,
  Exp_ToolBar_ExtensionExtent,
  Exp_ToolBarSeparator_Extent,
  Exp_ToolBar_ItemMargin,
  Exp_ToolBar_ItemSpacing,
  Exp_MenuBarItem_Margin,
  Exp_MenuFrame_Width,
  Exp_MenuItem_Margin,
  Exp_MenuItem_CheckSpace,
  Exp_MenuItem_IconWidth,
  Exp_MenuItem_IconSpace,
  Exp_MenuItem_AccelSpace,
  Exp_MenuItem_ArrowSpace,
  Exp_MenuItem_ArrowWidth,
  Exp_MenuItem_MinHeight,
  Exp_MenuItemSeparator_Height,
  Exp_ToolButton_ContentsMargin,
  Exp_ToolButton_FocusMargin,
  Exp_Splitter_Width,
  Exp_DockWidget_FrameWidth,
  Exp_DockWidget_SeparatorExtent,
  Exp_Window_TitleHeight,
  Exp_Window_NoTitleFrame,
  Exp_Window_TitleMargin,
  Exp_Window_ButtonWidth,
  Exp_Window_ButtonSpace,
  Exp_Window_ButtonToTextSpace,
  Identifiers__Count
};
enum Items {
  LayoutProperties = 0,
  GenericFrame = 1,
  Button = 2,
  CheckBox = 3,
  RadioButton = 4,
  LineEdit = 5,
  ComboBox = 6,
  SpinBox = 7,
  ScrollBarButton = 8,
  ScrollBarGroove = 9,
  ScrollBarHandle = 10,
  SliderGroove = 11,
  SliderHandle = 12,
  ProgressBarGroove = 13,
  ProgressBarBar = 14,
  TabBarTab = 15,
  Header = 16,
  ToolBoxTab = 17,
  ToolBarHandle = 18,
  ToolBarSeparator = 19,
  MenuBarItem = 20,
  MenuFrame = 21,
  MenuItem = 22,
  MenuItemSeparator = 23,
  ToolButton = 24,
  Splitter = 25,
  DockWidgetFrame = 26,
  DockWidgetTitle = 27,
  WindowFrame = 28,
  WindowTitle = 29,
  WindowButton = 30,
  Items__Count
};
enum GenericFrameStates0 {
  GenericFrame0_raised = 0,
  GenericFrame0_sunken = 32,
  GenericFrame0_plain = 64
};
enum GenericFrameStates1 {
  GenericFrame1_normal = 0,
  GenericFrame1_focused = 128
};
enum ButtonStates0 {
  Button0_normal = 0,
  Button0_default = 32
};
enum ButtonStates1 {
  Button1_normal = 0,
  Button1_pressed = 64,
  Button1_prelight = 128,
  Button1_disabled = 192
};
enum CheckBoxStates0 {
  CheckBox0_normal = 0,
  CheckBox0_pressed = 32,
  CheckBox0_prelight = 64,
  CheckBox0_disabled = 96
};
enum CheckBoxStates1 {
  CheckBox1_on = 0,
  CheckBox1_off = 128,
  CheckBox1_tristate = 256
};
enum RadioButtonStates0 {
  RadioButton0_normal = 0,
  RadioButton0_pressed = 32,
  RadioButton0_prelight = 64,
  RadioButton0_disabled = 96
};
enum RadioButtonStates1 {
  RadioButton1_on = 0,
  RadioButton1_off = 128
};
enum LineEditStates0 {
  LineEdit0_normal = 0,
  LineEdit0_focused = 32,
  LineEdit0_disabled = 64
};
enum ComboBoxStates0 {
  ComboBox0_normal = 0,
  ComboBox0_pressed = 32,
  ComboBox0_prelight = 64,
  ComboBox0_disabled = 96
};
enum ComboBoxStates1 {
  ComboBox1_normal = 0,
  ComboBox1_focused = 128
};
enum SpinBoxStates0 {
  SpinBox0_normal = 0,
  SpinBox0_pressedUp = 32,
  SpinBox0_pressedDown = 64,
  SpinBox0_prelightUp = 96,
  SpinBox0_prelightDown = 128,
  SpinBox0_disabled = 160
};
enum SpinBoxStates1 {
  SpinBox1_normal = 0,
  SpinBox1_focused = 256
};
enum ScrollBarButtonStates0 {
  ScrollBarButton0_normal = 0,
  ScrollBarButton0_pressed = 32,
  ScrollBarButton0_disabled = 64
};
enum ScrollBarButtonStates1 {
  ScrollBarButton1_left = 0,
  ScrollBarButton1_right = 128,
  ScrollBarButton1_up = 256,
  ScrollBarButton1_down = 384
};
enum ScrollBarGrooveStates0 {
  ScrollBarGroove0_normal = 0,
  ScrollBarGroove0_pressed = 32,
  ScrollBarGroove0_disabled = 64
};
enum ScrollBarGrooveStates1 {
  ScrollBarGroove1_horizontal = 0,
  ScrollBarGroove1_vertical = 128
};
enum ScrollBarHandleStates0 {
  ScrollBarHandle0_normal = 0,
  ScrollBarHandle0_pressed = 32,
  ScrollBarHandle0_disabled = 64
};
enum ScrollBarHandleStates1 {
  ScrollBarHandle1_horizontal = 0,
  ScrollBarHandle1_vertical = 128
};
enum SliderGrooveStates0 {
  SliderGroove0_normal = 0,
  SliderGroove0_disabled = 32
};
enum SliderGrooveStates1 {
  SliderGroove1_horizontal = 0,
  SliderGroove1_vertical = 64
};
enum SliderGrooveStates2 {
  SliderGroove2_notFocused = 0,
  SliderGroove2_focused = 128
};
enum SliderHandleStates0 {
  SliderHandle0_normal = 0,
  SliderHandle0_prelight = 32,
  SliderHandle0_pressed = 64,
  SliderHandle0_disabled = 96
};
enum SliderHandleStates1 {
  SliderHandle1_horizontal = 0,
  SliderHandle1_vertical = 128
};
enum SliderHandleStates2 {
  SliderHandle2_notFocused = 0,
  SliderHandle2_focused = 256
};
enum ProgressBarGrooveStates0 {
  ProgressBarGroove0_normal = 0,
  ProgressBarGroove0_disabled = 32
};
enum ProgressBarGrooveStates1 {
  ProgressBarGroove1_horizontal = 0,
  ProgressBarGroove1_vertical = 64
};
enum ProgressBarBarStates0 {
  ProgressBarBar0_normal = 0,
  ProgressBarBar0_disabled = 32
};
enum ProgressBarBarStates1 {
  ProgressBarBar1_horizontal = 0,
  ProgressBarBar1_vertical = 64
};
enum TabBarTabStates0 {
  TabBarTab0_normal = 0,
  TabBarTab0_prelight = 32,
  TabBarTab0_disabled = 64
};
enum TabBarTabStates1 {
  TabBarTab1_selected = 0,
  TabBarTab1_nextIsSelected = 128,
  TabBarTab1_previousIsSelected = 256,
  TabBarTab1_notAdjacent = 384
};
enum TabBarTabStates2 {
  TabBarTab2_beginning = 0,
  TabBarTab2_middle = 512,
  TabBarTab2_end = 1024,
  TabBarTab2_onlyOne = 1536
};
enum TabBarTabStates3 {
  TabBarTab3_north = 0,
  TabBarTab3_east = 2048,
  TabBarTab3_south = 4096,
  TabBarTab3_west = 6144
};
enum TabBarTabStates4 {
  TabBarTab4_notFocused = 0,
  TabBarTab4_focused = 8192
};
enum TabBarTabStates5 {
  TabBarTab5_rounded = 0,
  TabBarTab5_triangular = 16384
};
enum HeaderStates0 {
  Header0_normal = 0,
  Header0_prelight = 32,
  Header0_disabled = 64
};
enum HeaderStates1 {
  Header1_selected = 0,
  Header1_nextIsSelected = 128,
  Header1_previousIsSelected = 256,
  Header1_nextAndPreviousAreSelected = 384,
  Header1_notAdjacent = 512
};
enum HeaderStates2 {
  Header2_beginning = 0,
  Header2_middle = 1024,
  Header2_end = 2048,
  Header2_onlyOne = 3072
};
enum HeaderStates3 {
  Header3_horizontal = 0,
  Header3_vertical = 4096
};
enum ToolBoxTabStates0 {
  ToolBoxTab0_normal = 0,
  ToolBoxTab0_pressed = 32,
  ToolBoxTab0_disabled = 64
};
enum ToolBoxTabStates1 {
  ToolBoxTab1_selected = 0,
  ToolBoxTab1_notSelected = 128
};
enum ToolBarHandleStates0 {
  ToolBarHandle0_normal = 0,
  ToolBarHandle0_pressed = 32,
  ToolBarHandle0_prelight = 64
};
enum ToolBarHandleStates1 {
  ToolBarHandle1_horizontal = 0,
  ToolBarHandle1_vertical = 128
};
enum ToolBarSeparatorStates0 {
  ToolBarSeparator0_horizontal = 0,
  ToolBarSeparator0_vertical = 32
};
enum MenuBarItemStates0 {
  MenuBarItem0_normal = 0,
  MenuBarItem0_pressed = 32,
  MenuBarItem0_prelight = 64
};
enum MenuItemStates0 {
  MenuItem0_prelight = 0
};
enum ToolButtonStates0 {
  ToolButton0_normal = 0,
  ToolButton0_pressed = 32,
  ToolButton0_prelight = 64,
  ToolButton0_disabled = 96
};
enum SplitterStates0 {
  Splitter0_normal = 0,
  Splitter0_pressed = 32,
  Splitter0_prelight = 64
};
enum SplitterStates1 {
  Splitter1_horizontal = 0,
  Splitter1_vertical = 128
};
enum WindowFrameStates0 {
  WindowFrame0_active = 0,
  WindowFrame0_inactive = 32
};
enum WindowTitleStates0 {
  WindowTitle0_active = 0,
  WindowTitle0_inactive = 32
};
enum WindowButtonStates0 {
  WindowButton0_active = 0,
  WindowButton0_inactive = 32
};
enum WindowButtonStates1 {
  WindowButton1_normal = 0,
  WindowButton1_pressed = 64
};
enum WindowButtonStates2 {
  WindowButton2_minimize = 0,
  WindowButton2_maximize = 128,
  WindowButton2_restore = 256,
  WindowButton2_close = 384,
  WindowButton2_shade = 512,
  WindowButton2_unshade = 640,
  WindowButton2_help = 768
};
class SpecDocument : public Cokoon::Document {
protected:
int mapToId(Cokoon::Document::DeclarationType type,const QString &str) const {
  switch(type) {
  case Cokoon::Document::ObjectStateDecl:
    if (false) { return -1; }
    else if (str == "LayoutProperties") { return LayoutProperties; }
    else if (str == "GenericFrame") { return GenericFrame; }
    else if (str == "Button") { return Button; }
    else if (str == "CheckBox") { return CheckBox; }
    else if (str == "RadioButton") { return RadioButton; }
    else if (str == "LineEdit") { return LineEdit; }
    else if (str == "ComboBox") { return ComboBox; }
    else if (str == "SpinBox") { return SpinBox; }
    else if (str == "ScrollBarButton") { return ScrollBarButton; }
    else if (str == "ScrollBarGroove") { return ScrollBarGroove; }
    else if (str == "ScrollBarHandle") { return ScrollBarHandle; }
    else if (str == "SliderGroove") { return SliderGroove; }
    else if (str == "SliderHandle") { return SliderHandle; }
    else if (str == "ProgressBarGroove") { return ProgressBarGroove; }
    else if (str == "ProgressBarBar") { return ProgressBarBar; }
    else if (str == "TabBarTab") { return TabBarTab; }
    else if (str == "Header") { return Header; }
    else if (str == "ToolBoxTab") { return ToolBoxTab; }
    else if (str == "ToolBarHandle") { return ToolBarHandle; }
    else if (str == "ToolBarSeparator") { return ToolBarSeparator; }
    else if (str == "MenuBarItem") { return MenuBarItem; }
    else if (str == "MenuFrame") { return MenuFrame; }
    else if (str == "MenuItem") { return MenuItem; }
    else if (str == "MenuItemSeparator") { return MenuItemSeparator; }
    else if (str == "ToolButton") { return ToolButton; }
    else if (str == "Splitter") { return Splitter; }
    else if (str == "DockWidgetFrame") { return DockWidgetFrame; }
    else if (str == "DockWidgetTitle") { return DockWidgetTitle; }
    else if (str == "WindowFrame") { return WindowFrame; }
    else if (str == "WindowTitle") { return WindowTitle; }
    else if (str == "WindowButton") { return WindowButton; }
    break;
  case Cokoon::Document::VariableDecl:
    if (false) { return -1; }
    break;
  case Cokoon::Document::IdentifierDecl:
    if (false) { return -1; }
    else if (str == "Generic_DefaultFrameWidth") { return Exp_Generic_DefaultFrameWidth; }
    else if (str == "Button_DefaultIndicatorMargin") { return Exp_Button_DefaultIndicatorMargin; }
    else if (str == "Button_ContentsMargin") { return Exp_Button_ContentsMargin; }
    else if (str == "Button_FocusMargin") { return Exp_Button_FocusMargin; }
    else if (str == "Button_PressedShiftHorizontal") { return Exp_Button_PressedShiftHorizontal; }
    else if (str == "Button_PressedShiftVertical") { return Exp_Button_PressedShiftVertical; }
    else if (str == "CheckBox_Size") { return Exp_CheckBox_Size; }
    else if (str == "RadioButton_Size") { return Exp_RadioButton_Size; }
    else if (str == "ComboBox_FrameWidth") { return Exp_ComboBox_FrameWidth; }
    else if (str == "ComboBox_ButtonWidth") { return Exp_ComboBox_ButtonWidth; }
    else if (str == "ComboBox_FocusMargin") { return Exp_ComboBox_FocusMargin; }
    else if (str == "SpinBox_FrameWidth") { return Exp_SpinBox_FrameWidth; }
    else if (str == "SpinBox_ButtonWidth") { return Exp_SpinBox_ButtonWidth; }
    else if (str == "ScrollBar_BarWidth") { return Exp_ScrollBar_BarWidth; }
    else if (str == "ScrollBar_ButtonHeight") { return Exp_ScrollBar_ButtonHeight; }
    else if (str == "TabBarTab_ContentsMargin") { return Exp_TabBarTab_ContentsMargin; }
    else if (str == "TabBarTab_FocusMargin") { return Exp_TabBarTab_FocusMargin; }
    else if (str == "TabBarTab_TextToIconSpace") { return Exp_TabBarTab_TextToIconSpace; }
    else if (str == "TabBar_BaseHeight") { return Exp_TabBar_BaseHeight; }
    else if (str == "TabBar_BaseOverlap") { return Exp_TabBar_BaseOverlap; }
    else if (str == "SliderHandle_Thickness") { return Exp_SliderHandle_Thickness; }
    else if (str == "SliderHandle_Length") { return Exp_SliderHandle_Length; }
    else if (str == "ProgressBar_GrooveMargin") { return Exp_ProgressBar_GrooveMargin; }
    else if (str == "ProgressBar_SideText") { return Exp_ProgressBar_SideText; }
    else if (str == "ProgressBar_BusyIndicatorSize") { return Exp_ProgressBar_BusyIndicatorSize; }
    else if (str == "TabBar_ScrollButtonWidth") { return Exp_TabBar_ScrollButtonWidth; }
    else if (str == "Header_ContentsMargin") { return Exp_Header_ContentsMargin; }
    else if (str == "Header_TextToIconSpace") { return Exp_Header_TextToIconSpace; }
    else if (str == "ToolBoxTab_Margin") { return Exp_ToolBoxTab_Margin; }
    else if (str == "ToolBarHandle_Extent") { return Exp_ToolBarHandle_Extent; }
    else if (str == "ToolBar_ExtensionExtent") { return Exp_ToolBar_ExtensionExtent; }
    else if (str == "ToolBarSeparator_Extent") { return Exp_ToolBarSeparator_Extent; }
    else if (str == "ToolBar_ItemMargin") { return Exp_ToolBar_ItemMargin; }
    else if (str == "ToolBar_ItemSpacing") { return Exp_ToolBar_ItemSpacing; }
    else if (str == "MenuBarItem_Margin") { return Exp_MenuBarItem_Margin; }
    else if (str == "MenuFrame_Width") { return Exp_MenuFrame_Width; }
    else if (str == "MenuItem_Margin") { return Exp_MenuItem_Margin; }
    else if (str == "MenuItem_CheckSpace") { return Exp_MenuItem_CheckSpace; }
    else if (str == "MenuItem_IconWidth") { return Exp_MenuItem_IconWidth; }
    else if (str == "MenuItem_IconSpace") { return Exp_MenuItem_IconSpace; }
    else if (str == "MenuItem_AccelSpace") { return Exp_MenuItem_AccelSpace; }
    else if (str == "MenuItem_ArrowSpace") { return Exp_MenuItem_ArrowSpace; }
    else if (str == "MenuItem_ArrowWidth") { return Exp_MenuItem_ArrowWidth; }
    else if (str == "MenuItem_MinHeight") { return Exp_MenuItem_MinHeight; }
    else if (str == "MenuItemSeparator_Height") { return Exp_MenuItemSeparator_Height; }
    else if (str == "ToolButton_ContentsMargin") { return Exp_ToolButton_ContentsMargin; }
    else if (str == "ToolButton_FocusMargin") { return Exp_ToolButton_FocusMargin; }
    else if (str == "Splitter_Width") { return Exp_Splitter_Width; }
    else if (str == "DockWidget_FrameWidth") { return Exp_DockWidget_FrameWidth; }
    else if (str == "DockWidget_SeparatorExtent") { return Exp_DockWidget_SeparatorExtent; }
    else if (str == "Window_TitleHeight") { return Exp_Window_TitleHeight; }
    else if (str == "Window_NoTitleFrame") { return Exp_Window_NoTitleFrame; }
    else if (str == "Window_TitleMargin") { return Exp_Window_TitleMargin; }
    else if (str == "Window_ButtonWidth") { return Exp_Window_ButtonWidth; }
    else if (str == "Window_ButtonSpace") { return Exp_Window_ButtonSpace; }
    else if (str == "Window_ButtonToTextSpace") { return Exp_Window_ButtonToTextSpace; }
    break;
  }
return Cokoon::Document::mapToId(type,str); }
int customIdMappingBase(Cokoon::Document::DeclarationType type) const {
  switch(type) {
  case Cokoon::Document::ObjectStateDecl: return Items__Count;
  case Cokoon::Document::VariableDecl:   return Var::Variables__Count;
  case Cokoon::Document::IdentifierDecl: return Identifiers__Count;
  }
return Cokoon::Document::customIdMappingBase(type); }
    int objectStateLevels(int objId) const {
        switch(objId) {
        case LayoutProperties:
            return 0;
        case GenericFrame:
            return 2;
        case Button:
            return 2;
        case CheckBox:
            return 2;
        case RadioButton:
            return 2;
        case LineEdit:
            return 1;
        case ComboBox:
            return 2;
        case SpinBox:
            return 2;
        case ScrollBarButton:
            return 2;
        case ScrollBarGroove:
            return 2;
        case ScrollBarHandle:
            return 2;
        case SliderGroove:
            return 3;
        case SliderHandle:
            return 3;
        case ProgressBarGroove:
            return 2;
        case ProgressBarBar:
            return 2;
        case TabBarTab:
            return 6;
        case Header:
            return 4;
        case ToolBoxTab:
            return 2;
        case ToolBarHandle:
            return 2;
        case ToolBarSeparator:
            return 1;
        case MenuBarItem:
            return 1;
        case MenuFrame:
            return 0;
        case MenuItem:
            return 1;
        case MenuItemSeparator:
            return 0;
        case ToolButton:
            return 1;
        case Splitter:
            return 2;
        case DockWidgetFrame:
            return 0;
        case DockWidgetTitle:
            return 0;
        case WindowFrame:
            return 1;
        case WindowTitle:
            return 1;
        case WindowButton:
            return 3;
        default: return -1;
        }
    }
    int objectStateLevelStates(int itemId, int stateLevel) const {
        switch(itemId) {
        case LayoutProperties:
            switch(stateLevel) {
            default: return -1;
            }
        case GenericFrame:
            switch(stateLevel) {
            case 0: return 3;
            case 1: return 2;
            default: return -1;
            }
        case Button:
            switch(stateLevel) {
            case 0: return 2;
            case 1: return 4;
            default: return -1;
            }
        case CheckBox:
            switch(stateLevel) {
            case 0: return 4;
            case 1: return 3;
            default: return -1;
            }
        case RadioButton:
            switch(stateLevel) {
            case 0: return 4;
            case 1: return 2;
            default: return -1;
            }
        case LineEdit:
            switch(stateLevel) {
            case 0: return 3;
            default: return -1;
            }
        case ComboBox:
            switch(stateLevel) {
            case 0: return 4;
            case 1: return 2;
            default: return -1;
            }
        case SpinBox:
            switch(stateLevel) {
            case 0: return 6;
            case 1: return 2;
            default: return -1;
            }
        case ScrollBarButton:
            switch(stateLevel) {
            case 0: return 3;
            case 1: return 4;
            default: return -1;
            }
        case ScrollBarGroove:
            switch(stateLevel) {
            case 0: return 3;
            case 1: return 2;
            default: return -1;
            }
        case ScrollBarHandle:
            switch(stateLevel) {
            case 0: return 3;
            case 1: return 2;
            default: return -1;
            }
        case SliderGroove:
            switch(stateLevel) {
            case 0: return 2;
            case 1: return 2;
            case 2: return 2;
            default: return -1;
            }
        case SliderHandle:
            switch(stateLevel) {
            case 0: return 4;
            case 1: return 2;
            case 2: return 2;
            default: return -1;
            }
        case ProgressBarGroove:
            switch(stateLevel) {
            case 0: return 2;
            case 1: return 2;
            default: return -1;
            }
        case ProgressBarBar:
            switch(stateLevel) {
            case 0: return 2;
            case 1: return 2;
            default: return -1;
            }
        case TabBarTab:
            switch(stateLevel) {
            case 0: return 3;
            case 1: return 4;
            case 2: return 4;
            case 3: return 4;
            case 4: return 2;
            case 5: return 2;
            default: return -1;
            }
        case Header:
            switch(stateLevel) {
            case 0: return 3;
            case 1: return 5;
            case 2: return 4;
            case 3: return 2;
            default: return -1;
            }
        case ToolBoxTab:
            switch(stateLevel) {
            case 0: return 3;
            case 1: return 2;
            default: return -1;
            }
        case ToolBarHandle:
            switch(stateLevel) {
            case 0: return 3;
            case 1: return 2;
            default: return -1;
            }
        case ToolBarSeparator:
            switch(stateLevel) {
            case 0: return 2;
            default: return -1;
            }
        case MenuBarItem:
            switch(stateLevel) {
            case 0: return 3;
            default: return -1;
            }
        case MenuFrame:
            switch(stateLevel) {
            default: return -1;
            }
        case MenuItem:
            switch(stateLevel) {
            case 0: return 1;
            default: return -1;
            }
        case MenuItemSeparator:
            switch(stateLevel) {
            default: return -1;
            }
        case ToolButton:
            switch(stateLevel) {
            case 0: return 4;
            default: return -1;
            }
        case Splitter:
            switch(stateLevel) {
            case 0: return 3;
            case 1: return 2;
            default: return -1;
            }
        case DockWidgetFrame:
            switch(stateLevel) {
            default: return -1;
            }
        case DockWidgetTitle:
            switch(stateLevel) {
            default: return -1;
            }
        case WindowFrame:
            switch(stateLevel) {
            case 0: return 2;
            default: return -1;
            }
        case WindowTitle:
            switch(stateLevel) {
            case 0: return 2;
            default: return -1;
            }
        case WindowButton:
            switch(stateLevel) {
            case 0: return 2;
            case 1: return 2;
            case 2: return 7;
            default: return -1;
            }
        default: return -1;
        }
    }
    int mapObjectStateToId(int itemId, int stateLevel, const QString &stateName) const {
        switch(itemId) {
        case LayoutProperties:
            switch(stateLevel) {
            default: return -1;
            }
            break;
        case GenericFrame:
            switch(stateLevel) {
            case 0:
              if (stateName == "raised") { return GenericFrame0_raised; }
              if (stateName == "sunken") { return GenericFrame0_sunken; }
              if (stateName == "plain") { return GenericFrame0_plain; }
                else return -1;
             break;
            case 1:
              if (stateName == "normal") { return GenericFrame1_normal; }
              if (stateName == "focused") { return GenericFrame1_focused; }
                else return -1;
             break;
            default: return -1;
            }
            break;
        case Button:
            switch(stateLevel) {
            case 0:
              if (stateName == "normal") { return Button0_normal; }
              if (stateName == "default") { return Button0_default; }
                else return -1;
             break;
            case 1:
              if (stateName == "normal") { return Button1_normal; }
              if (stateName == "pressed") { return Button1_pressed; }
              if (stateName == "prelight") { return Button1_prelight; }
              if (stateName == "disabled") { return Button1_disabled; }
                else return -1;
             break;
            default: return -1;
            }
            break;
        case CheckBox:
            switch(stateLevel) {
            case 0:
              if (stateName == "normal") { return CheckBox0_normal; }
              if (stateName == "pressed") { return CheckBox0_pressed; }
              if (stateName == "prelight") { return CheckBox0_prelight; }
              if (stateName == "disabled") { return CheckBox0_disabled; }
                else return -1;
             break;
            case 1:
              if (stateName == "on") { return CheckBox1_on; }
              if (stateName == "off") { return CheckBox1_off; }
              if (stateName == "tristate") { return CheckBox1_tristate; }
                else return -1;
             break;
            default: return -1;
            }
            break;
        case RadioButton:
            switch(stateLevel) {
            case 0:
              if (stateName == "normal") { return RadioButton0_normal; }
              if (stateName == "pressed") { return RadioButton0_pressed; }
              if (stateName == "prelight") { return RadioButton0_prelight; }
              if (stateName == "disabled") { return RadioButton0_disabled; }
                else return -1;
             break;
            case 1:
              if (stateName == "on") { return RadioButton1_on; }
              if (stateName == "off") { return RadioButton1_off; }
                else return -1;
             break;
            default: return -1;
            }
            break;
        case LineEdit:
            switch(stateLevel) {
            case 0:
              if (stateName == "normal") { return LineEdit0_normal; }
              if (stateName == "focused") { return LineEdit0_focused; }
              if (stateName == "disabled") { return LineEdit0_disabled; }
                else return -1;
             break;
            default: return -1;
            }
            break;
        case ComboBox:
            switch(stateLevel) {
            case 0:
              if (stateName == "normal") { return ComboBox0_normal; }
              if (stateName == "pressed") { return ComboBox0_pressed; }
              if (stateName == "prelight") { return ComboBox0_prelight; }
              if (stateName == "disabled") { return ComboBox0_disabled; }
                else return -1;
             break;
            case 1:
              if (stateName == "normal") { return ComboBox1_normal; }
              if (stateName == "focused") { return ComboBox1_focused; }
                else return -1;
             break;
            default: return -1;
            }
            break;
        case SpinBox:
            switch(stateLevel) {
            case 0:
              if (stateName == "normal") { return SpinBox0_normal; }
              if (stateName == "pressedUp") { return SpinBox0_pressedUp; }
              if (stateName == "pressedDown") { return SpinBox0_pressedDown; }
              if (stateName == "prelightUp") { return SpinBox0_prelightUp; }
              if (stateName == "prelightDown") { return SpinBox0_prelightDown; }
              if (stateName == "disabled") { return SpinBox0_disabled; }
                else return -1;
             break;
            case 1:
              if (stateName == "normal") { return SpinBox1_normal; }
              if (stateName == "focused") { return SpinBox1_focused; }
                else return -1;
             break;
            default: return -1;
            }
            break;
        case ScrollBarButton:
            switch(stateLevel) {
            case 0:
              if (stateName == "normal") { return ScrollBarButton0_normal; }
              if (stateName == "pressed") { return ScrollBarButton0_pressed; }
              if (stateName == "disabled") { return ScrollBarButton0_disabled; }
                else return -1;
             break;
            case 1:
              if (stateName == "left") { return ScrollBarButton1_left; }
              if (stateName == "right") { return ScrollBarButton1_right; }
              if (stateName == "up") { return ScrollBarButton1_up; }
              if (stateName == "down") { return ScrollBarButton1_down; }
                else return -1;
             break;
            default: return -1;
            }
            break;
        case ScrollBarGroove:
            switch(stateLevel) {
            case 0:
              if (stateName == "normal") { return ScrollBarGroove0_normal; }
              if (stateName == "pressed") { return ScrollBarGroove0_pressed; }
              if (stateName == "disabled") { return ScrollBarGroove0_disabled; }
                else return -1;
             break;
            case 1:
              if (stateName == "horizontal") { return ScrollBarGroove1_horizontal; }
              if (stateName == "vertical") { return ScrollBarGroove1_vertical; }
                else return -1;
             break;
            default: return -1;
            }
            break;
        case ScrollBarHandle:
            switch(stateLevel) {
            case 0:
              if (stateName == "normal") { return ScrollBarHandle0_normal; }
              if (stateName == "pressed") { return ScrollBarHandle0_pressed; }
              if (stateName == "disabled") { return ScrollBarHandle0_disabled; }
                else return -1;
             break;
            case 1:
              if (stateName == "horizontal") { return ScrollBarHandle1_horizontal; }
              if (stateName == "vertical") { return ScrollBarHandle1_vertical; }
                else return -1;
             break;
            default: return -1;
            }
            break;
        case SliderGroove:
            switch(stateLevel) {
            case 0:
              if (stateName == "normal") { return SliderGroove0_normal; }
              if (stateName == "disabled") { return SliderGroove0_disabled; }
                else return -1;
             break;
            case 1:
              if (stateName == "horizontal") { return SliderGroove1_horizontal; }
              if (stateName == "vertical") { return SliderGroove1_vertical; }
                else return -1;
             break;
            case 2:
              if (stateName == "notFocused") { return SliderGroove2_notFocused; }
              if (stateName == "focused") { return SliderGroove2_focused; }
                else return -1;
             break;
            default: return -1;
            }
            break;
        case SliderHandle:
            switch(stateLevel) {
            case 0:
              if (stateName == "normal") { return SliderHandle0_normal; }
              if (stateName == "prelight") { return SliderHandle0_prelight; }
              if (stateName == "pressed") { return SliderHandle0_pressed; }
              if (stateName == "disabled") { return SliderHandle0_disabled; }
                else return -1;
             break;
            case 1:
              if (stateName == "horizontal") { return SliderHandle1_horizontal; }
              if (stateName == "vertical") { return SliderHandle1_vertical; }
                else return -1;
             break;
            case 2:
              if (stateName == "notFocused") { return SliderHandle2_notFocused; }
              if (stateName == "focused") { return SliderHandle2_focused; }
                else return -1;
             break;
            default: return -1;
            }
            break;
        case ProgressBarGroove:
            switch(stateLevel) {
            case 0:
              if (stateName == "normal") { return ProgressBarGroove0_normal; }
              if (stateName == "disabled") { return ProgressBarGroove0_disabled; }
                else return -1;
             break;
            case 1:
              if (stateName == "horizontal") { return ProgressBarGroove1_horizontal; }
              if (stateName == "vertical") { return ProgressBarGroove1_vertical; }
                else return -1;
             break;
            default: return -1;
            }
            break;
        case ProgressBarBar:
            switch(stateLevel) {
            case 0:
              if (stateName == "normal") { return ProgressBarBar0_normal; }
              if (stateName == "disabled") { return ProgressBarBar0_disabled; }
                else return -1;
             break;
            case 1:
              if (stateName == "horizontal") { return ProgressBarBar1_horizontal; }
              if (stateName == "vertical") { return ProgressBarBar1_vertical; }
                else return -1;
             break;
            default: return -1;
            }
            break;
        case TabBarTab:
            switch(stateLevel) {
            case 0:
              if (stateName == "normal") { return TabBarTab0_normal; }
              if (stateName == "prelight") { return TabBarTab0_prelight; }
              if (stateName == "disabled") { return TabBarTab0_disabled; }
                else return -1;
             break;
            case 1:
              if (stateName == "selected") { return TabBarTab1_selected; }
              if (stateName == "nextIsSelected") { return TabBarTab1_nextIsSelected; }
              if (stateName == "previousIsSelected") { return TabBarTab1_previousIsSelected; }
              if (stateName == "notAdjacent") { return TabBarTab1_notAdjacent; }
                else return -1;
             break;
            case 2:
              if (stateName == "beginning") { return TabBarTab2_beginning; }
              if (stateName == "middle") { return TabBarTab2_middle; }
              if (stateName == "end") { return TabBarTab2_end; }
              if (stateName == "onlyOne") { return TabBarTab2_onlyOne; }
                else return -1;
             break;
            case 3:
              if (stateName == "north") { return TabBarTab3_north; }
              if (stateName == "east") { return TabBarTab3_east; }
              if (stateName == "south") { return TabBarTab3_south; }
              if (stateName == "west") { return TabBarTab3_west; }
                else return -1;
             break;
            case 4:
              if (stateName == "notFocused") { return TabBarTab4_notFocused; }
              if (stateName == "focused") { return TabBarTab4_focused; }
                else return -1;
             break;
            case 5:
              if (stateName == "rounded") { return TabBarTab5_rounded; }
              if (stateName == "triangular") { return TabBarTab5_triangular; }
                else return -1;
             break;
            default: return -1;
            }
            break;
        case Header:
            switch(stateLevel) {
            case 0:
              if (stateName == "normal") { return Header0_normal; }
              if (stateName == "prelight") { return Header0_prelight; }
              if (stateName == "disabled") { return Header0_disabled; }
                else return -1;
             break;
            case 1:
              if (stateName == "selected") { return Header1_selected; }
              if (stateName == "nextIsSelected") { return Header1_nextIsSelected; }
              if (stateName == "previousIsSelected") { return Header1_previousIsSelected; }
              if (stateName == "nextAndPreviousAreSelected") { return Header1_nextAndPreviousAreSelected; }
              if (stateName == "notAdjacent") { return Header1_notAdjacent; }
                else return -1;
             break;
            case 2:
              if (stateName == "beginning") { return Header2_beginning; }
              if (stateName == "middle") { return Header2_middle; }
              if (stateName == "end") { return Header2_end; }
              if (stateName == "onlyOne") { return Header2_onlyOne; }
                else return -1;
             break;
            case 3:
              if (stateName == "horizontal") { return Header3_horizontal; }
              if (stateName == "vertical") { return Header3_vertical; }
                else return -1;
             break;
            default: return -1;
            }
            break;
        case ToolBoxTab:
            switch(stateLevel) {
            case 0:
              if (stateName == "normal") { return ToolBoxTab0_normal; }
              if (stateName == "pressed") { return ToolBoxTab0_pressed; }
              if (stateName == "disabled") { return ToolBoxTab0_disabled; }
                else return -1;
             break;
            case 1:
              if (stateName == "selected") { return ToolBoxTab1_selected; }
              if (stateName == "notSelected") { return ToolBoxTab1_notSelected; }
                else return -1;
             break;
            default: return -1;
            }
            break;
        case ToolBarHandle:
            switch(stateLevel) {
            case 0:
              if (stateName == "normal") { return ToolBarHandle0_normal; }
              if (stateName == "pressed") { return ToolBarHandle0_pressed; }
              if (stateName == "prelight") { return ToolBarHandle0_prelight; }
                else return -1;
             break;
            case 1:
              if (stateName == "horizontal") { return ToolBarHandle1_horizontal; }
              if (stateName == "vertical") { return ToolBarHandle1_vertical; }
                else return -1;
             break;
            default: return -1;
            }
            break;
        case ToolBarSeparator:
            switch(stateLevel) {
            case 0:
              if (stateName == "horizontal") { return ToolBarSeparator0_horizontal; }
              if (stateName == "vertical") { return ToolBarSeparator0_vertical; }
                else return -1;
             break;
            default: return -1;
            }
            break;
        case MenuBarItem:
            switch(stateLevel) {
            case 0:
              if (stateName == "normal") { return MenuBarItem0_normal; }
              if (stateName == "pressed") { return MenuBarItem0_pressed; }
              if (stateName == "prelight") { return MenuBarItem0_prelight; }
                else return -1;
             break;
            default: return -1;
            }
            break;
        case MenuFrame:
            switch(stateLevel) {
            default: return -1;
            }
            break;
        case MenuItem:
            switch(stateLevel) {
            case 0:
              if (stateName == "prelight") { return MenuItem0_prelight; }
                else return -1;
             break;
            default: return -1;
            }
            break;
        case MenuItemSeparator:
            switch(stateLevel) {
            default: return -1;
            }
            break;
        case ToolButton:
            switch(stateLevel) {
            case 0:
              if (stateName == "normal") { return ToolButton0_normal; }
              if (stateName == "pressed") { return ToolButton0_pressed; }
              if (stateName == "prelight") { return ToolButton0_prelight; }
              if (stateName == "disabled") { return ToolButton0_disabled; }
                else return -1;
             break;
            default: return -1;
            }
            break;
        case Splitter:
            switch(stateLevel) {
            case 0:
              if (stateName == "normal") { return Splitter0_normal; }
              if (stateName == "pressed") { return Splitter0_pressed; }
              if (stateName == "prelight") { return Splitter0_prelight; }
                else return -1;
             break;
            case 1:
              if (stateName == "horizontal") { return Splitter1_horizontal; }
              if (stateName == "vertical") { return Splitter1_vertical; }
                else return -1;
             break;
            default: return -1;
            }
            break;
        case DockWidgetFrame:
            switch(stateLevel) {
            default: return -1;
            }
            break;
        case DockWidgetTitle:
            switch(stateLevel) {
            default: return -1;
            }
            break;
        case WindowFrame:
            switch(stateLevel) {
            case 0:
              if (stateName == "active") { return WindowFrame0_active; }
              if (stateName == "inactive") { return WindowFrame0_inactive; }
                else return -1;
             break;
            default: return -1;
            }
            break;
        case WindowTitle:
            switch(stateLevel) {
            case 0:
              if (stateName == "active") { return WindowTitle0_active; }
              if (stateName == "inactive") { return WindowTitle0_inactive; }
                else return -1;
             break;
            default: return -1;
            }
            break;
        case WindowButton:
            switch(stateLevel) {
            case 0:
              if (stateName == "active") { return WindowButton0_active; }
              if (stateName == "inactive") { return WindowButton0_inactive; }
                else return -1;
             break;
            case 1:
              if (stateName == "normal") { return WindowButton1_normal; }
              if (stateName == "pressed") { return WindowButton1_pressed; }
                else return -1;
             break;
            case 2:
              if (stateName == "minimize") { return WindowButton2_minimize; }
              if (stateName == "maximize") { return WindowButton2_maximize; }
              if (stateName == "restore") { return WindowButton2_restore; }
              if (stateName == "close") { return WindowButton2_close; }
              if (stateName == "shade") { return WindowButton2_shade; }
              if (stateName == "unshade") { return WindowButton2_unshade; }
              if (stateName == "help") { return WindowButton2_help; }
                else return -1;
             break;
            default: return -1;
            }
            break;
        default: return -1;
        }
    }
    int mapObjectStateToId(int itemId, int stateLevel, int stateNumber) const {
        switch(itemId) {
        case LayoutProperties:
            switch(stateLevel) {
            }
        case GenericFrame:
            switch(stateLevel) {
            case 0:
                switch(stateNumber) {
                case 0: return GenericFrame0_raised;
                case 1: return GenericFrame0_sunken;
                case 2: return GenericFrame0_plain;
                default: return -1;
                }
             break;
            case 1:
                switch(stateNumber) {
                case 0: return GenericFrame1_normal;
                case 1: return GenericFrame1_focused;
                default: return -1;
                }
             break;
            }
        case Button:
            switch(stateLevel) {
            case 0:
                switch(stateNumber) {
                case 0: return Button0_normal;
                case 1: return Button0_default;
                default: return -1;
                }
             break;
            case 1:
                switch(stateNumber) {
                case 0: return Button1_normal;
                case 1: return Button1_pressed;
                case 2: return Button1_prelight;
                case 3: return Button1_disabled;
                default: return -1;
                }
             break;
            }
        case CheckBox:
            switch(stateLevel) {
            case 0:
                switch(stateNumber) {
                case 0: return CheckBox0_normal;
                case 1: return CheckBox0_pressed;
                case 2: return CheckBox0_prelight;
                case 3: return CheckBox0_disabled;
                default: return -1;
                }
             break;
            case 1:
                switch(stateNumber) {
                case 0: return CheckBox1_on;
                case 1: return CheckBox1_off;
                case 2: return CheckBox1_tristate;
                default: return -1;
                }
             break;
            }
        case RadioButton:
            switch(stateLevel) {
            case 0:
                switch(stateNumber) {
                case 0: return RadioButton0_normal;
                case 1: return RadioButton0_pressed;
                case 2: return RadioButton0_prelight;
                case 3: return RadioButton0_disabled;
                default: return -1;
                }
             break;
            case 1:
                switch(stateNumber) {
                case 0: return RadioButton1_on;
                case 1: return RadioButton1_off;
                default: return -1;
                }
             break;
            }
        case LineEdit:
            switch(stateLevel) {
            case 0:
                switch(stateNumber) {
                case 0: return LineEdit0_normal;
                case 1: return LineEdit0_focused;
                case 2: return LineEdit0_disabled;
                default: return -1;
                }
             break;
            }
        case ComboBox:
            switch(stateLevel) {
            case 0:
                switch(stateNumber) {
                case 0: return ComboBox0_normal;
                case 1: return ComboBox0_pressed;
                case 2: return ComboBox0_prelight;
                case 3: return ComboBox0_disabled;
                default: return -1;
                }
             break;
            case 1:
                switch(stateNumber) {
                case 0: return ComboBox1_normal;
                case 1: return ComboBox1_focused;
                default: return -1;
                }
             break;
            }
        case SpinBox:
            switch(stateLevel) {
            case 0:
                switch(stateNumber) {
                case 0: return SpinBox0_normal;
                case 1: return SpinBox0_pressedUp;
                case 2: return SpinBox0_pressedDown;
                case 3: return SpinBox0_prelightUp;
                case 4: return SpinBox0_prelightDown;
                case 5: return SpinBox0_disabled;
                default: return -1;
                }
             break;
            case 1:
                switch(stateNumber) {
                case 0: return SpinBox1_normal;
                case 1: return SpinBox1_focused;
                default: return -1;
                }
             break;
            }
        case ScrollBarButton:
            switch(stateLevel) {
            case 0:
                switch(stateNumber) {
                case 0: return ScrollBarButton0_normal;
                case 1: return ScrollBarButton0_pressed;
                case 2: return ScrollBarButton0_disabled;
                default: return -1;
                }
             break;
            case 1:
                switch(stateNumber) {
                case 0: return ScrollBarButton1_left;
                case 1: return ScrollBarButton1_right;
                case 2: return ScrollBarButton1_up;
                case 3: return ScrollBarButton1_down;
                default: return -1;
                }
             break;
            }
        case ScrollBarGroove:
            switch(stateLevel) {
            case 0:
                switch(stateNumber) {
                case 0: return ScrollBarGroove0_normal;
                case 1: return ScrollBarGroove0_pressed;
                case 2: return ScrollBarGroove0_disabled;
                default: return -1;
                }
             break;
            case 1:
                switch(stateNumber) {
                case 0: return ScrollBarGroove1_horizontal;
                case 1: return ScrollBarGroove1_vertical;
                default: return -1;
                }
             break;
            }
        case ScrollBarHandle:
            switch(stateLevel) {
            case 0:
                switch(stateNumber) {
                case 0: return ScrollBarHandle0_normal;
                case 1: return ScrollBarHandle0_pressed;
                case 2: return ScrollBarHandle0_disabled;
                default: return -1;
                }
             break;
            case 1:
                switch(stateNumber) {
                case 0: return ScrollBarHandle1_horizontal;
                case 1: return ScrollBarHandle1_vertical;
                default: return -1;
                }
             break;
            }
        case SliderGroove:
            switch(stateLevel) {
            case 0:
                switch(stateNumber) {
                case 0: return SliderGroove0_normal;
                case 1: return SliderGroove0_disabled;
                default: return -1;
                }
             break;
            case 1:
                switch(stateNumber) {
                case 0: return SliderGroove1_horizontal;
                case 1: return SliderGroove1_vertical;
                default: return -1;
                }
             break;
            case 2:
                switch(stateNumber) {
                case 0: return SliderGroove2_notFocused;
                case 1: return SliderGroove2_focused;
                default: return -1;
                }
             break;
            }
        case SliderHandle:
            switch(stateLevel) {
            case 0:
                switch(stateNumber) {
                case 0: return SliderHandle0_normal;
                case 1: return SliderHandle0_prelight;
                case 2: return SliderHandle0_pressed;
                case 3: return SliderHandle0_disabled;
                default: return -1;
                }
             break;
            case 1:
                switch(stateNumber) {
                case 0: return SliderHandle1_horizontal;
                case 1: return SliderHandle1_vertical;
                default: return -1;
                }
             break;
            case 2:
                switch(stateNumber) {
                case 0: return SliderHandle2_notFocused;
                case 1: return SliderHandle2_focused;
                default: return -1;
                }
             break;
            }
        case ProgressBarGroove:
            switch(stateLevel) {
            case 0:
                switch(stateNumber) {
                case 0: return ProgressBarGroove0_normal;
                case 1: return ProgressBarGroove0_disabled;
                default: return -1;
                }
             break;
            case 1:
                switch(stateNumber) {
                case 0: return ProgressBarGroove1_horizontal;
                case 1: return ProgressBarGroove1_vertical;
                default: return -1;
                }
             break;
            }
        case ProgressBarBar:
            switch(stateLevel) {
            case 0:
                switch(stateNumber) {
                case 0: return ProgressBarBar0_normal;
                case 1: return ProgressBarBar0_disabled;
                default: return -1;
                }
             break;
            case 1:
                switch(stateNumber) {
                case 0: return ProgressBarBar1_horizontal;
                case 1: return ProgressBarBar1_vertical;
                default: return -1;
                }
             break;
            }
        case TabBarTab:
            switch(stateLevel) {
            case 0:
                switch(stateNumber) {
                case 0: return TabBarTab0_normal;
                case 1: return TabBarTab0_prelight;
                case 2: return TabBarTab0_disabled;
                default: return -1;
                }
             break;
            case 1:
                switch(stateNumber) {
                case 0: return TabBarTab1_selected;
                case 1: return TabBarTab1_nextIsSelected;
                case 2: return TabBarTab1_previousIsSelected;
                case 3: return TabBarTab1_notAdjacent;
                default: return -1;
                }
             break;
            case 2:
                switch(stateNumber) {
                case 0: return TabBarTab2_beginning;
                case 1: return TabBarTab2_middle;
                case 2: return TabBarTab2_end;
                case 3: return TabBarTab2_onlyOne;
                default: return -1;
                }
             break;
            case 3:
                switch(stateNumber) {
                case 0: return TabBarTab3_north;
                case 1: return TabBarTab3_east;
                case 2: return TabBarTab3_south;
                case 3: return TabBarTab3_west;
                default: return -1;
                }
             break;
            case 4:
                switch(stateNumber) {
                case 0: return TabBarTab4_notFocused;
                case 1: return TabBarTab4_focused;
                default: return -1;
                }
             break;
            case 5:
                switch(stateNumber) {
                case 0: return TabBarTab5_rounded;
                case 1: return TabBarTab5_triangular;
                default: return -1;
                }
             break;
            }
        case Header:
            switch(stateLevel) {
            case 0:
                switch(stateNumber) {
                case 0: return Header0_normal;
                case 1: return Header0_prelight;
                case 2: return Header0_disabled;
                default: return -1;
                }
             break;
            case 1:
                switch(stateNumber) {
                case 0: return Header1_selected;
                case 1: return Header1_nextIsSelected;
                case 2: return Header1_previousIsSelected;
                case 3: return Header1_nextAndPreviousAreSelected;
                case 4: return Header1_notAdjacent;
                default: return -1;
                }
             break;
            case 2:
                switch(stateNumber) {
                case 0: return Header2_beginning;
                case 1: return Header2_middle;
                case 2: return Header2_end;
                case 3: return Header2_onlyOne;
                default: return -1;
                }
             break;
            case 3:
                switch(stateNumber) {
                case 0: return Header3_horizontal;
                case 1: return Header3_vertical;
                default: return -1;
                }
             break;
            }
        case ToolBoxTab:
            switch(stateLevel) {
            case 0:
                switch(stateNumber) {
                case 0: return ToolBoxTab0_normal;
                case 1: return ToolBoxTab0_pressed;
                case 2: return ToolBoxTab0_disabled;
                default: return -1;
                }
             break;
            case 1:
                switch(stateNumber) {
                case 0: return ToolBoxTab1_selected;
                case 1: return ToolBoxTab1_notSelected;
                default: return -1;
                }
             break;
            }
        case ToolBarHandle:
            switch(stateLevel) {
            case 0:
                switch(stateNumber) {
                case 0: return ToolBarHandle0_normal;
                case 1: return ToolBarHandle0_pressed;
                case 2: return ToolBarHandle0_prelight;
                default: return -1;
                }
             break;
            case 1:
                switch(stateNumber) {
                case 0: return ToolBarHandle1_horizontal;
                case 1: return ToolBarHandle1_vertical;
                default: return -1;
                }
             break;
            }
        case ToolBarSeparator:
            switch(stateLevel) {
            case 0:
                switch(stateNumber) {
                case 0: return ToolBarSeparator0_horizontal;
                case 1: return ToolBarSeparator0_vertical;
                default: return -1;
                }
             break;
            }
        case MenuBarItem:
            switch(stateLevel) {
            case 0:
                switch(stateNumber) {
                case 0: return MenuBarItem0_normal;
                case 1: return MenuBarItem0_pressed;
                case 2: return MenuBarItem0_prelight;
                default: return -1;
                }
             break;
            }
        case MenuFrame:
            switch(stateLevel) {
            }
        case MenuItem:
            switch(stateLevel) {
            case 0:
                switch(stateNumber) {
                case 0: return MenuItem0_prelight;
                default: return -1;
                }
             break;
            }
        case MenuItemSeparator:
            switch(stateLevel) {
            }
        case ToolButton:
            switch(stateLevel) {
            case 0:
                switch(stateNumber) {
                case 0: return ToolButton0_normal;
                case 1: return ToolButton0_pressed;
                case 2: return ToolButton0_prelight;
                case 3: return ToolButton0_disabled;
                default: return -1;
                }
             break;
            }
        case Splitter:
            switch(stateLevel) {
            case 0:
                switch(stateNumber) {
                case 0: return Splitter0_normal;
                case 1: return Splitter0_pressed;
                case 2: return Splitter0_prelight;
                default: return -1;
                }
             break;
            case 1:
                switch(stateNumber) {
                case 0: return Splitter1_horizontal;
                case 1: return Splitter1_vertical;
                default: return -1;
                }
             break;
            }
        case DockWidgetFrame:
            switch(stateLevel) {
            }
        case DockWidgetTitle:
            switch(stateLevel) {
            }
        case WindowFrame:
            switch(stateLevel) {
            case 0:
                switch(stateNumber) {
                case 0: return WindowFrame0_active;
                case 1: return WindowFrame0_inactive;
                default: return -1;
                }
             break;
            }
        case WindowTitle:
            switch(stateLevel) {
            case 0:
                switch(stateNumber) {
                case 0: return WindowTitle0_active;
                case 1: return WindowTitle0_inactive;
                default: return -1;
                }
             break;
            }
        case WindowButton:
            switch(stateLevel) {
            case 0:
                switch(stateNumber) {
                case 0: return WindowButton0_active;
                case 1: return WindowButton0_inactive;
                default: return -1;
                }
             break;
            case 1:
                switch(stateNumber) {
                case 0: return WindowButton1_normal;
                case 1: return WindowButton1_pressed;
                default: return -1;
                }
             break;
            case 2:
                switch(stateNumber) {
                case 0: return WindowButton2_minimize;
                case 1: return WindowButton2_maximize;
                case 2: return WindowButton2_restore;
                case 3: return WindowButton2_close;
                case 4: return WindowButton2_shade;
                case 5: return WindowButton2_unshade;
                case 6: return WindowButton2_help;
                default: return -1;
                }
             break;
            }
        default: return -1;
        }
    }
};
}
#endif // CokoonStyleSpec_H
