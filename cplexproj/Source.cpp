
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
  
  IloInt w;//szeroko�� produktu [30,80]
  IloInt h;//wysoko�� produktu [30,50]
  IloIntVar x1; //bli�sza kraw�d� na osi X
  IloIntVar x2; //dalsza kraw�d� na osi X
  IloIntVar y1;
  IloIntVar y2;
  //IloIntVar x; //po�o�enie �rodka produktu
  //IloIntVar y;
  //gap = 3 //odst�p pomiedzy produktami
  //gap frame = 10 //przerwa pomi�dzy palet� a produktami
  //box limit 180x180 //wymiary palety
  IloNum pole; //pole a*b
  IloBoolVar o; //orientacja o {0,1} bez obrotu, obr�t
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

int main()
{
  IloEnv env;

  std::vector<Klocek> wszystkieKlocki;
  if(true == wczytanieDanych(env, "dane.txt",wszystkieKlocki))
  {

    try
    {
      IloModel model(env);
    
      //stworzenie tablicy p�l wszystkich klock�w
      IloNumArray iPola(env);
      IloBoolVarArray iTabWybranych(env);
      IloNum Sumapol=0;//suma wszystkich pol
      for(int i = 0; i<ILOSC_KLOCKOW; i++)
      {
        iPola.add(wszystkieKlocki[i].pole);
        iTabWybranych.add(wszystkieKlocki[i].p);
        Sumapol=Sumapol+wszystkieKlocki[i].pole;
      }
    
      IloIntVar x_max(env, a, b-a);//uzyte tylko w drugim wariancie
      IloIntVar y_max(env, a, c-a);//uzyte tylko w drugim wariancie
      
      bool bPierwszyWariant = true;
      if(Sumapol < 0.75 * b*c)
      {
        bPierwszyWariant = false;
      }
    
      //ograniczenia
      for(int i = 0; i<ILOSC_KLOCKOW; i++) //i to k 
      {
        dodajWspolneOgraniczenia(model, wszystkieKlocki[i]);
    
        if(true == bPierwszyWariant)
        {
          for(int j = i+1; j<ILOSC_KLOCKOW; j++) //j to f
          {
            dodajOgraniczeniaPierwszegoWariantu(model, wszystkieKlocki[i], wszystkieKlocki[j]);
          }
        }
        else
        {
          for(int j = i+1; j<ILOSC_KLOCKOW ; j++) //j to f
          {
            dodajOgraniczeniaDrugiegoWariantu(model, wszystkieKlocki[i], wszystkieKlocki[j]);
          }
          model.add(wszystkieKlocki[i].x2 <= x_max);
          model.add(wszystkieKlocki[i].y2 <= y_max);
        }
      }
    
      //dodanie do modelu celu
      if(true == bPierwszyWariant)
      {
        model.add(IloMaximize(env ,IloScalProd(iPola,iTabWybranych)));
      }
      else
      {
        model.add(IloMinimize(env, x_max + y_max));
      }
    
      IloCplex cplex(model);
      cplex.setParam(IloCplex::TiLim, 60);//limit czasowy
     // cplex.setOut(env.getNullStream());
      //cplex.setWarning(env.getNullStream());
      cplex.solve();
    
    
      cout<<"Suma pol wszystkich klockow = "<<Sumapol<<", Pole palety = "<< c*b << endl;
      wyswietlenieWynikow(cplex, bPierwszyWariant, wszystkieKlocki);
      rysowanieWPliku(cplex,bPierwszyWariant, wszystkieKlocki);

    }
    catch (IloException& ex) {
      cerr << "Error: " << ex << endl;
    }
    catch (...) {
      cerr << "Error" << endl;
    }
  }
  
  
  env.end();

  getchar();//stop 
  return 0;
}