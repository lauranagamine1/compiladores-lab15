#include <iostream>
#include "exp.h"
using namespace std;
BinaryExp::BinaryExp(Exp* l, Exp* r, BinaryOp op):left(l),right(r),op(op) {}
NumberExp::NumberExp(int v):value(v) {}
IdentifierExp::IdentifierExp(const string& n):name(n) {}
Exp::~Exp() {}
BinaryExp::~BinaryExp() { delete left; delete right; }
NumberExp::~NumberExp() { }
IdentifierExp::~IdentifierExp() { }
AssignStatement::AssignStatement(string id, Exp* e): id(id), rhs(e) {}
AssignStatement::~AssignStatement() {
    delete rhs;
}
PrintStatement::PrintStatement(Exp* e): e(e) {}
PrintStatement::~PrintStatement() {
    delete e;
}
Program::Program() {}
void Program::add(Stm* s) {
    slist.push_back(s);
}
Program::~Program() {
    for (Stm* s : slist) {
        delete s;
    }
}
Stm::~Stm() {}
char Exp::binopToChar(BinaryOp op) {
    char  c=' ';
    switch(op) {
        case PLUS_OP: c = '+'; break;
        case MINUS_OP: c = '-'; break;
        case MUL_OP: c = '*'; break;
        case DIV_OP: c = '/'; break;
        default: c = '$';
    }
    return c;
}