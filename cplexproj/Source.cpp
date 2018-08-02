
#include <ilcp/cp.h>

ILOSTLBEGIN

#include <fstream>
#include <string>
#include <vector>
#include <iostream>

	
const unsigned int ILOSC_KLOCKOW = 1;

const IloInt b = 180, c = 180;
const int jednostka = 30;
const int wymiarX=b/jednostka;
const int wymiarY=c/jednostka;

class Klocek{
public:
  Klocek(const IloEnv &env,unsigned int w, unsigned int h):
	  w(w),o(env,0,1),h(h),p(env,0,1)//, d1(env,0,1), d2(env,0,1), d3(env,0,1), d4(env,0,1)
  {
    for(int i =0; i<wymiarX; i++)
    {
      for(int j=0;j<wymiarY;j++)
      {
        tablica[i][j]=IloBoolVar(env,0,1);
      }
    }
  }

  IloInt w;//szeroko�� produktu [30,80]
  IloInt h;//wysoko�� produktu [30,50]
 // IloIntVar x1; //bli�sza kraw�d� na osi X
  //IloIntVar x2; //dalsza kraw�d� na osi X
 // IloIntVar y1;
  //IloIntVar y2;
  //IloIntVar x; //po�o�enie �rodka produktu
  //IloIntVar y;
  //gap = 3 //odst�p pomiedzy produktami
  //gap frame = 10 //przerwa pomi�dzy palet� a produktami
  //box limit 180x180 //wymiary palety
  //IloNum pole; //pole a*b
  IloBoolVar o; //orientacja o {0,1} bez obrotu, obr�t
  IloBoolVar p;// 0 nie wybrany, 1 wybrany
  
  //IloBoolVar d1;
  //IloBoolVar d2;
  //IloBoolVar d3;
  //IloBoolVar d4;
  IloIntVar tablica[wymiarX][wymiarY];
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
      //if(a>80 || a<30 || b>50 || b<30)
      //{
      //  cout<<"bledne dane!!!!"<<endl;
      //  return false;
      //}

      a= a/jednostka;
      b = b/jednostka;

      Klocek k(env,a,b);
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

void rysowanieWPliku(const IloCP &cplex, const bool bPierwszyWariant, const vector<Klocek> & wszystkieKlocki)
{
  char tablica[wymiarX][wymiarY];
  char znaki[] ={'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','0','1','2','3','4','5','6','7','8','9'};

  memset(tablica,'.', wymiarX*wymiarY);
  for(int i = 0; i<ILOSC_KLOCKOW; i++)
  {
    for(int x=0;x<wymiarX;x++)
    {
      for(int y=0;y<wymiarY;y++)
      {
        cout<<cplex.getValue(wszystkieKlocki[i].tablica[x][y]);
        if(cplex.getValue(wszystkieKlocki[i].tablica[x][y])==1)
        {
          tablica[x][y]= znaki[i];
        }
      }
      cout<<endl;
    }
    cout<<endl;
  }


  fstream outputFile;
  outputFile.open("wynik.txt",'w');
  if(outputFile.good())
  {
    for(int y=wymiarY -1;y>=0;y--)
    {
      for(int x =0; x<wymiarX;x++)
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
      IloNum Sumapol=0;//suma wszystkich pol
      IloIntExpr poleMax(env);
      for(int i = 0; i<ILOSC_KLOCKOW; i++)
      {
        for(int x=0; x<wymiarX;x++)
        {
          for(int y=0;y<wymiarY;y++)
          {
            poleMax+=wszystkieKlocki[i].tablica[x][y];
          }
        }
    /*    iPola.add(wszystkieKlocki[i].pole);
        iTabWybranych.add(wszystkieKlocki[i].p);
        Sumapol=Sumapol+wszystkieKlocki[i].pole;*/
      }
    
      //IloIntVar x_max(env, a, b-a);//uzyte tylko w drugim wariancie
      //IloIntVar y_max(env, a, c-a);//uzyte tylko w drugim wariancie
      
      bool bPierwszyWariant = true;
   /*   if(Sumapol < 0.75 * wymiarX*wymiarY)
      {
        bPierwszyWariant = false;
      }*/
    
      //ograniczenia
      for(int x = 0; x<wymiarX; x++)
      {
        for(int y=0;y<wymiarY;y++)
        {
          IloIntExpr suma(env);

          for(int i = 0;i <ILOSC_KLOCKOW; i++)
          {
            for(int j = i+1 ; j<ILOSC_KLOCKOW; j++)
            {
              model.add(wszystkieKlocki[i].tablica[x][y]+wszystkieKlocki[j].tablica[x][y] <=1 );//warunek �eby klocki na siebie nie nachodzi�y
            }
          }
        }
      }

      for(int i=0;i<ILOSC_KLOCKOW;i++)//suma w wierszu
      {
        for(int x =0;x<wymiarX;x++)
        {
          IloIntExpr suma1(env);
          for(int y =0; y<wymiarY;y++)
          {
            suma1+=wszystkieKlocki[i].tablica[x][y];
          }
          model.add(suma1==0||suma1 == wszystkieKlocki[i].w /*+wszystkieKlocki[k].h*wszystkieKlocki[k].o - wszystkieKlocki[k].w*wszystkieKlocki[k].o*/);
        }
     // suma w kolumnie (mo�liwe �e odwrotnie trzeba wysoko�� i szeroko�� da�)
        for(int y =0; y<wymiarY;y++)
        {
          IloIntExpr suma2(env);
          for(int x =0;x<wymiarX;x++)
          {
            suma2+=wszystkieKlocki[i].tablica[x][y];
          }
          model.add(suma2==0||suma2 == wszystkieKlocki[i].h /*+wszystkieKlocki[k].w*wszystkieKlocki[k].o - wszystkieKlocki[k].h*wszystkieKlocki[k].o*/);//tu chyba ma by� odwrotnie
        }
      }

      for(int k=0;k<ILOSC_KLOCKOW;k++)
      {
        
        for(int i=1;i<wymiarX-1;i++)
        {
          for(int j=1;j<wymiarY-1;j++)
          {
            if(wszystkieKlocki[k].w>1 && wszystkieKlocki[k].h>1)
            {
            model.add((wszystkieKlocki[k].tablica[i][j]+wszystkieKlocki[k].tablica[i][j+1]==2*wszystkieKlocki[k].tablica[i][j] ||
                       wszystkieKlocki[k].tablica[i][j]+wszystkieKlocki[k].tablica[i][j+1]==2*wszystkieKlocki[k].tablica[i][j]));
           model.add((wszystkieKlocki[k].tablica[i][j]+wszystkieKlocki[k].tablica[i+1][j]==2*wszystkieKlocki[k].tablica[i][j] ||
                       wszystkieKlocki[k].tablica[i][j]+wszystkieKlocki[k].tablica[i-1][j]==2*wszystkieKlocki[k].tablica[i][j]));
            }


          }
        }
      }


     /*       for(int l = i+wszystkieKlocki[k].w; l<jednostka; l++)
            {
              wszystkieKlocki[k].tablica[i][j]+*/
            //wszystkieKlocki[k].o;

            //alternatywny spos�b
           /* IloIntExpr bezObrotuX(env);


            IloIntExpr bezObrotuY=wszystkieKlocki[k].tablica[i][0];
            IloIntExpr zObrotemX=wszystkieKlocki[k].tablica[i][0];
            IloIntExpr zObrotemY=wszystkieKlocki[k].tablica[0][j];
            suma += wszystkieKlocki[k].tablica[i][j];
            for(int l =1;l<wszystkieKlocki[k].w, l<wymiarX;l++)
            {
              bezObrotuX+=wszystkieKlocki[k].tablica[l][j];
              zObrotemY+=wszystkieKlocki[k].tablica[l][j];
            }
            for(int l = 1 ; l<wszystkieKlocki[k].h, l<wymiarY;l++)
            {
              bezObrotuY+=wszystkieKlocki[k].tablica[i][l];
              zObrotemX+=wszystkieKlocki[k].tablica[i][l];
            }

            model.add((bezObrotuX == wszystkieKlocki[k].w && bezObrotuY == wszystkieKlocki[k].h) ||
                      (zObrotemX == wszystkieKlocki[k].h && zObrotemY == wszystkieKlocki[k].w));
          }
          model.add(suma <= 1);
        }
      }
*/

//dla ka�dej tablicy suma wierszy/kolumn = w lub h lub 0











      //for(int i = 0; i<ILOSC_KLOCKOW; i++) //i to k 
      //{
      //  //dodajWspolneOgraniczenia(model, wszystkieKlocki[i]);
      //  wszystkieKlocki[i].x1+
      //  if(true == bPierwszyWariant)
      //  {
      //    for(int j = i+1; j<ILOSC_KLOCKOW; j++) //j to f
      //    {
      //      dodajOgraniczeniaPierwszegoWariantu(model, wszystkieKlocki[i], wszystkieKlocki[j]);
      //    }
      //  }
      //  else
      //  {
      //    for(int j = i+1; j<ILOSC_KLOCKOW ; j++) //j to f
      //    {
      //      dodajOgraniczeniaDrugiegoWariantu(model, wszystkieKlocki[i], wszystkieKlocki[j]);
      //    }
      //    model.add(wszystkieKlocki[i].x2 <= x_max);
      //    model.add(wszystkieKlocki[i].y2 <= y_max);
      //  }
      //}
    
      //dodanie do modelu celu
      if(true == bPierwszyWariant)
      {
        model.add(IloMaximize(env ,poleMax));
      }
      else
      {
//        model.add(IloMinimize(env, x_max + y_max));
      }
    
      IloCP cp(model);
     // cp.setParam(IloCP::TiLim, 60);//limit czasowy
     // cplex.setOut(env.getNullStream());
      //cplex.setWarning(env.getNullStream());
      if(cp.solve())
      {
    
    
      cout<<"Suma pol wszystkich klockow = "<<Sumapol<<", Pole palety = "<< c*b << endl;
 /*     wyswietlenieWynikow(cplex, bPierwszyWariant, wszystkieKlocki);*/
      rysowanieWPliku(cp,bPierwszyWariant, wszystkieKlocki);
      }
      else{
        cout<<"brak rozwi�zania"<<endl;
      }
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