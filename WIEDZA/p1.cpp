#include <ilcplex/ilocplex.h>

ILOSTLBEGIN

int  main()
{
	IloEnv                env;
	IloModel      model(env );
	IloNumVarArray    x(env );//tablica zmiennych decyzyjnych

	//dodajemy kolejne elementy do tablicy
	x.add(IloNumVar (env , 0, 40)); //zakres od 0 do 40
	x.add(IloNumVar (env ));
	x.add(IloNumVar (env ));

	//dodanie do modelu równañ ograniczaj¹cych
	model.add( - x[0] +      x[1] + x[2]  <= 20);
	model.add(    x[0]  - 3 * x[1] + x[2]  <= 30);

	//dodanie do modelu celu - maksymalizacja tego równania
	model.add(IloMaximize(env , x[0]+2* x[1]+3* x[2]));

	//rozwi¹zanie
	IloCplex  cplex(model );
	cplex.solve ();

	cout  << "Max=" << cplex.getObjValue () << endl;
	env.end ();

	//zatrzymanie programu
	getchar();
}