
#include <ilcp/cp.h>

ILOSTLBEGIN

#include <fstream>
#include <string>
#include <vector>
#include <iostream>

	
const unsigned int ILOSC_KLOCKOW = 7;

const IloInt b = 50, c = 50;
const int jednostka = 10;
const int wymiarX=b/jednostka;
const int wymiarY=c/jednostka;

class Klocek{
public:
  Klocek(const IloEnv &env,unsigned int w, unsigned int h):
	  w(w),o(env,0,1),h(h)
  {
    for(int i =0; i<wymiarX; i++)
    {
      for(int j=0;j<wymiarY;j++)
      {
        tablica[i][j]=IloBoolVar(env,0,1);
      }
    }
  }

  IloInt w;//szerokoœæ produktu [30,80]
  IloInt h;//wysokoœæ produktu [30,50]
  IloBoolVar o; //orientacja o {0,1} bez obrotu, obrót
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
    
      //stworzenie tablicy pól wszystkich klocków
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
      }
    
      IloIntVar x_max(env, 0, wymiarX);//uzyte tylko w drugim wariancie
      IloIntVar y_max(env, 0, wymiarY);//uzyte tylko w drugim wariancie
      
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
              model.add(wszystkieKlocki[i].tablica[x][y]+wszystkieKlocki[j].tablica[x][y] <=1 );//warunek ¿eby klocki na siebie nie nachodzi³y
            }
          }
        }
      }

      for(int i=0;i<ILOSC_KLOCKOW;i++)
      {
        IloIntExpr sumaCalosci(env);
        for(int x =0;x<wymiarX;x++)//suma w wierszu
        {
          IloIntExpr suma1(env);
          for(int y =0; y<wymiarY;y++)
          {
            suma1+=wszystkieKlocki[i].tablica[x][y];
            sumaCalosci+=wszystkieKlocki[i].tablica[x][y];
          }
          model.add(suma1==0||suma1 == wszystkieKlocki[i].w +wszystkieKlocki[i].h*wszystkieKlocki[i].o - wszystkieKlocki[i].w*wszystkieKlocki[i].o);
        }
        model.add(sumaCalosci==wszystkieKlocki[i].w*wszystkieKlocki[i].h||sumaCalosci==0);
     // suma w kolumnie (mo¿liwe ¿e odwrotnie trzeba wysokoœæ i szerokoœæ daæ)
        for(int y =0; y<wymiarY;y++)
        {
          IloIntExpr suma2(env);
          for(int x =0;x<wymiarX;x++)
          {
            suma2+=wszystkieKlocki[i].tablica[x][y];
          }
          model.add(suma2==0||suma2 == wszystkieKlocki[i].h +wszystkieKlocki[i].w*wszystkieKlocki[i].o - wszystkieKlocki[i].h*wszystkieKlocki[i].o);
        }
 
        for(int x=0;x<wymiarX;x++)
        {
          for(int y=0;y<wymiarY;y++)
          {
            for(int k = x+wszystkieKlocki[i].h; k<wymiarX; k++)
            {
              model.add(wszystkieKlocki[i].tablica[x][y]*wszystkieKlocki[i].tablica[k][y]*(1-wszystkieKlocki[i].o) == 0);
            }
            for(int k = y+wszystkieKlocki[i].w; k<wymiarY; k++)
            {
              model.add(wszystkieKlocki[i].tablica[x][y]*wszystkieKlocki[i].tablica[x][k]*(1- wszystkieKlocki[i].o) == 0);
            } 
             
            for(int k = x+wszystkieKlocki[i].w; k<wymiarX; k++)
            {
              model.add(wszystkieKlocki[i].tablica[x][y]*wszystkieKlocki[i].tablica[k][y]*wszystkieKlocki[i].o == 0);
            }
            for(int k = y+wszystkieKlocki[i].h; k<wymiarY; k++)
            {
              model.add(wszystkieKlocki[i].tablica[x][y]*wszystkieKlocki[i].tablica[x][k]*wszystkieKlocki[i].o == 0);
            } 
          }
        }
      }
      //dodanie do modelu celu
      if(true == bPierwszyWariant)
      {
        model.add(IloMaximize(env ,poleMax));
      }
      else
      {
        model.add(IloMinimize(env, x_max + y_max));
      }
    
      IloCP cp(model);
      cp.setParameter(IloCP::NumParam::TimeLimit,60);
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
        cout<<"brak rozwi¹zania"<<endl;
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