#include"imp_type_checker.hh"

ImpTypeChecker::ImpTypeChecker():inttype(),booltype(),voidtype() {
  inttype.set_basic_type("int");
  booltype.set_basic_type("bool");
  voidtype.set_basic_type("void");
  list<string> noparams;
  maintype.set_fun_type(noparams,"void");
  has_main = false;
  insideConditional = false;
}

void ImpTypeChecker::sp_incr(int n) {
    sp += n;
 //   cout << "[DEBUG] sp_incr: sp = " << sp << endl;
    if (sp > max_sp) max_sp = sp;
}

void ImpTypeChecker::sp_decr(int n) {
    sp -= n;
  //  cout << "[DEBUG] sp_decr: sp = " << sp << endl;
    if (sp < 0) {
        cout << "[ERROR] stack less than 0" << endl;
        exit(1);
    }
}


void ImpTypeChecker::typecheck(Program* p) {
  env.clear();
  p->accept(this);
  return;
}

void ImpTypeChecker::visit(Program* p) {
  env.add_level();
  ftable.add_level();
  p->vardecs->accept(this);
  p->fundecs->accept(this);
  if (!has_main) {
    cout << "Programa no tiene main" << endl;
    exit(0);
  }
  env.remove_level();

  for(int i = 0; i < fnames.size(); i++) {
    cout << "-- Function: " << fnames[i] << endl;
    FEntry fentry = ftable.lookup(fnames[i]);

  

    cout << fentry.fname << " : " << fentry.ftype << endl;
    // added
    cout << "max stack height: " << fentry.max_stack << endl;
    cout << "mem local variables: " << fentry.mem_locals << endl;
  }
  
  return;
}

void ImpTypeChecker::visit(Body* b) {
  int start_dir = dir;  // added
  env.add_level();
  b->vardecs->accept(this);
  b->slist->accept(this);
  env.remove_level();
  dir = start_dir;  // added
  return;
}

void ImpTypeChecker::visit(VarDecList* decs) {
  list<VarDec*>::iterator it;
  for (it = decs->vardecs.begin(); it != decs->vardecs.end(); ++it) {
    (*it)->accept(this);
  }  
  return;
}

void ImpTypeChecker::visit(FunDecList* s) {
  list<FunDec*>::iterator it;
  for (it = s->flist.begin(); it != s->flist.end(); ++it) {
    add_fundec(*it);
  }
  for (it = s->flist.begin(); it != s->flist.end(); ++it) {
    // added
    sp = max_sp = 0;
    dir = max_dir = 0;
    // end-added
    (*it)->accept(this);
    FEntry fentry;
    string fname  = (*it)->fname;
    fentry.fname = fname;
    fentry.ftype = env.lookup(fname);
    fnames.push_back(fname);
    // added
    fentry.max_stack = max_sp;
    fentry.mem_locals = max_dir;
    // end added
    ftable.add_var(fname, fentry);
  }
  return;
}

void ImpTypeChecker::visit(VarDec* vd) {
  ImpType type;  
  type.set_basic_type(vd->type);
  if (type.ttype==ImpType::NOTYPE || type.ttype==ImpType::VOID) {
    cout << "Tipo invalido: " << vd->type << endl;
    exit(0);
  }
  list<string>::iterator it;
  for (it = vd->vars.begin(); it != vd->vars.end(); ++it) {
    env.add_var(*it, type);
    dir++;
    if (dir > max_dir) max_dir = dir;
  }   
  return;
}

void ImpTypeChecker::add_fundec(FunDec* fd) {
  ImpType funtype;


  if (!funtype.set_fun_type(fd->types, fd->rtype)) {
    cout << "Tipo invalido en declaracion de funcion: " << fd->fname << endl;
    exit(0);
  }

  // Validar función principal (main)
  if (fd->fname.compare("main") == 0) {
    if (!funtype.match(maintype)) {
      cout << "Tipo incorrecto de main: " << funtype << endl;
      exit(0);
    }
    has_main = true;
  }

  // Agregar a la tabla de símbolos
  env.add_var(fd->fname, funtype);
}


void ImpTypeChecker::visit(FunDec* fd) {
    env.add_level();

    // Configurar el estado de la función actual
    currentFunction = fd->fname;
    hasReturned = false; // Reiniciar la bandera para esta función

    // Verificar tipos de parámetros y retorno
    ImpType funtype = env.lookup(fd->fname);
    ImpType rtype, ptype;
    rtype.set_basic_type(funtype.types.back());
    list<string>::iterator it;
    int i = 0;
    for (it = fd->vars.begin(); it != fd->vars.end(); ++it, i++) {
        ptype.set_basic_type(funtype.types[i]);
        env.add_var(*it, ptype);
    }
    env.add_var("return", rtype);
    
    // Procesar el cuerpo de la función
    fd->body->accept(this);

    // Validar que solo hubo un retorno
    if (!hasReturned && rtype.ttype != ImpType::VOID) {
        cout << "Error: La función '" << fd->fname << "' no tiene un retorno explícito." << endl;
        exit(1);
    }

    // Limpiar el estado de la función actual
    currentFunction = "";
    env.remove_level();
}


void ImpTypeChecker::visit(StatementList* s) {
  list<Stm*>::iterator it;
  for (it = s->stms.begin(); it != s->stms.end(); ++it) {
    (*it)->accept(this);
  }
  return;
}

void ImpTypeChecker::visit(AssignStatement* s) {
    ImpType type = s->rhs->accept(this);

    // Verificar si la asignación es al nombre de la función actual


    if (!env.check(s->id)) {
        cout << "Variable " << s->id << " indefinida." << endl;
        exit(1);
    }
    ImpType var_type = env.lookup(s->id);
    if (!type.match(var_type)) {
        cout << "Tipo incorrecto en Assign a " << s->id << endl;
        exit(1);
    }
    sp_decr(1);
}

void ImpTypeChecker::visit(PrintStatement* s) {
  //s->e->accept(this);
  for (auto f :s->e)
    {
        f->accept(this);
    }

  sp_decr(1);
  return;
}
void ImpTypeChecker::visit(IfStatement* s) {
    if (!s->condition->accept(this).match(booltype)) {
        cout << "Expresion conditional en IF debe de ser bool" << endl;
        exit(0);
    }
    sp_decr(1); // Ajustar la pila tras evaluar la condición

    s->then->accept(this); // Procesar el bloque `then`
    if (s->els != NULL) {
        s->els->accept(this); // Procesar el bloque `else`, si existe
    }

    return;
}

void ImpTypeChecker::visit(WhileStatement* s) {
    // Evaluar la condición del while
    if (!s->condition->accept(this).match(booltype)) {
        cout << "Expresion conditional en WHILE debe de ser bool" << endl;
        exit(0);
    }

    // Decrementar la pila tras evaluar la condición
    sp_decr(1);

    // Evaluar el cuerpo del while
    s->b->accept(this);
}

void ImpTypeChecker::visit(ReturnStatement* s) {


    hasReturned = true; // Marcar que ya hubo un retorno

    ImpType rtype = env.lookup("return");
    ImpType etype;

    if (rtype.ttype == ImpType::VOID) {
        cout << "Error: No se permite 'return' en procedimientos." << endl;
        exit(1);
    }

    if (s->e != NULL) {
        etype = s->e->accept(this);
        sp_decr(1); // Decrementar solo si hay una expresión en el return
    } else {
        etype = voidtype;
    }

    if (!rtype.match(etype)) {
        cout << "Return type mismatch: " << rtype << " <-> " << etype << endl;
        exit(0);
    }

    return;
}

void ImpTypeChecker::visit(ForStatement* s) {
  if(!s->start->rhs->accept(this).match(inttype) || !s->end->accept(this).match(inttype) || !s->step->accept(this).match(inttype)) {
    cout << "Expresiones en for deben de ser int" << endl;
    exit(0);
  }
  sp_decr(3);
  s->b->accept(this);
  return;
}


ImpType ImpTypeChecker::visit(BinaryExp* e) {
    ImpType t1 = e->left->accept(this);
    sp_decr(1); // Ajustar después de evaluar el operando izquierdo

    ImpType t2 = e->right->accept(this);
    sp_decr(1); // Ajustar después de evaluar el operando derecho

    if (!t1.match(inttype) || !t2.match(inttype)) {
        cout << "Tipos en BinExp deben de ser int" << endl;
        exit(0);
    }
    sp_incr(1);

    ImpType result;
    switch (e->op) {
        case PLUS_OP:
        case MINUS_OP:
        case MUL_OP:
        case DIV_OP:
            result = inttype;
            break;
        case LT_OP:
        case LE_OP:
        case EQ_OP:
            result = booltype;
            break;
    }

    sp_incr(1); // Incrementar por el resultado
    return result;
}

ImpType ImpTypeChecker::visit(NumberExp* e) {
  sp_incr(1);
  return inttype;
}

ImpType ImpTypeChecker::visit(BoolExp* e) {
  sp_incr(1);
  return booltype;
}

ImpType ImpTypeChecker::visit(IdentifierExp* e) {
  sp_incr(1);
  if (env.check(e->name))
    return env.lookup(e->name);
  else {
    cout << "Variable indefinida: " << e->name << endl;
    exit(0);
  }
}

ImpType ImpTypeChecker::visit(IFExp* e) {
  if (!e->cond->accept(this).match(booltype)) {
    cout << "Tipo en ifexp debe de ser bool" << endl;
    exit(0);
  }
  sp_decr(1);
  int sp_start = sp;
  ImpType ttype =  e->left->accept(this);
  sp = sp_start;  //   sp_decr(1);
  if (!ttype.match(e->right->accept(this))) {
    cout << "Tipos en ifexp deben de ser iguales" << endl;
    exit(0);
  }
  
  return ttype;
}

ImpType ImpTypeChecker::visit(FCallExp* e) {
  return process_fcall(e);
}



ImpType ImpTypeChecker::process_fcall(FCallExp* e) {
    if (!env.check(e->fname)) {
      cout << "(Function call): " << e->fname <<  " no existe" << endl;
      exit(0);
    }
    ImpType funtype = env.lookup(e->fname);
    if (funtype.ttype != ImpType::FUN) {
      cout << "(Function call): " << e->fname <<  " no es una funcion" << endl;
      exit(0);
    }

    // check args
    int num_fun_args = funtype.types.size()-1;
    int num_fcall_args = e->args.size();
    ImpType rtype;
    rtype.set_basic_type(funtype.types[num_fun_args]);
    // added
    if (!rtype.match(voidtype)) {
      sp_incr(1);
    }
    if (num_fun_args != num_fcall_args) {
      cout << "(Function call) Numero de argumentos no corresponde a declaracion de: " << e->fname << endl;
      exit(0);
    }
    ImpType argtype;
    list<Exp*>::iterator it;
    int i = 0;
    for (it = e->args.begin(); it != e->args.end(); ++it) {
      argtype = (*it)->accept(this);
      if (argtype.ttype != funtype.types[i]) {
          cout << "(Function call) Tipo de argumento no corresponde a tipo de parametro en fcall de: " << e->fname << endl;
          exit(0);
      }
      i++;
    }
    sp_decr(i);
    return rtype;
}

//Si o si debe ser de tipo void
void ImpTypeChecker::visit(FCallStatement* s) {
    if (!env.check(s->fname)) {
        cout << "(Function call): " << s->fname << " no existe" << endl;
        exit(0);
    }

    ImpType funtype = env.lookup(s->fname);

    if (funtype.ttype != ImpType::FUN) {
        cout << "(Function call): " << s->fname << " no es una funcion" << endl;
        exit(0);
    }

    // Verificar que la función sea de tipo void
    ImpType rtype;
    rtype.set_basic_type(funtype.types.back());
    if (!rtype.match(voidtype)) {
        cout << "(Function call): " << s->fname << " no es de tipo void" << endl;
        exit(0);
    }

    // Verificar los argumentos
    int num_fun_args = funtype.types.size() - 1;
    int num_fcall_args = s->args.size();
    if (num_fun_args != num_fcall_args) {
        cout << "(Function call) Numero de argumentos no corresponde a declaracion de: " << s->fname << endl;
        exit(0);
    }

    // Evaluar los argumentos y ajustar la pila
    list<Exp*>::iterator it;
    int i = 0;
    for (it = s->args.begin(); it != s->args.end(); ++it) {
        (*it)->accept(this);
        sp_decr(1); // Ajustar la pila por cada argumento
    }

    return;
}
