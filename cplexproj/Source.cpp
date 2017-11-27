#include <ilcplex/ilocplex.h>




ILOSTLBEGIN

#include <fstream>
#include <string>
#include <vector>

	
const unsigned int ILOSC_KLOCKOW = 3;

IloInt a = 10;//gap frame
IloInt b = 180, c = 180;

class Klocek{
public:
  Klocek(const IloEnv &env,unsigned int w, unsigned int h):
	  w(w),h(h),pole(w*h),p(env,0,1), x1(env, a, b-a), x2(env, a, b-a),y1(env, a, c-a), y2(env, a, c-a)
  {

  }
//private:
  IloInt w;//szerokoœæ produktu [30,80]
  IloInt h;//wysokoœæ produktu [30,50]
  IloIntVar x1; //bli¿sza krawêdŸ na osi X
  IloIntVar x2; //dalsza krawêdŸ na osi X
  IloIntVar y1;
  IloIntVar y2;
  IloIntVar x; //po³o¿enie œrodka produktu
  IloIntVar y;
  //gap = 3 //odstêp pomiedzy produktami
  //gap frame = 10 //przerwa pomiêdzy palet¹ a produktami
  //box limit 180x180 //wymiary palety
  IloNum pole; //pole a*b
  //orientacja o {0,1} bez obrotu, obrót
  IloIntVar p;// 0 nie wybrany, 1 wybrany


};



int main()
{
	  IloEnv env;

  std::vector<Klocek> wszystkieKlocki;
  
  fstream fInputFile;
  fInputFile.open("dane.txt");
  if(fInputFile.good())
  {
    for(int i=0; i<ILOSC_KLOCKOW; i++)
    {
      int a=0,b=0;
      std::string sLine;
      std::getline(fInputFile,sLine);
      std::stringstream stream(sLine);
      stream>>a>>b;
      Klocek k(env,a,b);
      wszystkieKlocki.push_back(k);
    }
  }




  try {
    IloInt j;


    IloModel model(env);

    IloInt iIlosc = ILOSC_KLOCKOW;//27 klocków w podajniku
 
	//stworzenie tablicy pól wszystkich klocków
	IloNumArray iPola(env);
	IloIntVarArray iTabWybranych(env);
	for(int i = 0; i<iIlosc; i++)
	{
		iPola.add(wszystkieKlocki[i].pole);
		iTabWybranych.add(wszystkieKlocki[i].p);
	}

	//ograniczenia
	for(int i = 0; i<iIlosc; i++)
	{ 
		model.add(wszystkieKlocki[i].x1 >= a);
		model.add(wszystkieKlocki[i].x2 <= b-a);
		model.add(wszystkieKlocki[i].y1 >= a);
		model.add(wszystkieKlocki[i].y2 <= c-a);

		model.add(wszystkieKlocki[i].x1 < wszystkieKlocki[i].x2);
		model.add(wszystkieKlocki[i].y1 < wszystkieKlocki[i].y2);

		model.add(wszystkieKlocki[i].pole == (wszystkieKlocki[i].x2 -wszystkieKlocki[i].x1) * (wszystkieKlocki[i].y2 -wszystkieKlocki[i].y1));
		

	}


	//dodanie do modelu celu - maksymalizacja tego równania
	model.add(IloMaximize(env ,IloScalProd(iPola,iTabWybranych)));

    // Optimize
    IloCplex cplex(model);
    cplex.setOut(env.getNullStream());
    cplex.setWarning(env.getNullStream());
    cplex.solve();
  }
  catch (IloException& ex) {
    cerr << "Error: " << ex << endl;
  }
  catch (...) {
    cerr << "Error" << endl;
  }
  env.end();

  	getchar();//stop 

	return 0;
}

