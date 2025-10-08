# STANOWISKO_TESTOWE_1

# BAZOWANIE:
-po włączeniu zasilania karetka może sygnalizować dwa stany:
---brak wykrycia przez czujnik,
---czujnik wykrył położenie karetki,

PROCEDURA BAZOWANIA:
-brak wykrycia: spoglądamy do urządzenia i sprawdzamy, w którym położeniu znajduje się karetka,
-jeśli znaduje się pomiędzy czujnikami to wykonujemy dowolny sposób bazowania w sensie pierwszy dojazd do wybranego czujnika,
-

# BRAK WYKRYCIA PRZEZ CZUJNIK:
-karetka znajduje się w polu pracy czyli pomiędzy czujnikami,
-karetka znajduje się poza polem pracy: może znajdować się poza polem pracy w pobliżu czujnika X1 albo czujnika X2,

## CZUJNIK WYKRYŁ POŁOŻENIE KARETKI:
-karetka znajduje się w polu wykrywania przez czujnik X1 lub X2: detekcja za pomocą odczytu stanu na linii,
-odczyt stanu na linii pozwala na wykrycie położenia karetki oraz determinuje w którą stronę powinien się zacząć poruszać wózek tak aby zakończyć bazowanie,
-jeśli jest na X1 to ruch od silnika do momentu odczytu sygnału o końcu wykrywania przez X1, zapisanie pozycji domowej X1,
przejazd do momentu wykrycia przez X2. Po wykryciu przez X2 od pozycji odejmujemy wartość HOMING i wracamy na pozycję X1_HOMING.
