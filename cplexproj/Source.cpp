
#include <ilcplex/ilocplex.h>




ILOSTLBEGIN

#include <fstream>
#include <string>
#include <vector>
#include <iostream>

	
const unsigned int ILOSC_KLOCKOW = 27;

IloInt a = 10;//gap frame
IloInt b = 180, c = 180;

class Klocek{
public:
  Klocek(const IloEnv &env,unsigned int w, unsigned int h):
	  w(w),o(env,0,1),h(h),pole(w*h),p(env,0,1), x1(env, a, b-a), x2(env, a, b-a),y1(env, a, c-a), y2(env, a, c-a), d1(env,0,1), d2(env,0,1), d3(env,0,1), d4(env,0,1)
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
  IloBoolVar o; //orientacja o {0,1} bez obrotu, obrót
  IloIntVar p;// 0 nie wybrany, 1 wybrany
  
  IloIntVar d1;
  IloIntVar d2;
  IloIntVar d3;
  IloIntVar d4;

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
	for(int i = 0; i<iIlosc; i++) //i to k
	{ 
		model.add(wszystkieKlocki[i].x1 >= a);
		model.add(wszystkieKlocki[i].x2 <= b-a);
		model.add(wszystkieKlocki[i].y1 >= a);
		model.add(wszystkieKlocki[i].y2 <= c-a);
			
		model.add(wszystkieKlocki[i].x1 - wszystkieKlocki[i].x2 <0);
		model.add(wszystkieKlocki[i].y1 - wszystkieKlocki[i].y2<0);  
		
		for(int j = i+1; j<iIlosc ; j++) //j to f
		{
			model.add(b*(1-wszystkieKlocki[i].p) + b*wszystkieKlocki[i].d1 + wszystkieKlocki[j].x1 - wszystkieKlocki[i].x2 +a >=0);
			model.add(b*(1-wszystkieKlocki[i].p) + b*wszystkieKlocki[i].d2 + wszystkieKlocki[i].x1 - wszystkieKlocki[j].x2 +a >=0);
			model.add(c*(1-wszystkieKlocki[i].p) + c*wszystkieKlocki[i].d3 + wszystkieKlocki[j].y1 - wszystkieKlocki[i].y2 +a >=0);
			model.add(c*(1-wszystkieKlocki[i].p) + c*wszystkieKlocki[i].d4 + wszystkieKlocki[i].y1 - wszystkieKlocki[j].y2 +a >=0);

			model.add(wszystkieKlocki[i].d1+wszystkieKlocki[i].d2+wszystkieKlocki[i].d3+wszystkieKlocki[i].d4<=3);

			

		}
		//model.add(wszystkieKlocki[i].pole == (wszystkieKlocki[i].x2 -wszystkieKlocki[i].x1) * (wszystkieKlocki[i].y2 -wszystkieKlocki[i].y1) );
		//ostatni warunek - suma
	}
	model.add(wszystkieKlocki[0].pole*wszystkieKlocki[0].p+
			  wszystkieKlocki[1].pole*wszystkieKlocki[1].p+
			  wszystkieKlocki[2].pole*wszystkieKlocki[2].p+
			  wszystkieKlocki[3].pole*wszystkieKlocki[3].p+
		      wszystkieKlocki[4].pole*wszystkieKlocki[4].p+
			  wszystkieKlocki[5].pole*wszystkieKlocki[5].p+
			  wszystkieKlocki[6].pole*wszystkieKlocki[6].p+
			  wszystkieKlocki[7].pole*wszystkieKlocki[7].p+
			  wszystkieKlocki[8].pole*wszystkieKlocki[8].p+
			  wszystkieKlocki[9].pole*wszystkieKlocki[9].p+
		      wszystkieKlocki[10].pole*wszystkieKlocki[10].p+
			  wszystkieKlocki[11].pole*wszystkieKlocki[11].p+
			  wszystkieKlocki[12].pole*wszystkieKlocki[12].p+
			  wszystkieKlocki[13].pole*wszystkieKlocki[13].p+
			  wszystkieKlocki[14].pole*wszystkieKlocki[14].p+
			  wszystkieKlocki[15].pole*wszystkieKlocki[15].p+
			  wszystkieKlocki[16].pole*wszystkieKlocki[16].p+
			  wszystkieKlocki[18].pole*wszystkieKlocki[18].p+
			  wszystkieKlocki[19].pole*wszystkieKlocki[19].p+
			  wszystkieKlocki[20].pole*wszystkieKlocki[20].p+
			  wszystkieKlocki[21].pole*wszystkieKlocki[21].p+
			  wszystkieKlocki[22].pole*wszystkieKlocki[22].p+
		      wszystkieKlocki[23].pole*wszystkieKlocki[23].p+
			  wszystkieKlocki[24].pole*wszystkieKlocki[24].p+
			  wszystkieKlocki[25].pole*wszystkieKlocki[25].p+
			  wszystkieKlocki[26].pole*wszystkieKlocki[26].p <= (b-a)*(c-a));
	//Suma wszystkich pol
	IloInt Sumapol=0;
	for(int powt_petli=0;powt_petli<=ILOSC_KLOCKOW;powt_petli++)
	{
		
		Sumapol=Sumapol+wszystkieKlocki[powt_petli].pole;
	}
 
	//IloIntVar k(env, 0, ILOSC_KLOCKOW), f(env, 1, ILOSC_KLOCKOW);
	//model.add(f>=k+1);
	//model.add(wszystkieKlocki[f].x1 - wszystkieKlocki[k].x2 >0);

	//dodanie do modelu celu - maksymalizacja tego równania
	/*model.add(IloMaximize(env ,IloScalProd(iPola,iTabWybranych)));*/
	model.add(IloMaximize(env, wszystkieKlocki[0].pole*wszystkieKlocki[0].p+
							   wszystkieKlocki[1].pole*wszystkieKlocki[1].p+
							   wszystkieKlocki[2].pole*wszystkieKlocki[2].p+
							   wszystkieKlocki[3].pole*wszystkieKlocki[3].p+
							   wszystkieKlocki[4].pole*wszystkieKlocki[4].p+
							   wszystkieKlocki[5].pole*wszystkieKlocki[5].p+
							   wszystkieKlocki[6].pole*wszystkieKlocki[6].p+
							   wszystkieKlocki[7].pole*wszystkieKlocki[7].p+
							   wszystkieKlocki[8].pole*wszystkieKlocki[8].p+
							   wszystkieKlocki[9].pole*wszystkieKlocki[9].p+
							   wszystkieKlocki[10].pole*wszystkieKlocki[10].p+
							   wszystkieKlocki[11].pole*wszystkieKlocki[11].p+
							   wszystkieKlocki[12].pole*wszystkieKlocki[12].p+
							   wszystkieKlocki[13].pole*wszystkieKlocki[13].p+
							   wszystkieKlocki[14].pole*wszystkieKlocki[14].p+
							   wszystkieKlocki[15].pole*wszystkieKlocki[15].p+
							   wszystkieKlocki[16].pole*wszystkieKlocki[16].p+
							   wszystkieKlocki[18].pole*wszystkieKlocki[18].p+
							   wszystkieKlocki[19].pole*wszystkieKlocki[19].p+
							   wszystkieKlocki[20].pole*wszystkieKlocki[20].p+
							   wszystkieKlocki[21].pole*wszystkieKlocki[21].p+
							   wszystkieKlocki[22].pole*wszystkieKlocki[22].p+
							   wszystkieKlocki[23].pole*wszystkieKlocki[23].p+
							   wszystkieKlocki[24].pole*wszystkieKlocki[24].p+
							   wszystkieKlocki[25].pole*wszystkieKlocki[25].p+
							   wszystkieKlocki[26].pole*wszystkieKlocki[26].p));
		
	


    // Optimize
    IloCplex cplex(model);
   // cplex.setOut(env.getNullStream());
    //cplex.setWarning(env.getNullStream());
    cplex.solve();

	
	cout  << "Suma pol wszystkich klockow = "<< Sumapol << endl;
	cout  << "Max po optymailizacji i ograniczeniach =" << cplex.getObjValue () << endl;
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



