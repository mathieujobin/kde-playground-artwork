#ifndef CokoonTestSpec_H
#define CokoonTestSpec_H
namespace CokoonTestSpec {
namespace Var {
enum Variables {
  var01,
  var02,
  Variables__Count
};
}
enum Identifiers {
  Exp_varValue01,
  Exp_varValue02,
  Exp_exp,
  Exp_expAssert,
  Identifiers__Count
};
enum Items {
  Expressions = 0,
  Painting = 1,
  Items__Count
};
enum ExpressionsStates0 {
  Expressions0_test01 = 0,
  Expressions0_test02 = 2,
  Expressions0_test03 = 4,
  Expressions0_test04 = 6,
  Expressions0_test05 = 8,
  Expressions0_test06 = 10,
  Expressions0_test07 = 12,
  Expressions0_test08 = 14,
  Expressions0_test09 = 16,
  Expressions0_test10 = 18,
  Expressions0_test11 = 20,
  Expressions0_test12 = 22,
  Expressions0_test13 = 24,
  Expressions0_test14 = 26,
  Expressions0_test15 = 28,
  Expressions0_test16 = 30,
  Expressions0_test17 = 32,
  Expressions0_test18 = 34,
  Expressions0_test19 = 36,
  Expressions0_test20 = 38
};
enum PaintingStates0 {
  Painting0_test01 = 0
};
class SpecDocument : public Cokoon::Document {
protected:
int mapToId(Cokoon::Document::DeclarationType type,const QString &str) const {
  switch(type) {
  case Cokoon::Document::ObjectNameDecl:
    if (false) { return -1; }
    else if (str == "Expressions") { return Expressions; }
    else if (str == "Painting") { return Painting; }
    break;
  case Cokoon::Document::VariableDecl:
    if (false) { return -1; }
    else if (str == "var01") { return Var::var01; }
    else if (str == "var02") { return Var::var02; }
    break;
  case Cokoon::Document::IdentifierDecl:
    if (false) { return -1; }
    else if (str == "varValue01") { return Exp_varValue01; }
    else if (str == "varValue02") { return Exp_varValue02; }
    else if (str == "exp") { return Exp_exp; }
    else if (str == "expAssert") { return Exp_expAssert; }
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
        case Expressions:
            return 1;
        case Painting:
            return 1;
        default: return -1;
        }
    }
    int objectStateLevelStates(int itemId, int stateLevel) const {
        switch(itemId) {
        case Expressions:
            switch(stateLevel) {
            case 0: return 20;
            default: return -1;
            }
        case Painting:
            switch(stateLevel) {
            case 0: return 1;
            default: return -1;
            }
        default: return -1;
        }
    }
    int mapObjectStateToId(int itemId, int stateLevel, const QString &stateName) const {
        switch(itemId) {
        case Expressions:
            switch(stateLevel) {
            case 0:
              if (stateName == "test01") { return Expressions0_test01; }
              if (stateName == "test02") { return Expressions0_test02; }
              if (stateName == "test03") { return Expressions0_test03; }
              if (stateName == "test04") { return Expressions0_test04; }
              if (stateName == "test05") { return Expressions0_test05; }
              if (stateName == "test06") { return Expressions0_test06; }
              if (stateName == "test07") { return Expressions0_test07; }
              if (stateName == "test08") { return Expressions0_test08; }
              if (stateName == "test09") { return Expressions0_test09; }
              if (stateName == "test10") { return Expressions0_test10; }
              if (stateName == "test11") { return Expressions0_test11; }
              if (stateName == "test12") { return Expressions0_test12; }
              if (stateName == "test13") { return Expressions0_test13; }
              if (stateName == "test14") { return Expressions0_test14; }
              if (stateName == "test15") { return Expressions0_test15; }
              if (stateName == "test16") { return Expressions0_test16; }
              if (stateName == "test17") { return Expressions0_test17; }
              if (stateName == "test18") { return Expressions0_test18; }
              if (stateName == "test19") { return Expressions0_test19; }
              if (stateName == "test20") { return Expressions0_test20; }
                else return -1;
             break;
            default: return -1;
            }
            break;
        case Painting:
            switch(stateLevel) {
            case 0:
              if (stateName == "test01") { return Painting0_test01; }
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
        case Expressions:
            switch(stateLevel) {
            case 0:
                switch(stateNumber) {
                case 0: return Expressions0_test01;
                case 1: return Expressions0_test02;
                case 2: return Expressions0_test03;
                case 3: return Expressions0_test04;
                case 4: return Expressions0_test05;
                case 5: return Expressions0_test06;
                case 6: return Expressions0_test07;
                case 7: return Expressions0_test08;
                case 8: return Expressions0_test09;
                case 9: return Expressions0_test10;
                case 10: return Expressions0_test11;
                case 11: return Expressions0_test12;
                case 12: return Expressions0_test13;
                case 13: return Expressions0_test14;
                case 14: return Expressions0_test15;
                case 15: return Expressions0_test16;
                case 16: return Expressions0_test17;
                case 17: return Expressions0_test18;
                case 18: return Expressions0_test19;
                case 19: return Expressions0_test20;
                default: return -1;
                }
             break;
            }
        case Painting:
            switch(stateLevel) {
            case 0:
                switch(stateNumber) {
                case 0: return Painting0_test01;
                default: return -1;
                }
             break;
            }
        default: return -1;
        }
    }
};
}
#endif // CokoonTestSpec_H
