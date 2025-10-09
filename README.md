x# STANOWISKO_TESTOWE_1

## BAZOWANIE:
-po włączeniu zasilania karetka może sygnalizować dwa stany:
---brak wykrycia przez czujnik,
---czujnik wykrył położenie karetki,

## PROCEDURA BAZOWANIA:
-brak wykrycia: spoglądamy do urządzenia i sprawdzamy, w którym położeniu znajduje się karetka,
-jeśli znaduje się pomiędzy czujnikami to wykonujemy dowolny sposób bazowania w sensie pierwszy dojazd do wybranego czujnika,

## BRAK WYKRYCIA PRZEZ CZUJNIK:
-karetka znajduje się w polu pracy czyli pomiędzy czujnikami,
-karetka znajduje się poza polem pracy: może znajdować się poza polem pracy w pobliżu czujnika X1 albo czujnika X2,

## CZUJNIK WYKRYŁ POŁOŻENIE KARETKI:
-karetka znajduje się w polu wykrywania przez czujnik X1 lub X2: detekcja za pomocą odczytu stanu na linii,

-odczyt stanu na linii pozwala na wykrycie położenia karetki oraz determinuje w którą stronę powinien się zacząć poruszać wózek tak aby zakończyć bazowanie,

-jeśli jest na X1 to ruch od silnika do momentu odczytu sygnału o końcu wykrywania przez X1, zapisanie pozycji domowej X1,
przejazd do momentu wykrycia przez X2. Po wykryciu przez X2 od pozycji odejmujemy wartość HOMING i wracamy na pozycję X1_HOMING.

## ZMIANA ILOŚCI KROKÓW NA OBRÓT:
-do dyspozycji mamy kilka ilości kroków na obrót: 200, 400, 800, 1600, 3200, 6400,
-po wejściu do trybu pracy należy sprawdzić czy ustawiona w sterowniku ilość kroków na obrót zgadza się z ilością ustawioną w programie,
-jeśli wartości nie będą zgodne to przeliczane dane nie będą prawidłowe,
-ewentualna zmiana wartości kroków na obrót może odbyć się w zmianie sprzętowej lub programowej,
-jeśli tryb pracy jest trybem MANUAL to po zmianie parametru programowo
-jeżeli jesteśmy w trybie AUTO to po zmianie paramertu na wartość wyższą, wszystkie parametry zostaną przeliczone,
-jeżeli jesteśmy w trybie AUTO to po zmianie parametru na wartość niższą i jeżeli położenie krokowe podzielone przez wartość zmniejszenia liczby kroków będzie dzielone z resztą to zmiana nie będzie możliwa,
-dzieję się tak dlatego, że jeżli byśmy zaakceptowali taką zmianę to nie można było by osiągnąć pewnych położeń
