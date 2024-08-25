/*
 * Progetto: Six_In
 * Autore: Italo Greco
 * Licenza: MIT License (vedi LICENSE per i dettagli).
 *
 *  Tensione di uscita controllata da un massimo di 
 *  sei ingressi digitali.
 *
**/

#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Button2.h>
#include <Hardware.h>

#define SCREEN_WIDTH 128  //  Larghezza di visualizzazione in pixel
#define SCREEN_HEIGHT 64  //  Visualizza l'altezza in pixel
#define OLED_RESET    -1  //  Pin di ripristino del display (non utilizzato con I2C)

/*
#define DEBUG_ON  // Abilitazione alla depurazione

#ifdef DEBUG_ON
  Serial.println("Messaggio di depurazione...");
#endif
*/

const int OUT_PWM = 5; // Pin a cui è collegato il PWM sull'ARDUINO UNO
const int IN_ADC = A0;  // Pin a cui è collegato il ADC sull'ARDUINO UNO
/*
Risoluzione ADC su Arduino Uno:
Arduino Uno ha un ADC a 10 bit,
il che significa che analogRead()
restituisce un valore compreso tra 0 e 1023,
corrispondente a un intervallo di tensione compreso tra 0 e 5 V
(o da 0 a qualsiasi riferimento di tensione in uso).
Non è possibile modificare la risoluzione dell'ADC su Arduino Uno.
È fissato a 10 bit.
*/

const int KEY_DOWN = 2;  // I pin per pulsanti
const int KEY_UP = 3;

//Definisce i pin utilizzati come input
const int INPUT_1 = 8;
const int INPUT_2 = 9;
const int INPUT_3 = 10;
const int INPUT_4 = 11;
const int INPUT_5 = 12;
const int INPUT_6 = 13;
const int inputPins[] = {INPUT_1, INPUT_2, INPUT_3, INPUT_4, INPUT_5, INPUT_6, KEY_DOWN, KEY_UP};
const int numPinsFault = sizeof(inputPins) / sizeof(inputPins[0]);

const float Vref = 5.0;  // Riferimento di tensione (5V per Arduino Uno)

volatile bool Initial = true;
volatile bool Normal = true;
volatile bool Setting = false;
volatile bool Fault = true;
volatile bool ValueFault = false;

uint8_t pwmValue = 0;
uint8_t pwmValueAnt = 0;
uint16_t valueADC = 0;
int sum = 0;
float voltage = 0.0;

int numbers[] = {1, 2, 3, 4, 5, 6};
int currentSelection = 5; // Indice del numero selezionato

// Definir el array de 14 valores uint8_t
uint8_t ValueRecupery[14] = {};
// Indirizzo iniziale nella EEPROM dove verranno salvati i dati
const int direzioneIniziale = 0;

const int NUM_SAMPLES = 5;  // Numero di campionamento
int samples[NUM_SAMPLES];

const unsigned long SHORT_INTERVAL = 50;  // Intervallo corto in ms
const unsigned long LONG_INTERVAL = 80;   // Intervallo lungo in ms

Adafruit_SSD1306 disp(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // I2C: A4->SDA, A5->SCL

// Creiamo istanze Button2 per ciascun pulsante
Button2 keyDown;
Button2 keyUp;
// Variabili per gestire tempi di pressione prolungati
const unsigned long LONG_PRESS_TIME = 2000;

void singleClick1(Button2& btn) {
  if (Normal) {
    if (Setting) {
      if (pwmValue >= 1 && pwmValue <= ValueRecupery[1]) {
        ValueRecupery[0] = --pwmValue;
      } else if (pwmValue >= ValueRecupery[1]+2 && pwmValue <= ValueRecupery[3]) {
        ValueRecupery[2] = --pwmValue;
      } else if (pwmValue >= ValueRecupery[3]+2 && pwmValue <= ValueRecupery[5]) {
        ValueRecupery[4] = --pwmValue;
      } else if (pwmValue >= ValueRecupery[5]+2 && pwmValue <= ValueRecupery[7]) {
        ValueRecupery[6] = --pwmValue;
      } else if (pwmValue >= ValueRecupery[7]+2 && pwmValue <= ValueRecupery[9]) {
        ValueRecupery[8] = --pwmValue;
      } else if (pwmValue >= ValueRecupery[9]+2 && pwmValue <= ValueRecupery[11]) {
        ValueRecupery[10] = --pwmValue;
      } else if (pwmValue >= ValueRecupery[11]+2 && pwmValue <= 255) {
        ValueRecupery[12] = --pwmValue;
      }
      analogWrite(OUT_PWM, pwmValue);  // Invia pwmValue all'uscita del PWM
    }
  } else {
    currentSelection--;
    if (currentSelection < 0) {
      currentSelection = 5;
    }
  }
}  

void singleClick2(Button2& btn) {
  if (Normal) {
    if (Setting) {
      if (pwmValue >= 0 && pwmValue <= ValueRecupery[1]-1) {
        ValueRecupery[0] = ++pwmValue;
      } else if (pwmValue >= ValueRecupery[1]+1 && pwmValue <= ValueRecupery[3]-1) {
        ValueRecupery[2] = ++pwmValue;
      } else if (pwmValue >= ValueRecupery[3]+1 && pwmValue <= ValueRecupery[5]-1) {
        ValueRecupery[4] = ++pwmValue;
      } else if (pwmValue >= ValueRecupery[5]+1 && pwmValue <= ValueRecupery[7]-1) {
        ValueRecupery[6] = ++pwmValue;
      } else if (pwmValue >= ValueRecupery[7]+1 && pwmValue <= ValueRecupery[9]-1) {
        ValueRecupery[8] = ++pwmValue;
      } else if (pwmValue >= ValueRecupery[9]+1 && pwmValue <= ValueRecupery[11]-1) {
        ValueRecupery[10] = ++pwmValue;
      } else if (pwmValue >= ValueRecupery[11]+1 && pwmValue <= 255) {
        ValueRecupery[12] = ++pwmValue;
      }
      analogWrite(OUT_PWM, pwmValue);  // Invia pwmValue all'uscita del PWM
    }
  } else {
    currentSelection++;
    if (currentSelection > 5) {
      currentSelection = 0;
    }
  }
}

// Calcolo della frammentazione dei valori PWM
void fragment(int n, byte *arr) {
  // Calcoliamo la nuova dimensione segmentata
  int segment = 2 * n;
  // Calcola l'incremento per suddividere l'intervallo utilizzando numeri interi
  int increment = 255 / segment; // Divisione intera troncata
  // Riempi l'array con i valori suddivisi in blocchi utilizzando il troncamento
  for (int i = 0; i <= segment; i++) {
    arr[i] = i * increment; // Qui viene eseguita la moltiplicazione dei numeri interi
  }
  // Regola l'ultimo valore in modo che sia esattamente 255
  arr[segment] = 255;
  // Riempi gli spazi rimanenti con 255 se la segmentazione <12
  for (int i = segment + 1; i < 13; i++) {
    arr[i] = 255;
  }
  // L'ultimo elemento dell'array e la quantita di pin che si decidera adoperare
  arr[13] = n;

  ScrivoArray(ValueRecupery);
}

// Leggere l'array dalla EEPROM
void LeggoArray(byte *array) {
  int max = sizeof(array) / sizeof(array[0]);
  for (int i = 0; i < max; i++) {
    array[i] = EEPROM.read(direzioneIniziale + i);
  }
}

// Scrivere l'array nella EEPROM
void ScrivoArray(byte *array) {
  int max = sizeof(array) / sizeof(array[0]);
  for (int i = 0; i < max; i++) {
    EEPROM.write(direzioneIniziale + i, array[i]);
  }
}

/*
Funzione Callback da richiamare alla pressione del pulsante.
Quando entra per la prima volta Setting = true
e la seconda volta, impostando su false, scrive
i nuovi valori nell'array ValueRecupery[].
*/

void longClick1(Button2& btn) {
  if (Normal) {
    Setting = !Setting;
    if(!Setting) {
      ScrivoArray(ValueRecupery);      
    }
  } else {
    switch(numbers[currentSelection]) {
      case 1:
      fragment(1, ValueRecupery);
      break;
    
      case 2:      
      fragment(2, ValueRecupery);
      break;
     
      case 3:  
      fragment(3, ValueRecupery);
      break;
     
      case 4:     
      fragment(4, ValueRecupery);
      break;
      
      case 5:   
      fragment(5, ValueRecupery);
      break;
     
      case 6:     
      fragment(6, ValueRecupery);
      break;
     
      default: break;                            
    }
    LeggoArray(ValueRecupery);
    Initial = true;
    Normal = true;    
  }
}

void longClick2(Button2& btn) {
  Normal = false;
}

// ISR per PCINT[7:0]: gestisce gli interrupt sui pin da 8 a 13
ISR(PCINT0_vect) {
  Fault = true;
}

void setup() {
  Serial.begin(115200);

  // Inizializza i pin come ingressi 8 a 13 è KEY_DOWN, KEY_UP
  for (int i = 0; i < numPinsFault; i++) {
    pinMode(inputPins[i], INPUT_PULLUP);
  }
  
  // Abilita gli interrupt di cambio pin (PCINT) per i pin da 8 a 13 
  PCICR  |= (1 << PCIE0);   //  Abilita gli interrupt per PCINT[7:0]
  PCMSK0 |= (1 << PCINT0);  //  Abilita l'interruzione per il pin 8
  PCMSK0 |= (1 << PCINT1);  //  Abilita l'interruzione per il pin 9
  PCMSK0 |= (1 << PCINT2);  //  Abilita l'interruzione per il pin 10
  PCMSK0 |= (1 << PCINT3);  //  Abilita l'interruzione per il pin 11
  PCMSK0 |= (1 << PCINT4);  //  Abilita l'interruzione per il pin 12
  PCMSK0 |= (1 << PCINT5);  //  Abilita l'interruzione per il pin 13

  //  Abilita gli interrupt globali
  sei();

  pinMode(IN_ADC, INPUT);  
 
  LeggoArray(ValueRecupery);

  #ifdef DEBUG_ON
  // Stampa l'array ottenuto
  Serial.print("Array ottenuto: ");
  for (int i = 0; i < 14; i++) {
    Serial.print(ValueRecupery[i]);
    Serial.print(" ");
  }
  #endif

  if (ValueRecupery[13] < 1 || ValueRecupery[13] > 6) {  //  Controlla se sono stati trovati dati
    //  Se non sono stati trovati dati, utilizzare i valori predefiniti e salva l'array
    fragment(6, ValueRecupery); // valore di default   
  }

  keyDown.begin(KEY_DOWN);  // Inizializzare i pulsanti.
  keyUp.begin(KEY_UP);
  
  // Imposta il tempo di pressione prolungata per il button1 e button2
  keyDown.setLongClickTime(LONG_PRESS_TIME);
  keyUp.setLongClickTime(LONG_PRESS_TIME);

  // Configurare i handlers per button1
  keyDown.setTapHandler(singleClick1);
  keyDown.setLongClickDetectedHandler(longClick1);

  // Configurare i handlers per button2
  keyUp.setTapHandler(singleClick2);
  keyUp.setLongClickDetectedHandler(longClick2); 

  disp.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // inizializzare il display con l'indirizzo I2C 0x3C
  disp.clearDisplay();
  disp.display();
}

void loop() {

  // Chiama il metodo loop() per ciascun pulsante nel ciclo principale
  keyDown.loop();
  keyUp.loop();

  if(Initial) {  
    Fault = true;
    Initial = false;
  }

  if(Fault) {
    pwmValueAnt = pwmValue;
    for (int i = 0; i < numPinsFault; i++) {
      delay(100);
      // Fare la somma dei pin che sono a livello basso
      if (digitalRead(inputPins[i]) == LOW) {
        sum += 1;      
      }
    }

    // Calcolare il valore PWM proporzionale al numero di pin a livelli bassi   
    switch(sum){
      case 0: pwmValue = ValueRecupery[0]; break;
      case 1: pwmValue = ValueRecupery[2]; break;
      case 2: pwmValue = ValueRecupery[4]; break;
      case 3: pwmValue = ValueRecupery[6]; break;
      case 4: pwmValue = ValueRecupery[8]; break;
      case 5: pwmValue = ValueRecupery[10]; break;
      case 6: pwmValue = ValueRecupery[12]; break;
      default: break;
    }

    if (pwmValue >= pwmValueAnt) {     
      analogWrite(OUT_PWM, pwmValue);
      pwmValueAnt = 0;
      ValueFault = false;
    } else {
      ValueFault = true;
    }
    sum = 0;
    Fault = false;
  }  
    
  unsigned long currentMillis = millis();
  static unsigned long previousMillis = 0;

  if (ValueFault) {
    if (currentMillis - previousMillis >= LONG_INTERVAL) {
      --pwmValueAnt;     
      if (pwmValueAnt <= pwmValue) {
        analogWrite(OUT_PWM, pwmValue);  // Assicurati di non superare il valore obiettivo
        pwmValueAnt = 0;
        previousMillis = 0;
        ValueFault = false;
      } else {
        analogWrite(OUT_PWM, pwmValueAnt);
        previousMillis = currentMillis;
      }
    }
  }  
  
  static int sampleIndex = 0;
  static unsigned long lastShortIntervalTime = 0;

  // Controlla se gli intervalli di tempo per la raccolta dei campioni sono trascorsi
  if (currentMillis - lastShortIntervalTime >= SHORT_INTERVAL) {   
    if (sampleIndex < NUM_SAMPLES) {
      samples[sampleIndex] = analogRead(IN_ADC);
      sampleIndex++;
    } else {
      // Ordina campioni      
      for (int i = 0; i < NUM_SAMPLES - 1; i++) {
        for (int j = i + 1; j < NUM_SAMPLES; j++) {
          if (samples[j] < samples[i]) {
            int temp = samples[i];
            samples[i] = samples[j];
            samples[j] = temp;
          }
        }
      }      
      valueADC = samples[NUM_SAMPLES / 2];  // Prendi il campione centrale      
      sampleIndex = 0;  // Reimposta l'Index di campionamento dopo aver raggiunto il massimo
    }
    lastShortIntervalTime = currentMillis;  //  Aggiorna la tempistica dell'ultimo intervallo
  }
  
  voltage = ((float)valueADC / 1023.0) * Vref;  //  Convertire in tensione
 
  disp.setTextSize(1);               
  disp.setTextColor(WHITE, BLACK);
  disp.clearDisplay();
  disp.setCursor(0, 0);
  if (Normal) {
    if (Setting) {
      disp.println("SETTING MODE:");  
    } else {
      disp.println("NORMAL MODE:");
    } 
    disp.println();
    disp.println("PWM to Volts -->");
    disp.setCursor(0, 29);
    disp.setTextSize(2);
    disp.print(voltage, 2);
    disp.println(" V");
    disp.setTextSize(1); 
    disp.println();
    disp.print("pwmValue = ");
    disp.println(pwmValue); 
    disp.display();
  } else {
    disp.setCursor(0, 0);
    disp.setTextSize(1); 
    disp.print("ACTUAL PINS = "); 
    disp.println(ValueRecupery[13]);    
    disp.println();
    disp.println("INPUT PINS ? -->");
    disp.println();
    for (int i = 0; i < 6; i++) {
      disp.print(numbers[i]);
      disp.print(" ");
    }    
    int cursorX = currentSelection * 12; // Regolare lo spazio in base alla fonte e alle dimensioni
    disp.setCursor(cursorX, 40);
    disp.print("^");
    disp.println();
    disp.println();
    disp.println("Up/Down for select");     
    disp.display();   
  }
}
