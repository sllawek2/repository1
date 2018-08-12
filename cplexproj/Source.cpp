
#include <ilcp/cp.h>

ILOSTLBEGIN

#include <fstream>
#include <string>
#include <vector>
#include <iostream>

	
const unsigned int ILOSC_KLOCKOW = 3;

const int wymiarX = 15, wymiarY = 15;

class Klocek{
public:
  Klocek(const IloEnv &env,unsigned int w, unsigned int h):
	  w(w),o(env,0,1),h(h),tablica(env)
  {
    for(int x =0; x<wymiarX; x++)
    {
      tablica.add(IloIntVarArray(env));
      for(int y=0;y<wymiarY;y++)
      {
        tablica[x].add(IloIntVar(env,0,1));
      }
    }
  }

  IloInt w;//szeroko�� produktu 
  IloInt h;//wysoko�� produktu 
  IloBoolVar o; //orientacja o {0,1} bez obrotu, obr�t
  IloArray<IloIntVarArray> tablica;
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
      if(a>wymiarX || a<1 || b>wymiarY || b<1)
      {
        cout<<"bledne dane!!!!"<<endl;
        return false;
      }

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

void rysowanieWPliku(const IloCP &cplex, const vector<Klocek> & wszystkieKlocki)
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
        if(cplex.getValue(wszystkieKlocki[i].tablica[x][y])==1)
        {
          tablica[x][y]= znaki[i];
        }
      }
    }
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

      IloIntVar x_max(env, 0, wymiarX);
      IloIntVar y_max(env, 0, wymiarY);
         
      //ograniczenia
      for(int x = 0; x<wymiarX; x++)
      {
        for(int y=0;y<wymiarY;y++)
        {
          for(int i = 0;i <ILOSC_KLOCKOW; i++)
          {
            for(int j = i+1 ; j<ILOSC_KLOCKOW; j++)
            {
              model.add(wszystkieKlocki[i].tablica[x][y]+wszystkieKlocki[j].tablica[x][y] <=1 );//warunek �eby klocki na siebie nie nachodzi�y
            }
          }
        }
      }

      for(int i=0;i<ILOSC_KLOCKOW;i++)
      {
        IloIntExpr sumaCalosci(env);
        for(int x =0;x<wymiarX;x++)
        {
          IloIntExpr suma1(env);
          for(int y =0; y<wymiarY;y++)
          {
            suma1+=wszystkieKlocki[i].tablica[x][y];//suma w kolumnie
            sumaCalosci+=wszystkieKlocki[i].tablica[x][y];//suma calosci
          }
          model.add(suma1==0||suma1 ==wszystkieKlocki[i].h +wszystkieKlocki[i].w*wszystkieKlocki[i].o - wszystkieKlocki[i].h*wszystkieKlocki[i].o);// ilosc p�l r�wna si� wysoko�� klocka lub zero
        }
        model.add(sumaCalosci==wszystkieKlocki[i].w*wszystkieKlocki[i].h);//warunek �eby ka�dy klocek zosta� dok�adnie raz u�yty

        for(int y =0; y<wymiarY;y++)
        {
          IloIntExpr suma2(env);
          for(int x =0;x<wymiarX;x++)
          {
            suma2+=wszystkieKlocki[i].tablica[x][y];     // suma w wierszu
          }
          model.add(suma2==0||suma2 ==  wszystkieKlocki[i].w +wszystkieKlocki[i].h*wszystkieKlocki[i].o - wszystkieKlocki[i].w*wszystkieKlocki[i].o);//ilo�� p�l r�wna si� szeroko�� klocka lub zero
        }
         //warunki �eby nie by�y rozstrzelone
        for(int x=0;x<wymiarX;x++)
        {
          for(int y=0;y<wymiarY;y++)
          {
            for(int k = x+wszystkieKlocki[i].w; k<wymiarX; k++)
            {
              model.add(wszystkieKlocki[i].tablica[x][y]*wszystkieKlocki[i].tablica[k][y]*(1-wszystkieKlocki[i].o) == 0);
            }
            for(int k = x+wszystkieKlocki[i].h; k<wymiarX; k++)
            {
              model.add(wszystkieKlocki[i].tablica[x][y]*wszystkieKlocki[i].tablica[k][y]*wszystkieKlocki[i].o == 0);
            }

            for(int k = y+wszystkieKlocki[i].h; k<wymiarY; k++)
            {
              model.add(wszystkieKlocki[i].tablica[x][y]*wszystkieKlocki[i].tablica[x][k]*(1- wszystkieKlocki[i].o) == 0);
            } 
            for(int k = y+wszystkieKlocki[i].w; k<wymiarY; k++)
            {
              model.add(wszystkieKlocki[i].tablica[x][y]*wszystkieKlocki[i].tablica[x][k]*wszystkieKlocki[i].o == 0);
            } 
          }
        }
      }
      //dodanie do modelu celu
      for(int i = 0;i<ILOSC_KLOCKOW;i++)
      {
        for(int y=0;y<wymiarY;y++)
        {
          IloIntVarArray tabX(env);
          
          for(int x =0; x<wymiarX;x++)
          {
            tabX.add(wszystkieKlocki[i].tablica[x][y]);
          }
          tabX.add(IloIntVar(env,0,0));
 
          IloIntVar indexX(env, 0,wymiarX-1);
          model.add(((1==IloElement(tabX,indexX)) && (0==IloElement(tabX,indexX+1))) == (IloSum(tabX)>0));
          model.add(indexX<=x_max);
        }
          
        for(int x =0; x<wymiarX;x++)
        {
          IloIntVarArray tabY(env);
          for(int y=0;y<wymiarY;y++)
          {
            tabY.add(wszystkieKlocki[i].tablica[x][y]);
          }
          tabY.add(IloIntVar(env,0,0));

          IloIntVar indexY(env, 0,wymiarY-1);
          model.add(((1==IloElement(tabY,indexY)) && (0==IloElement(tabY,indexY+1))) == (IloSum(tabY)>0));
          model.add(indexY<=y_max);
        }

      }
      model.add(IloMinimize(env, x_max+ y_max ));
  
      IloCP cp(model);
      cp.setParameter(IloCP::NumParam::TimeLimit,60);

      if(cp.solve())
      {
        rysowanieWPliku(cp, wszystkieKlocki);
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