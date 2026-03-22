La memoria comienza antes de empezar las clases de la asignatura, pues se me recomendó empezar con tiempo porque el proyecto era bastante grande. 

## 17/01/2026

Primeras pruebas Midi. Se consigue por primera vez recibir mensajes Midi en el Arduino desde el DAW usando la librería Midi.h y el programa Hairless MIDI y se hacen las primeras pruebas de recepción del Midi Clock.

```javascript
#include <MIDI.h>
#include <TimerOne.h>

struct HairlessConfig : public midi::DefaultSettings {
  static const long BaudRate = 115200;
};

MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, HairlessConfig);
int tiempo;
volatile boolean notaFuera;
void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff();
}
```



## 3/2/2026

Primera presentación del proyecto 

<div style="margin-bottom: 20px;"><img src="docs/assets/img_2fbbd598-0807-802d-b24e-de7df97d8244.png" width="400" style="display: block; margin-bottom: 5px;" alt="Una caja" /><p style="margin-top: 0px;"><sub><i>Una caja</i></sub></p></div>

## 10/02/2026

Se ha conseguido añadir hasta ocho potenciómetros y se ha empezado a trabajar la programación del muteado de los pasos. Tambien se ha conseguido editar la longitud de la secuencia desde el Arduino:

📄 [Edición del número de pasos (Max de 16)](https://www.notion.so/303bd598080780b78244d991088aca70)



<div style="margin-bottom: 20px;"><img src="docs/assets/img_304bd598-0807-80a8-9af3-ea392d34d000.png" width="400" style="display: block; margin-bottom: 5px;" alt="Protoboard en la fecha de la memoria" /><p style="margin-top: 0px;"><sub><i>Protoboard en la fecha de la memoria</i></sub></p></div>

## 17/02/2026

Se han añadido 8 botones para mutear cada uno de los potenciómetros independientemente.

📄 [Muteado de los pasos](https://www.notion.so/304bd5980807806a8955d3bfb23b4774)



También, se ha optimizado el código de dibujado de la pantalla para que solo cambie los caracteres que se mueven y no que redibuje todo cada vez que haces un cambio. Esto sobre todo es importante en el visionado de los pasos:



<div style="margin-bottom: 20px;"><img src="docs/assets/img_30abd598-0807-80b9-bdbf-e29a836add72.png" width="400" style="display: block; margin-bottom: 5px;" alt="Visionado de los pasos" /><p style="margin-top: 0px;"><sub><i>Visionado de los pasos</i></sub></p></div>



```c++
inline void drawSteps() {
  if(mutesValue[nStep]){
    if(mutesValue[nStepAnterior]){
      lcd.setCursor(nStepAnterior, 1);
      lcd.write(byte(5));
      lcd.setCursor(nStep,1);
      lcd.write(byte(5));
    }
    else if(!mutesValue[nStepAnterior]){
      lcd.setCursor(nStepAnterior, 1);
      lcd.write(byte(6));
      lcd.setCursor(nStep,1);
      lcd.write(byte(5));
    }
  }
  else if(!mutesValue[nStep]){
    if(mutesValue[nStepAnterior]){
      lcd.setCursor(nStepAnterior, 1);
      lcd.write(byte(5));
      lcd.setCursor(nStep,1);
      lcd.write(byte(7));
    }
    else if(!mutesValue[nStepAnterior]){
      lcd.setCursor(nStepAnterior, 1);
      lcd.write(byte(6));
      lcd.setCursor(nStep,1);
      lcd.write(byte(7));
      
    }
  }
  nStepAnterior = nStep;
}
```

Se ha empezado también a trabajar la cuantización a escala y poder escoger desde los potenciómetros, notas que pertenezcan a la escala escogida:

```c++
int escalaSeleccionada = 1;
const int escalasNotas[5][13] = {
  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}, //Escala cromática
  {0, 2, 3, 5, 7, 8, 10, 12, 0, 0, 0, 0, 0}, //Escala menor Natural
  {0, 2, 4, 5, 7, 9, 11, 12, 0, 0, 0, 0, 0}, //Escala Mayor
  {0, 2, 4, 6, 8, 10, 12, 0, 0, 0, 0, 0, 0}, //Escala de tonos
  {0, 1, 3, 4, 6, 7, 9, 10, 12, 0, 0, 0, 0} //Escala Semitono-tono
};
const int nNotasEscalas[5] = {13, 8, 8, 7, 9};

[...]

  for(int i = 0; i < numPotes; i++){
    int indice = constrain(map(analogRead(potesPin[i]), 1018, 30, 0, nNotasEscalas[escalaSeleccionada] - 1), 0, nNotasEscalas[escalaSeleccionada] - 1);
    int val = escalasNotas[escalaSeleccionada][indice];
```



Tambien se ha empezado a trabajar la gestión de las octavas, pudiendo ahora transportar toda la secuencia octavas hacia arriba y hacia abajo pulsando los botones de mute 1, 2 y 3, 4 respectivamente a la vez:

```c++
      if(mutePulsado[0] && mutePulsado[1] && octava < 9 && tiempoActualMillis - ultimoTiempoBotonMute[0] > 250 && tiempoActualMillis - ultimoTiempoBotonMute[1] > 250){
        octava ++;
        ultimoTiempoBotonMute[0] = tiempoActualMillis;
        ultimoTiempoBotonMute[1] = tiempoActualMillis;
        timeShowOctValue = tiempoActualMillis;
        menuActual = 4;
        updateLCD = true;
      }
      else if(mutePulsado[2] && mutePulsado[3] && octava > 0 && tiempoActualMillis - ultimoTiempoBotonMute[2] > 250 && tiempoActualMillis - ultimoTiempoBotonMute[3] > 250){
        octava --;
        ultimoTiempoBotonMute[2] = tiempoActualMillis;
        ultimoTiempoBotonMute[3] = tiempoActualMillis;
        timeShowOctValue = tiempoActualMillis;
        menuActual = 4;
        updateLCD = true;
      }
      else if(tiempoActualMillis - TiempoPulsadoBotonMute[i] > 100 && tiempoActualMillis - ultimoTiempoBotonMute[i] > 250){
        mutesValue[i] = !mutesValue[i];
        ultimoTiempoBotonMute[i] = tiempoActualMillis;
        updateLCD = true;
```

Tambien se ha organizado la carpeta SRC del GitHub por versiones añadiendo un log por cada una para ver los cambios. Ese log está hecho con gemini pasándole la versión nueva y que analice que cambia con la versión antigua..

## 24/02/2026

Se ha conseguido cambiar los valores de la velocity individualmente pulsando el boton mute y girando el pote, así como cambiar la octava por cada una de las notas de la secuencia convirtiendo los botones en un selector:

📄 [Escoger fácilmente la octava de cada uno de los potenciometros](https://www.notion.so/30cbd5980807801a8d3bca6844b82bbb)

📄 [Escoger fácilmente la velocity de cada una de las notas](https://www.notion.so/313bd5980807800ca0a0ca5a68b020ec)



Se ha hecho una Reunión con el profesor para determinar hacia donde caminar: el siguiente paso es cambiar la manera en la que se detectan los potenciómetros, haciendo una comparativa de los valores reales de los potenciómetros y no de su valor convertido a notas.

Se ha hablado como organizar la construcción de prototipo, en especial de los botones. Se compraran varios paneles pcb para conectar las tierras de los botones.

## 03/03/2026

Se ha reestructurado todo el código, convirtiendo cada archivo .h en una clase con sus variables y sus funciones para mejor entendimiento. De esta manera tenemos los siguientes archivos:

El archivo que más cambios ha presentado ha sido midiProgramming, pues se ha convertido su clase en un objeto para poder ser lanzado varias veces sin depender de arrays y manteniendo los valores de las variables en cada gestión de la secuencia. De esta manera, cada secuencia tiene una cantidad de pasos, una subdivisión y un modo de subdivisión completamente independientes.

Puedes ver como funciona mejor en la siguiente página de notion:

Para ello también, se han convertido las variables de la secuencia (noteValue[], velocityValue[], subdivision] en arrays (los que ya eran arrays, en matrices 2d) donde cada indice son los valores de cada secuencia:

## 11/03/2026

Se ha conseguido implementar definitivamente varias secuencias a la vez con un control de play, stop y un modo de subdivisión completamente independientes, pudiendo escoger secuencias con subdivisión simple y otras compleja. 

También se ha implementado un modo de MidiClock, pudiendo tanto recibirlo y sincronizarse con el DAW o enviarlo y que el propio marcapasos sincronice el DAW. Puedes encontrar más información en la siguiente pagina:

Tambien se ha realizado el trabajo de limpiar el código convirtiendo variables de int a byte para liberar memoria reduciéndola un 15% y una liberación de procesamiento dejando que las tareas pesadas como la pantalla o la lectura de potenciómetros lo haga una sola vez.

### 17/03/2026

Se ha seguido limpiando el código generando funciones para tareas que se realizan en muchos momentos del código, favoreciendo la lectura y la organización. 

Se está trabajando en la incorporación del sistema de envio de mensajes CC. Por cada secuencia, puedes escoger si será una secuencia de notas o una secuencia de automatización.

El funcionamiento será el siguiente. Cada potenciómetro tendrá un valor de 0 a 127 y se realizará una interpolación entre cada uno de los valores de 24 puntos (uno por cada mensaje MidiClock que recibe). Además, tambien se ha añadido la función de poder editar la curva de la interpolación, de manera más exponencial o logarítmica.

La idea futura es que funcione como un clip de automatización donde cada uno de los steps sean los puntos de la automatización y con los mutes vayas activando cada uno de los puntos. 





