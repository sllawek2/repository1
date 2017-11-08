IloIntArray(const IloEnv env, IloInt n, const IloInt v0, ...);
IloArray< > templatowy obiekt do tworzenia tablic obiektów cplexowych, można tak zrobić dwuwymiarową tablicę (tablicę tablic):
IloArray<IloIntArray>
IloInt - int
IloNum - double
IloBool - IloFalse to 0, IloTrue to 1
IloBoolArray

IloIntExprArg IloCount(const IloIntVarArray vars, IloInt value); zwraca ile razy wartość value występuje w tablicy vars. IloIntExprArg jest typem określającym ilość

IloIntExprArg IloScalProd(const IloIntVarArray vars, 
                            const IloIntArray values);



wszystko co ma Var w nazwie to zmienne decyzyjne - to cplex określa ich wartość 
IloIntVarArray
IloNumVarArray
IloBoolVarArray

