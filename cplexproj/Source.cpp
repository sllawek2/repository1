
#include <ilcplex/ilocplex.h>




ILOSTLBEGIN

#include <fstream>
#include <string>
#include <vector>
#include <iostream>

	
const unsigned int ILOSC_KLOCKOW = 4;

IloInt a = 10;//gap frame
IloInt b = 100, c = 180;

class Klocek{
public:
  Klocek(const IloEnv &env,unsigned int w, unsigned int h):
	  w(w),o(env,0,1),h(h),pole(w*h),p(env,0,1), x1(env, a, b-a), x2(env, a, b-a),y1(env, a, c-a), y2(env, a, c-a), d1(env,0,1), d2(env,0,1), d3(env,0,1), d4(env,0,1)
  {

  }

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
  IloBoolVar p;// 0 nie wybrany, 1 wybrany
  
  IloBoolVar d1;
  IloBoolVar d2;
  IloBoolVar d3;
  IloBoolVar d4;

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
	  if(a>80 || a<30 || b>50 || b<30)
	  {
		  cout<<"bledne dane!!!!"<<endl;
		  getchar();
		  exit(1);
	  }

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
	
	//Suma wszystkich pol
	IloNum Sumapol=0;
	for(int i=0;i<ILOSC_KLOCKOW;i++)
	{
		Sumapol=Sumapol+wszystkieKlocki[i].pole;
	}

	IloIntVar x_max(env, a, b-a);//uzyte tylko w drugim wariancie
	IloIntVar y_max(env, a, c-a);//uzyte tylko w drugim wariancie

	unsigned int Wariant = 1;
	if(Sumapol < 0.75 * b*c)
	{
		Wariant =2;
	}

	//ograniczenia
	for(int i = 0; i<iIlosc; i++) //i to k
	{ 
		model.add(wszystkieKlocki[i].x1 >= a);                          //4.2
		model.add(wszystkieKlocki[i].x2 <= b-a);                        //4.3
		model.add(wszystkieKlocki[i].y1 >= a);							//4.4
		model.add(wszystkieKlocki[i].y2 <= c-a);						//4.5
			
		model.add(wszystkieKlocki[i].x1 + wszystkieKlocki[i].w + (wszystkieKlocki[i].h - wszystkieKlocki[i].w)*wszystkieKlocki[i].o - wszystkieKlocki[i].x2 == 0);//4.7 4.8
		model.add(wszystkieKlocki[i].y1 + wszystkieKlocki[i].h + (wszystkieKlocki[i].w - wszystkieKlocki[i].h)*wszystkieKlocki[i].o - wszystkieKlocki[i].y2 == 0); //4.9 4.10
		

		if(1 == Wariant)
		{
		   for(int j = i+1; j<iIlosc ; j++) //j to f
		   {
		   		model.add(wszystkieKlocki[i].d1+wszystkieKlocki[i].d2+wszystkieKlocki[i].d3+wszystkieKlocki[i].d4<=3);			//4.6
		   		model.add(b-b*wszystkieKlocki[i].p + b*wszystkieKlocki[i].d1 + wszystkieKlocki[j].x1 - wszystkieKlocki[i].x2 -a >=0);		//4.11
		   		model.add(b-b*wszystkieKlocki[i].p + b*wszystkieKlocki[i].d2 + wszystkieKlocki[i].x1 - wszystkieKlocki[j].x2 -a >=0);		//4.12
		   		model.add(c-c*wszystkieKlocki[i].p + c*wszystkieKlocki[i].d3 + wszystkieKlocki[j].y1 - wszystkieKlocki[i].y2 -a >=0);		//4.13
				model.add(c-c*wszystkieKlocki[i].p + c*wszystkieKlocki[i].d4 + wszystkieKlocki[i].y1 - wszystkieKlocki[j].y2 -a >=0);		//4.14
		   }
		}
		else
		{
			for(int j = i+1; j<iIlosc ; j++) //j to f
			{
				model.add(wszystkieKlocki[i].d1+wszystkieKlocki[i].d2+wszystkieKlocki[i].d3+wszystkieKlocki[i].d4<=3);		//4.6
		   		model.add(b*wszystkieKlocki[i].d1 + wszystkieKlocki[j].x1 - wszystkieKlocki[i].x2 -a >=0);		//4.11
				model.add(b*wszystkieKlocki[i].d2 + wszystkieKlocki[i].x1 - wszystkieKlocki[j].x2 -a >=0);		//4.12
				model.add(c*wszystkieKlocki[i].d3 + wszystkieKlocki[j].y1 - wszystkieKlocki[i].y2 -a >=0);		//4.13
		   		model.add(c*wszystkieKlocki[i].d4 + wszystkieKlocki[i].y1 - wszystkieKlocki[j].y2 -a >=0);		//4.14
			}
			model.add(wszystkieKlocki[i].x2 <= x_max);
			model.add(wszystkieKlocki[i].y2 <= y_max);
		}		
	}
	////ostatni warunek - suma
	//if(1==Wariant)
	//{
	//	model.add(wszystkieKlocki[0].pole*wszystkieKlocki[0].p+
	//		      wszystkieKlocki[1].pole*wszystkieKlocki[1].p+
	//		      wszystkieKlocki[2].pole*wszystkieKlocki[2].p+
	//		      wszystkieKlocki[3].pole*wszystkieKlocki[3].p+
	//	          wszystkieKlocki[4].pole*wszystkieKlocki[4].p+
	//		      wszystkieKlocki[5].pole*wszystkieKlocki[5].p+
	//		      wszystkieKlocki[6].pole*wszystkieKlocki[6].p+
	//		      wszystkieKlocki[7].pole*wszystkieKlocki[7].p+
	//		      wszystkieKlocki[8].pole*wszystkieKlocki[8].p+
	//		      wszystkieKlocki[9].pole*wszystkieKlocki[9].p+
	//	          wszystkieKlocki[10].pole*wszystkieKlocki[10].p+
	//		      wszystkieKlocki[11].pole*wszystkieKlocki[11].p+
	//		      wszystkieKlocki[12].pole*wszystkieKlocki[12].p+
	//		      wszystkieKlocki[13].pole*wszystkieKlocki[13].p+
	//		      wszystkieKlocki[14].pole*wszystkieKlocki[14].p+
	//		      wszystkieKlocki[15].pole*wszystkieKlocki[15].p+
	//		      wszystkieKlocki[16].pole*wszystkieKlocki[16].p+
	//		      wszystkieKlocki[18].pole*wszystkieKlocki[18].p+
	//		      wszystkieKlocki[19].pole*wszystkieKlocki[19].p+
	//		      wszystkieKlocki[20].pole*wszystkieKlocki[20].p+
	//		      wszystkieKlocki[21].pole*wszystkieKlocki[21].p+
	//		      wszystkieKlocki[22].pole*wszystkieKlocki[22].p+
	//	          wszystkieKlocki[23].pole*wszystkieKlocki[23].p+
	//		      wszystkieKlocki[24].pole*wszystkieKlocki[24].p+
	//		      wszystkieKlocki[25].pole*wszystkieKlocki[25].p+
	//		      wszystkieKlocki[26].pole*wszystkieKlocki[26].p <= (b-a)*(c-a));     //4.15 ale ten warunek zawiera sie w 4.3 i 4.5   
	//}
 

	//dodanie do modelu celu - maksymalizacja tego równania
	if(1 == Wariant)
	{
		model.add(IloMaximize(env ,IloScalProd(iPola,iTabWybranych)));
	/*model.add(IloMaximize(env, wszystkieKlocki[0].pole*wszystkieKlocki[0].p+
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
					           wszystkieKlocki[17].pole*wszystkieKlocki[17].p+
							   wszystkieKlocki[18].pole*wszystkieKlocki[18].p+
							   wszystkieKlocki[19].pole*wszystkieKlocki[19].p+
							   wszystkieKlocki[20].pole*wszystkieKlocki[20].p+
							   wszystkieKlocki[21].pole*wszystkieKlocki[21].p+
							   wszystkieKlocki[22].pole*wszystkieKlocki[22].p+
							   wszystkieKlocki[23].pole*wszystkieKlocki[23].p+
							   wszystkieKlocki[24].pole*wszystkieKlocki[24].p+
							   wszystkieKlocki[25].pole*wszystkieKlocki[25].p+
							   wszystkieKlocki[26].pole*wszystkieKlocki[26].p));*/
	}
	else
	{
		model.add(IloMinimize(env, x_max + y_max));
	}
	


    // Optimize
    IloCplex cplex(model);
   // cplex.setOut(env.getNullStream());
    //cplex.setWarning(env.getNullStream());
    cplex.solve();
	cout << "-----------------------------------------------------------"<<endl;
	cout << "status"<< cplex.getStatus()<<endl;
	cout << "-----------------------------------------------------------"<<endl;
	cout  << "Suma pol wszystkich klockow = "<< Sumapol << endl;
	cout  << "Pole palety = "<< c*b << endl;
	cout << "-----------------------------------------------------------"<<endl;
	cout  << "Max po optymailizacji i ograniczeniach =" << cplex.getObjValue () << endl;
	cout << "------------------------------------------------------------------------"<<endl;
	for(int i = 0; i<ILOSC_KLOCKOW;i++)
	{
		if(1 == Wariant)
		{
			cout<< "p["<<i<<"]="<<cplex.getValue(wszystkieKlocki[i].p)<<endl;
		}
		cout<< "x1["<<i<<"]="<<cplex.getValue(wszystkieKlocki[i].x1)<<endl;
		cout<< "x2["<<i<<"]="<<cplex.getValue(wszystkieKlocki[i].x2)<<endl;
		cout<< "y1["<<i<<"]="<<cplex.getValue(wszystkieKlocki[i].y1)<<endl;
		cout<< "y2["<<i<<"]="<<cplex.getValue(wszystkieKlocki[i].y2)<<endl;

		if(i<ILOSC_KLOCKOW-1)//poniewaz d nie sa dodawane dla ostatniego klocka
		{
		   cout<< "d1["<<i<<"]="<<cplex.getValue(wszystkieKlocki[i].d1)<<endl;
		   cout<< "d2["<<i<<"]="<<cplex.getValue(wszystkieKlocki[i].d2)<<endl;
		   cout<< "d3["<<i<<"]="<<cplex.getValue(wszystkieKlocki[i].d3)<<endl;
		   cout<< "d4["<<i<<"]="<<cplex.getValue(wszystkieKlocki[i].d4)<<endl;
		}

//	cplex.getValues( (const IloIntVarArray)iTabWybranych);
	}
	cout << "-----------------------------------------------------------"<<endl;

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



