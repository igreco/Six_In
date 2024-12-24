# Six_In

Tensione di uscita controllata da un massimo di sei ingressi digitali.

Licenza:
Questo progetto è concesso secondo i termini della Licenza MIT, per i dettagli, vedere il file LICENSE.

Licenze di terze parti:
Questo progetto utilizza diverse librerie di terze parti, 
ognuna delle quali è soggetta alle proprie licenze. 
Per i dettagli, vedere il file THIRD_PARTY_LICENSES.md.

---

Six_In è un programma per Atemega328P, che genera una tensione di uscita di controllo corrispondente a la quantità degli ingressi digitali attivi.
Questa è una versione minimizzata di Six_In_Out (vedere progetto Six_In_Out), mentre quest’ultima usa un DAC per la uscita di tensione, Six_In adopera PWM.

Il test sarà fatto con un kit Arduino Uno Rev.3, un display SSD1306 tipo OLED, 2 pulsanti (N.A.), un filtro passa basso attivo fatto con condensatori e resistenze varie e un amplificatore operazionale, cavi di interconnessione, ecc. (vedere circuito Six_In.pdf)(vedere NOTA BENE).

Il programma permette di settare la quantità degli ingressi digitali di controllo: da uno (minimo) a sei (massimo), chiamati Control_In.

I Control_In sono normali a livello alto e attivi a livello basso.

Nel caso di tutti gli ingressi a livello alto, 
la tensione in uscita sarà minima, ovvero 0,0 Volt (GND).

Nel caso di tutti i Control_In a livello basso, 
la tensione in uscita sarà massima, ovvero 5 Volt (VCC).

Una volta programmato, il micro parte con un settaggio di sei ingressi di controllo (sei Control_In), che è il modo per default, dopo vedremmo come vanno cambiati.

La seguente tabella in basso mostra come è generata la tensione in uscita in riferimento a la quantità di Control_In attivi.

| Control_In | Tensione di uscita nominale generata |
|:-----------------:|-----------------------------|
| NESSUNO            | 0,0 Volt (GND).             |
| UNO                 | 1/6 VCC                     |
| DUE                 | 1/3 VCC (2/6).              |
| TRE                 | 1/2 VCC (3/6).              |
| QUATRO                 | 2/3 VCC (4/6).              |
| CINQUE                 | 5/6 VCC.                    |
| SEI                 | VCC (5 Volt).            |

La tensione di uscita generata può essere variata in più o in meno 
tramite due pulsanti identificati come UP e DOWN, una sorta di trimmer digitale di regolazione.

Il display mostra lo stato in cui si trova l'Atemega328P, che può essere:
NORMAL MODE o SETTING MODE.

Nello stato NORMAL MODE, il display mostra l'uscita in Volt della tensione di controllo (valore misurato tramite l'ADC dell'Atemega328P) e il valore assoluto del PWM. 
I pulsanti non generano alcuna azione apparente.

Per passare allo stato SETTING MODE è necessario tenere premuto il pulsante DOWN per due secondi.
Nello stato SETTING MODE, i pulsanti svolgono le loro funzioni, 
potendo regolare verso l'alto o verso il basso la tensione di controllo dell'uscita e visualizzarla sul display SSD1306. 
Mettendo a massa ogni Control_In necessari si vedrà sul display la tensione nominale generata e si potrà agire come detto prima, andando a scatti ogni volta che si pigia i pulsanti.

Una volta completate le correzioni, premendo nuovamente il pulsante DOWN per due secondi, l' Atemega328P uscirà dallo stato SETTING MODE e si porterà a lo stato NORMAL MODE, facendo sì che tutte le impostazioni effettuate vengano memorizzate nella memoria EEPROM, in questo modo le impostazioni rimarranno permanenti anche se VCC è disconnesso.

Da far notare che nello stato SETTING MODE, si dalla ultima volta che si ha pigiato su un pulsante passa 60 secondi senza aver fatto il passaggio a lo stato NORMAL MODE tenendo premuto nuovamente il pulsante DOWN per due secondi, il ESP32 torna in automatico a lo stato NORMAL MODE facendo sì che tutte le impostazioni effettuate fino questo instante vengano memorizzate nella memoria non volatile (EEPROM), in questo modo le impostazioni fatte non si perdono e si potrà riprendere quelle mancanti in futuro.

Se invece dallo stato NORMAL MODE si preme per due secondi, questa volta però, il tasto UP, 
il display cambia e mostrerà ora una riga con i valori da 1 a 6 e sotto di essi 
un cursore che si sposterà con i due tasti (con DOWN verso sinistra e UP verso la destra del display), posizionando il cursore sotto la quantità che si vuole di Control_In ed uscendo dallo stato SETTING MODE con il tasto DOWN, tenendolo premuto per due secondi, le impostazioni della quantità dei Control_In cambierà a la scelta fatta e i valori della tensione di uscita sarà come indicati nella prossima tabella.
Nuovamente se, sin dalla ultima volta che si ha pigiato su un pulsante passa 60 secondi senza aver fatto il passaggio a lo stato NORMAL MODE tenendo premuto nuovamente il pulsante DOWN per due secondi, il Atemega328P torna in automatico a lo stato NORMAL MODE, questa volta pero non cambierà la scelta della quantità di Control_In, sarà compito del utente a farlo solo pigiando per due secondi il pulsante DOWN, il resto rimane invariato. Cosi è possibile scegliere il numero di input necessari da 1 a 6. E da far notare che si la scelta e fatta correttamente, esempio: se prima era impostato a sei Control_In e ora e impostato a tre Control_In tenendo premuto per due secondi il tasto DOWN, per far che il micro prenda le sue nuove impostazione si dovrà resettarlo o togliere l’alimentazione, aspettare un po e tornarlo ad accenderlo (fare questo in automatico sarà una miglioria da fare in futuro).

Gli ingressi non necessari verranno scartati dal circuito rimuovendo i ponticelli su di essi (vedere circuito Six_In.pdf). Se invece si decide di restare i ponticelli il comportamento sarà sulla quantità di Control_In attivi sul totale, per controllare la tensione di uscita. 
Ad esempio: scegliendo 3 Control_In come quantità e mantenendo tutti 6 ponticelli, qualunque de i sei ingressi sarà visto per il programma come attivo e facendo si che il valore di tensione varie accordo a la massima quantità di tre, finché non scenda a due o uno la uscita si fermerà al valore di tensione corrispondente a tre. 

I valori della tensione di uscita nominali saranno secondo la seguente tabella:

| **Control_In Attivi -->** | **_NESSUNO_** | **_UNO_** | **_DUE_** | **_TRE_** | **_QUATTRO_** | **_CINQUE_** | **_SEI_** |
|:-------------------:|:------------------:|:-------------:|:-------------:|:-------------:|:-------------:|:-------------:|:-------------:|
|    **x 6 input**    |      _0 Volt_      |   _1/6 VCC_   |   _1/3 VCC_   |   _1/2 VCC_   |   _2/3 VCC_   |   _5/6 VCC_   |     _VCC_     |
|    **x 5 input**    |      _0 Volt_      |   _1/5 VCC_   |   _2/5 VCC_   |   _3/5 VCC_   |   _4/5 VCC_   |     _VCC_     |     _VCC_     |
|    **x 4 input**    |      _0 Volt_      |   _1/4 VCC_   |   _1/2 VCC_   |   _3/4 VCC_   |     _VCC_     |     _VCC_     |     _VCC_     |
|    **x 3 input**    |      _0 Volt_      |   _1/3 VCC_   |   _2/3 VCC_   |     _VCC_     |     _VCC_     |     _VCC_     |     _VCC_     |
|    **x 2 input**    |      _0 Volt_      |   _1/2 VCC_   |     _VCC_     |     _VCC_     |     _VCC_     |     _VCC_     |     _VCC_     |
|    **x 1 input**    |      _0 Volt_      |     _VCC_     |     _VCC_     |     _VCC_     |     _VCC_     |     _VCC_     |     _VCC_     |

Cioè, le tensioni di uscita vengono trattate in base al numero di Control_In di ingresso scelti e in base al numero di Control_In attivi.

Il numero Control_In in questione così come le loro impostazioni una volta memorizzati nella memoria non volatile rimangono permanenti finché non viene effettuata una nuova modifica con i due pulsanti e visualizzata sul display.

Il display e i due pulsanti possono essere un modulo separabile.
Una volta impostato l'Atmega328P è possibile scollegare il modulo, 
rendendo più economica la produzione in quanto con un solo modulo si possono impostare altre schede e non essendo lasciati i pulsanti su di esso non permette di variare facilmente i valori per l'utente.

# Funzionamento degli ingressi Control_In.

Ogni volta che un Control_In viene attivato, la tensione di uscita viene generata rapidamente.
Quando la transizione sui Control_In avviene da attivato a normale, l'uscita della tensione di controllo avverrà gradualmente attraverso una rampa scendente fino a 3 secondi, 
producendo così un'uscita di tensione di controllo di tipo Soft Start.

# Riferimento incrociato tra Atmega328P e Arduino Uno Rev.3.

| **Ingressi/Uscite** | **PORT Atmega328P** | **Pin Atmega328P** | **Pin Arduino Uno** |
|:-------------------:|:-------------------:|:------------------:|:-------------------:|
|    **_INPUT_1_**    |         PB0         |         14         |          8          |
|    **_INPUT_1_**    |         PB1         |         15         |          9          |
|    **_INPUT_3_**    |         PB2         |         16         |          10         |
|    **_INPUT_4_**    |         PB3         |         17         |          11         |
|    **_INPUT_5_**    |         PB4         |         18         |          12         |
|    **_INPUT_6_**    |         PB5         |         19         |          13         |
|     **_IN_ADC_**    |         PC0         |         23         |          A0         |
|      **_SDA_**      |         PC4         |         27         |          A4         |
|      **_SCL_**      |         PC5         |         28         |          A5         |
|      **/RESET**     |      /RESET-PC6     |          1         |        /RESET       |
|    **_KEY_DOWN_**   |         PD2         |          4         |          3          |
|     **_KEY_UP_**    |         PD3         |          5         |          2          |
|      **_PWM_**      |         PD5         |         11         |          5 |


# NOTA BENE:

Il firmware e stato collaudato su un Arduino Uno con un semplice filtro passa basso RC, andando come previsto. Il PWM generato è da 980Hz approssimativamente.

La idea finale è fatta su un circuito dove e inserito un singolo Atmega328P funzionando senza Xtal con oscillatore interno di 8MHz e un filtro  passa basso attivo con un OPAMP per il PWM.

La programmazione de l’Atmega328P e fatta con il Arduino Uno (come programmatore) via ICSP per ciò si adopera il Pin /RESET de l’Arduino (vedere circuito Six_In.pdf).

Anche quest’ultimo e stato collaudato e funziona tutto bene, Il PWM generato con il oscillatore interno a 8MHz pero si e dimezzato a 440Hz (volendo mantenere la stesa frequenza che con il xtal da 16MHz esterno a 980Hz, sarà una miglioria da fare in futuro).

In riferimento a la uscita di tensione di controllo, se la corrente necessaria e intorno a i 10mA e sufficiente usare una singola alimentazione da 5V e un solo CI LM7332 con due amplificatori interni se invece e necessario 100mA e meglio fare con un buffer come nel circuito Six_In.pdf.