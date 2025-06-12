#include <iostream>
#include "exp.h"
#include "visitor.h"
#include <unordered_map>
using namespace std;
unordered_map<std::string, int> memoria;
///////////////////////////////////////////////////////////////////////////////////
int BinaryExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int NumberExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int IdentifierExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int AssignStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int PrintStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}



///////////////////////////////////////////////////////////////////////////////////

int PrintVisitor::visit(BinaryExp* exp) {
    exp->left->accept(this);
    cout << ' ' << Exp::binopToChar(exp->op) << ' ';
    exp->right->accept(this);
    return 0;
}

int PrintVisitor::visit(NumberExp* exp) {
    cout << exp->value;
    return 0;
}

int PrintVisitor::visit(IdentifierExp* exp) {
    cout << exp->name;
    return 0;
}

void PrintVisitor::visit(AssignStatement* stm) {
    cout << stm->id << " = ";
    stm->rhs->accept(this);
    cout << ";";
}

void PrintVisitor::visit(PrintStatement* stm) {
    cout << "print(";
    stm->e->accept(this);
    cout << ");";
}

void PrintVisitor::imprimir(Program* program){
    for (Stm* s : program->slist) {
        s->accept(this);
        cout << endl;
    }
};

///////////////////////////////////////////////////////////////////////////////////
int EVALVisitor::visit(BinaryExp* exp) {
    int result;
    int v1 = exp->left->accept(this);
    int v2 = exp->right->accept(this);
    switch(Exp::binopToChar(exp->op)) {
        case '+': result = v1 + v2; break;
        case '-': result = v1 - v2; break;
        case '*': result = v1 * v2; break;
        case '/':
            if(v2 != 0) result = v1 / v2;
            else {
                cout << "Error: división por cero" << endl;
                result = 0;
            }
            break;
        default:
            cout << "Operador desconocido" << endl;
            result = 0;
    }
    return result;
}

int EVALVisitor::visit(NumberExp* exp) {
    return exp->value;
}

int EVALVisitor::visit(IdentifierExp* exp) {
    return memoria[exp->name];

}

void EVALVisitor::visit(AssignStatement* stm) {
    memoria[stm->id] = stm->rhs->accept(this);
}

void EVALVisitor::visit(PrintStatement* stm) {
    cout << stm->e->accept(this);
}

void EVALVisitor::ejecutar(Program* program){
    for (Stm* s : program->slist) {
        s->accept(this);
    }
};

// gen code visitor
void GenCodeVisitor::gencode(Program* program){
    cout << ".data" << endl;
    cout << "print_fmt: .string \"%ld\\n\"" << endl;
    cout << ".text " << endl;
    cout << ".globl main " << endl;
    cout << "main: " << endl;
    cout << " pushq %rbp " << endl;
    cout << " movq %rsp, %rbp " << endl;

    int n_assign = 0;
    for (auto stm : program->slist) {
        if (dynamic_cast<AssignStatement*>(stm))
            n_assign++;
    }

    if(n_assign > 0){
        cout << " subq $" << n_assign * 8 << ", %rsp" << endl;
    }

    for(auto i:program->slist) {
        i->accept(this);
    }

    // retorno de main
    cout << "  movl $0, %eax " << endl;
    cout << "  leave " << endl;
    cout << "  ret " << endl;
    cout << ".section .note.GNU-stack,  \"\",@progbits " << endl;
}

int GenCodeVisitor::visit(BinaryExp* exp){
    exp->left->accept(this);
    switch(Exp::binopToChar(exp->op)){
        case '+':{
            cout<<" pushq %rax"<<endl;
            exp->right->accept(this);
            cout<<" movq %rax, %rcx"<<endl;
            cout<<" popq %rax"<<endl;
            cout<<" addq %rcx, %rax"<<endl;
            break;
        }
        case '-':{
            cout<< " pushq %rax"<<endl;
            exp->right->accept(this);
            cout<<" movq %rax, %rcx"<<endl;
            cout<<" popq %rax"<<endl;
            cout<<" subq %rcx, %rax"<<endl;
            break;
        }
        case '*':{
            cout<<" pushq %rax"<<endl;
            exp->right->accept(this);
            cout<<" movq %rax, %rcx"<<endl;
            break;
        }
        case '/':{
            cout << "  pushq %rax\n";
            exp->right->accept(this);
            cout<<" movq %rax, %rcx\n";
            cout<<" popq %rax\n";
            cout<<" cqo\n"
                  " idivq %rcx"<<endl;

            break;
        }
        default:
            cout<<"error: ingrese expresion binaria."<<endl;

    }
    return 0;
};

int GenCodeVisitor::visit(NumberExp* exp) {
    cout << " movq $"<<exp->value<<", %rax"<<endl;
    return 0;
};

int GenCodeVisitor::visit(IdentifierExp* exp) {
    cout<<" movq "<<memoria[exp->name]*-8<<"(%rbp), %rax"<<endl;
    return 0;
};

void GenCodeVisitor::visit(AssignStatement* stm) {
    stm->rhs->accept(this);
    memoria[stm->id] = contador;
    contador++;
    cout<< "movq %rax, "<<(memoria[stm->id])*-8<<"(%rbp)"<<endl;
}

void GenCodeVisitor::visit(PrintStatement* stm) {
    stm->e->accept(this);
    cout << " movq %rax, %rsi" << endl;
    cout << " leaq print_fmt(%rip), %rdi" << endl;
    cout << " movl $0, %eax" << endl;
    cout << " call printf@PLT" << endl;
};

