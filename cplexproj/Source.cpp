
#include <ilcplex/ilocplex.h>

ILOSTLBEGIN

#include <fstream>
#include <string>
#include <vector>
#include <iostream>

	
const unsigned int ILOSC_KLOCKOW = 4;
const unsigned int JEDNOSTKA = 30;

///liczymy od 0 czy 1?

IloInt a = 1;//poczatek ukladu
IloInt b = 180 / JEDNOSTKA, c = 180 / JEDNOSTKA;//koniec ukladu

unsigned short tab[a][b] = {0};

class Klocek{
public:
  Klocek(const IloEnv &env,unsigned int w, unsigned int h, int nrKlocka):
	  w(w),o(env,0,1),h(h),pole(w*h),p(env,0,1), x1(env, a, b), x2(env, a, b),y1(env, a, c), y2(env, a, c), d1(env,0,1), d2(env,0,1), d3(env,0,1), d4(env,0,1), nrKlocka(nrKlocka)
  {

  }

  int nrKlocka;
  
  IloInt w;//szerokoœæ produktu [30,80]
  IloInt h;//wysokoœæ produktu [30,50]
  IloIntVar x1; //bli¿sza krawêdŸ na osi X
  IloIntVar x2; //dalsza krawêdŸ na osi X
  IloIntVar y1;
  IloIntVar y2;
  //IloIntVar x; //po³o¿enie œrodka produktu
  //IloIntVar y;
  //gap = 3 //odstêp pomiedzy produktami
  //gap frame = 10 //przerwa pomiêdzy palet¹ a produktami
  //box limit 180x180 //wymiary palety
  IloNum pole; //pole a*b
  IloBoolVar o; //orientacja o {0,1} bez obrotu, obrót
  IloBoolVar p;// 0 nie wybrany, 1 wybrany
  
  IloBoolVar d1;
  IloBoolVar d2;
  IloBoolVar d3;
  IloBoolVar d4;

  
  IloArray<IloArray<IloIntVarArray> > tab;
};



bool wczytanieDanych(const IloEnv& env, const string nazwa_pliku, std::vector<Klocek> &ref)
{
  fstream fInputFile;
  fInputFile.open(nazwa_pliku);
  if(fInputFile.good())
  {
    for(int i=0; i<ILOSC_KLOCKOW; i++)
    {
      int a=0,b=0;
      std::string sLine;
      std::getline(fInputFile,sLine);
      std::stringstream stream(sLine);
      stream>>a>>b;
      if(a>80 || a<30 || b>50 || b<30)
      {
        cout<<"bledne dane!!!!"<<endl;
        return false;
      }
      
	  a = a/JEDNOSTKA;
	  b = b/JEDNOSTKA;
	  
      Klocek k(env,a,b, i+1);
      ref.push_back(k);
    }
    return true;
  }
  else
  {
    cout<<"nieprawidlowy plik!"<<endl;
    return false;
  }
}

void dodajWspolneOgraniczenia(IloModel& modelRef, const Klocek & klocek)
{
  modelRef.add(klocek.x1 >= a);                          //4.2
  modelRef.add(klocek.x2 <= b);                        //4.3
  modelRef.add(klocek.y1 >= a);							//4.4
  modelRef.add(klocek.y2 <= c);						//4.5
  
  for(int i = 0; i<a; i++)
  {
	for(int j = 0; j<b; j++)
	{
	  klocek.tab[i][j]
	}
  }
  
  
//  modelRef.add(klocek.x1 + klocek.w + klocek.h*klocek.o - klocek.w*klocek.o - klocek.x2 == 0);//4.7 4.8
//  modelRef.add(klocek.y1 + klocek.h + (klocek.w - klocek.h)*klocek.o - klocek.y2 == 0); //4.9 4.10
}

void dodajOgraniczeniaPierwszegoWariantu(IloModel& modelRef, const Klocek& klocek, const Klocek& kolejnyKlocek)
{
  modelRef.add(klocek.d1+klocek.d2+klocek.d3+klocek.d4<=3);			//4.6
  modelRef.add(b-b*klocek.p + b*klocek.d1 + kolejnyKlocek.x1 - klocek.x2 -a >=0);		//4.11
  modelRef.add(b-b*klocek.p + b*klocek.d2 + klocek.x1 - kolejnyKlocek.x2 -a >=0);		//4.12
  modelRef.add(c-c*klocek.p + c*klocek.d3 + kolejnyKlocek.y1 - klocek.y2 -a >=0);		//4.13
  modelRef.add(c-c*klocek.p + c*klocek.d4 + klocek.y1 - kolejnyKlocek.y2 -a >=0);		//4.14
}

void dodajOgraniczeniaDrugiegoWariantu(IloModel& modelRef, const Klocek & klocek, const Klocek & kolejnyKlocek)
{
  modelRef.add(klocek.d1+klocek.d2+klocek.d3+klocek.d4<=3);		//4.6
  modelRef.add(b*klocek.d1 + kolejnyKlocek.x1 - klocek.x2 -a >=0);		//4.11
  modelRef.add(b*klocek.d2 + klocek.x1 - kolejnyKlocek.x2 -a >=0);		//4.12
  modelRef.add(c*klocek.d3 + kolejnyKlocek.y1 - klocek.y2 -a >=0);		//4.13
  modelRef.add(c*klocek.d4 + klocek.y1 - kolejnyKlocek.y2 -a >=0);		//4.14
}

void wyswietlenieWynikow(const IloCplex & cplex, const bool bPierwszyWariant, const std::vector<Klocek> & wszystkieKlocki)
{
  cout << "-----------------------------------------------------------"<<endl;
  cout << "status = "<< cplex.getStatus()<<endl;
  cout << "-----------------------------------------------------------"<<endl;
  if(true == bPierwszyWariant)
  {
    cout << "Max po optymalizacji i ograniczeniach =" << cplex.getObjValue() << endl;
  }
  else
  {
    cout <<"Minimum po optymalizacji i ograniczeniach ="<<cplex.getObjValue()<<endl;
  }
  cout << "------------------------------------------------------------------------"<<endl;
  for(int i = 0; i<ILOSC_KLOCKOW;i++)
  {
    cout<< "klocek["<<i<<"]:"<<endl;
    if(true == bPierwszyWariant)
    {
      cout<< "p="<<cplex.getValue(wszystkieKlocki[i].p)<<endl;
    }
    cout<< "\t[x1,y1]=["<<cplex.getValue(wszystkieKlocki[i].x1)<<","<<cplex.getValue(wszystkieKlocki[i].y1)<<"]"<<endl;
    cout<< "\t[x2,y2]=["<<cplex.getValue(wszystkieKlocki[i].x2)<<","<<cplex.getValue(wszystkieKlocki[i].y2)<<"]"<<endl;
    if(i<ILOSC_KLOCKOW-1)//poniewaz d nie sa dodawane dla ostatniego klocka
    {
      cout<< "\td1,d2,d3,d4=["<<cplex.getValue(wszystkieKlocki[i].d1)<<cplex.getValue(wszystkieKlocki[i].d2)<<cplex.getValue(wszystkieKlocki[i].d3)<<cplex.getValue(wszystkieKlocki[i].d4)<<"]"<<endl;
    }
  }
  cout << "-----------------------------------------------------------"<<endl;
}

void rysowanieWPliku(const IloCplex &cplex, const bool bPierwszyWariant, const vector<Klocek> & wszystkieKlocki)
{
  char tablica[180][180];
  memset(tablica,'.', 180*180);
  for(int i = 0; i<ILOSC_KLOCKOW; i++)
  {
    if(false == bPierwszyWariant || true == cplex.getValue(wszystkieKlocki[i].p))
    {
      for(int x=cplex.getValue(wszystkieKlocki[i].x1);x<cplex.getValue(wszystkieKlocki[i].x2);x++)
      {
        for(int y=cplex.getValue(wszystkieKlocki[i].y1);y<cplex.getValue(wszystkieKlocki[i].y2);y++)
        {
          tablica[x][y] ='*';
        }
      }
    }
  }
  
  fstream outputFile;
  outputFile.open("wynik.txt",'w');
  if(outputFile.good())
  {
    for(int y=180 -1;y>=0;y--)
    {
      for(int x =0; x<180;x++)
      {
        outputFile<<tablica[x][y];
      }
      outputFile<<endl;
    }
  }
  else
  {
    cout<<"problem z zapisem wyniku do pliku"<<endl;
  }
}

#include <ilcp/cp.h>

int main(int, const char * []) {
  IloEnv env;
  try {
    IloModel model(env);
    IloInt i,j;
    IloInt sizeSquare = 112;
    IloInt nbSquares  = 21;
    IloIntArray size(env, nbSquares, 50,42,37,35,33,29,27,25,24,19,18,17,16,15,11,9,8,7,6,4,2);
    IloIntervalVarArray x(env, nbSquares);
    IloIntervalVarArray y(env, nbSquares);
    IloCumulFunctionExpr rx(env);
    IloCumulFunctionExpr ry(env);
    char name[16];
	
	IloNumExpr obj(env);
    for (i=0; i<nbSquares; ++i) {
      sprintf(name, "X%ld", i);
      x[i] = IloIntervalVar(env, size[i], name);
      x[i].setEndMax(sizeSquare);
	  x[i].setOptional();
      sprintf(name, "Y%ld", i);
      y[i] = IloIntervalVar(env, size[i], name);
      y[i].setEndMax(sizeSquare);
	  y[i].setOptional();
      rx += IloPulse(x[i], size[i]);
      ry += IloPulse(y[i], size[i]);
      for (j=0; j<i; ++j)
        model.add((IloEndOf(x[i]) <= IloStartOf(x[j])) ||
                  (IloEndOf(x[j]) <= IloStartOf(x[i])) ||
                  (IloEndOf(y[i]) <= IloStartOf(y[j])) ||
                  (IloEndOf(y[j]) <= IloStartOf(y[i])));
				  
      obj+=(IloEndOf(x[i])-IloStartOf(x[i])) * (IloEndOf(y[i])-IloStartOf(y[i]) );
    }
	
    model.add(IloAlwaysIn(env, rx, 0, sizeSquare, sizeSquare, sizeSquare));
    model.add(IloAlwaysIn(env, ry, 0, sizeSquare, sizeSquare, sizeSquare));
	
	model.add(IloMaximize(obj);

    IloCP cp(model);
    IloSearchPhaseArray phases(env);
    phases.add(IloSearchPhase(env, x));
    phases.add(IloSearchPhase(env, y));
    if (cp.solve(phases)) {
      for (i=0; i<nbSquares; ++i) {
        cp.out() << "Square " << i << ": ["
                 << cp.getStart(x[i]) << "," << cp.getEnd(x[i])
                 << "] x ["
                 << cp.getStart(y[i]) << "," << cp.getEnd(y[i])
                 << "]" << std::endl;
      }
    }
  } catch (IloException& ex) {
    env.out() << "Caught: " << ex << std::endl;
  }
  env.end();
  return 0;
}