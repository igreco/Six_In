
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

Gli ingressi digitali sono normali a livello alto e attivi a livello basso, considerati in FAULT.

Nel caso di tutti gli ingressi a livello alto, 
la tensione in uscita sarà minima, ovvero 0,0 Volt (GND).

Nel caso di tutti gli ingressi a livello basso (tutti gli ingressi in FAULT), 
la tensione in uscita sarà massima, ovvero 5.0 Volt (VCC).

In base al numero di voci in FAULT la tensione in uscita viene generata come segue: 

| **Ingressi in FAULT** | **Tensione di uscita generata** |
|:---------------------:|---------------------------------|
|        **_0_**        | 0,0 Volt (GND).                 |
|        **_1_**        | 1/6 VCC                         |
|        **_2_**        | 2/6 VCC (1/3).                  |
|        **_3_**        | 3/6 VCC (1/2).                  |
|        **_4_**        | 4/6 VCC (2/3).                  |
|        **_5_**        | 5/6 VCC.                        |
|        **_6_**        | 5,0 Volts (VCC).                |

I valori di tensione generati possono essere variati in più o in meno 
tramite due pulsanti identificati come UP e DOWN (una sorta di trimmer digitale di regolazione fine).

Il display mostra lo stato in cui si trova l'Atemega328P, che può essere:
NORMAL MODE o SETTING MODE.

Nello stato NORMAL MODE, il display mostra l'uscita in Volt della tensione di controllo (valore misurato tramite l'ADC dell'Atemega328P) e il valore assoluto del PWM. 
I pulsanti non generano alcuna azione apparente.

Per passare allo stato SETTING MODE è necessario tenere premuto il pulsante DOWN per due secondi.
Nello stato SETTING MODE, i pulsanti svolgono le loro funzioni, 
potendo regolare verso l'alto o verso il basso la tensione di controllo dell'uscita e visualizzarla sul display SSD1306.

Generando i FAULT (un ingresso in FAULT, due ingressi in FAULT, tre... ecc.), è possibile effettuare una regolazione fine della tensione di controllo necessaria per ogni tipo di FAULT.

Una volta completate le correzioni, premendo nuovamente il pulsante DOWN per due secondi, l' Atemega328P uscirà dallo stato SETTING MODE e si porterà a lo stato NORMAL MODE, facendo sì che tutte le impostazioni effettuate vengano memorizzate nella memoria EEPROM, in questo modo le impostazioni rimarranno permanenti anche se VCC è disconnesso.

Per ritornare ai valori iniziali (0, 1/6, 2/6, ecc.) o DEFAULT procedere come segue:
Dallo stato NORMAL MODE premere per due secondi, questa volta però, il tasto UP, 
il display cambia e mostrerà ora una riga con i valori da 1 a 6 e sotto di essi 
un cursore che si sposterà con i due tasti (con DOWN verso sinistra e UP verso la destra del display), posizionando il cursore sotto 6 ed uscendo dallo stato SETTING MODE con il tasto DOWN, tenendolo premuto per due secondi, le impostazioni dei valori della tensione di uscita torneranno a quelle di la tabella dei valori iniziali precedente.

Come si può vedere intuitivamente, è possibile scegliere il numero di input necessari da 1 a 6.

Gli ingressi non necessari saranno scartati dal circuito rimuovendo i relativi ponticelli (vedere circuito Six_In.pdf).

I valori iniziali della tensione di uscita saranno secondo la seguente tabella:

| **VALORI INIZIALI** | _**NESSUN FAULT**_ | _**1 FAULT**_ | _**2 FAULT**_ | _**3 FAULT**_ | _**4 FAULT**_ | _**5 FAULT**_ | _**6 FAULT**_ |
|:-------------------:|:------------------:|:-------------:|:-------------:|:-------------:|:-------------:|:-------------:|:-------------:|
|   **_x 6 input_**   |    _**0 Volt**_    | _**1/6 VCC**_ | _**1/3 VCC**_ | _**1/2 VCC**_ | _**2/3 VCC**_ | _**5/6 VCC**_ |   _**VCC**_   |
|   **_x 5 input_**   |    _**0 Volt**_    | _**1/5 VCC**_ | _**2/5 VCC**_ | _**3/5 VCC**_ | _**4/5 VCC**_ |   _**VCC**_   |   _**VCC**_   |
|   **_x 4 input_**   |    _**0 Volt**_    | _**1/4 VCC**_ | _**1/2 VCC**_ | _**3/4 VCC**_ |   _**VCC**_   |   _**VCC**_   |   _**VCC**_   |
|   **_x 3 input_**   |    _**0 Volt**_    | _**1/3 VCC**_ | _**2/3 VCC**_ |   _**VCC**_   |   _**VCC**_   |   _**VCC**_   |   _**VCC**_   |
|   **_x 2 input_**   |    _**0 Volt**_    | _**1/2 VCC**_ |   _**VCC**_   |   _**VCC**_   |   _**VCC**_   |   _**VCC**_   |   _**VCC**_   |
|   **_x 1 input_**   |    _**0 Volt**_    |   _**VCC**_   |   _**VCC**_   |   _**VCC**_   |   _**VCC**_   |   _**VCC**_   |   _**VCC**_   |

Cioè, le tensioni di uscita vengono trattate in base al numero di pin di ingresso scelti e in base al numero di ingressi che sono in FAULT.

Il numero di pin in questione così come le loro impostazioni una volta memorizzati nella memoria non volatile 
rimangono permanenti finché non viene effettuata una nuova modifica con i due pulsanti e memorizzati nuovamente.

Il display e i due pulsanti possono essere un modulo separabile.
Una volta impostato l'Atmega328P è possibile scollegare il modulo, 
rendendo più economica la produzione in quanto con un solo modulo si possono impostare altre schede e non essendo lasciati i pulsanti su di esso non permette di variare facilmente i valori per l'utente.

## Funzionamento degli ingressi in FAULT.

Ogni volta che viene generato un FAULT, la tensione di uscita viene generata rapidamente.
Quando la transizione avviene da FAULT a normale, l'uscita della tensione di controllo, avverrà gradualmente attraverso una rampa fino a 3 secondi, 
producendo così un'uscita di tensione di controllo di tipo Soft Start.


## Riferimento incrociato tra Atmega328P e Arduino Uno Rev.3.

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


## NOTA BENE:

A differenza di Six_In_Out fatto con l’ESP32 il quale e stato testato, questo firmware non e stato testato e collaudato ancora.
La idea finale è di fare un circuito dove inserire un singolo Atmega328P funzionando senza Xtal con oscillatore interno di 8MHz. e un filtro passa basso attivo con un OPAMP per il PWM.
La programmazione de l’Atmega328P si farà con il Arduino Uno Rev.3 via ICSP per ciò si adopera il Pin /RESET de l’Arduino (vedere circuito Six_In.pdf).
