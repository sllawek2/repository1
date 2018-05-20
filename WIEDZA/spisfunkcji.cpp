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


Panowie,

odnośnie przedmiotów "laboratorium dyplomowe" i "seminarium dyplomowe", które są powiązane z przygotowaniem pracy dyplomowej i z których na koniec wystawię oceny, proponuję następujący tryb działań:

1. Podstawowym zadaniem jest przygotowanie pracy i postępy w tym zakresie będą przedmiotem oceny.

2. Trzeba starać się pracować systematycznie. W związku z tym, oczekuję najpóźniej w ostatnim dniu każdego miesiąca (od marca do maja, a w czerwcu przed rozpoczęciem sesji) dokumentacji prac wykonanych w danym miesiącu. Powinien być to dokument opisujący postępy pracy (forma sprawozdania) oraz materiały powiązane (pliki projektów, kody źródłowe itd.). Dokumentację proszę wprowadzać do systemu sprawozdań (http://sprawozdania.kia.prz.edu.pl/) korzystając z grupy "dyplomanci" przypisanej do mojego nazwiska - dla każdego miesiąca jest tam oddzielna rubryka.

3. Na koniec semestru dodatkowo każdy powinien osobiście przedstawić całościowe rezultaty.

4. Ocenę końcową wyznaczę na podstawie dokumentacji z poszczególnych miesięcy i końcowej prezentacji.

5. Nie wymagam bezpośredniej obecności w trakcie semestru w regularnych konkretnych terminach. Każdy powinien się zgłaszać w celu konsultacji stosownie do potrzeb. Dla dyplomantów jestem dyspozycyjny zarówno w godzinach konsultacji ogólnych (wtorek 10:00-11:30, środa 12:30-14:00), jak i przeznaczonych oficjalnie na sem./lab. dyplomowe (wtorek 11:30-12:30 tydzień B, środa 14:00-15:00 tydzień A) - rozróżnienie tych form dyżuru wynika z wymogów formalnych, ale w praktyce dla nas nie ma znaczenia. Proszę zawsze o wcześniejsze uzgodnienie terminu via e-mail.

6. W przypadku studentów stacjonarnych, realizacja pracy będzie częściowo powiązana z projektem wykonywanym na przedmiocie "mechatronika ...". W trakcie tych zajęć będę mógł również odpowiadać na bieżące pytania. Niezależnie od tego, co do zasady wszystkie przedmioty podlegają indywidualnemu zaliczeniu, tzn. z "mechatroniki ..." trzeba przygotować dokumentację i prezentację zgodnie z zasadami podanymi dla tego przedmiotu, a z sem./lab. dyplomowego spełnić warunki podane wyżej.

Pozdrawiam





TODO:
1. obczaić "node log"
2. przetestować oba warianty - zobaczyć czy klocki są dobrze ustawione i czy to ma sens
3. uporządkować kod
4. emulator - ma losować klocki i zapisywać do pliku
5. menu programu
6. zrobić drugą werjsę na środkach zamiast na polach?



///
1. Gdzie zapisać wynik - na razie jest wyświetlany
2. Co ma być w wyniku, w jakim formacie ma być wynik
3. Jaki ma być limit czasowy
4. Generator danych czy ma być w tym samym programie czy osobno?

1. W pierwszym wariancie musiałem dodać limit czasowy bo bardzo długo szuka rozwiązania, nawet kilka godzin
2. W drugim wariancie warunek celu z minimalizacją x_max + y_max nie spełnia zadania
3. Jeszcze musze to też lepiej potestować bo nie wiem czy dobrze działa





