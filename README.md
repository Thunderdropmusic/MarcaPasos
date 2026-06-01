# MARCAPASOS - THE ARDUINO MIDI SEQUENCER

<img width="1001" height="484" alt="imagen" src="https://github.com/user-attachments/assets/97bb485b-a2eb-4de0-858c-7178f29842e2" />


🔗 Consulta la documentación técnica completa y detallada en mi página de Notion:

https://www.notion.so/MARCAPASOS-SEQUENCIADOR-MIDI-2cebd5980807801b87b1fa9e6536f0fe?source=copy_link
<!-- START_NOTION -->

Aquest és un projecte realitzat per a l'assignatura Laboratori de So II en la Esmuc.

El Marcapassos és un seqüenciador MIDI que funciona com un seqüenciador 
analògic modular sense les limitacions que suposa tenir tots els mòduls 
que requereix un sintetitzador modular.

Pots trobar com funciona cada apartat donant clic en els enllaços de les prestacions.

Les prestacions són totes aquelles característiques que m'agradaria afegir 
en algun moment al dispositiu, encara que no siguin possibles en els 
quatre mesos de l'assignatura.

# **PRESTACIONS**

- [ ] **SINCRONIZACIÓ EN EL DAW**
  - [x] **Tempo**
  - [ ] Posició
  - [x] [**Subdivisions:**](https://www.notion.so/2f5bd598080780a48fc1de89fd2955de)
    - [x] **Binaris **
    - [x] **Ternaris**
    - [x] **Compostes**
  - [ ] Lligat dels pasos
  - [x] [**Decidir si reb MIDI_CLOCK o si ho envia**](https://www.notion.so/316bd598080780cbaf06ceae53ab0743)
- [ ] **PASOS EDITABLES**
  - [x] [**Muteat dels pasos**](https://www.notion.so/304bd5980807806a8955d3bfb23b4774)
  - [x] [**Edició del nombre de pasos (Max de 16)**](https://www.notion.so/303bd598080780b78244d991088aca70)
  - [x] **Extendre la secuencia a 32 mitjançant els botons de slot**
  - [ ] Extensió del pas
- [ ] **CONTROL**
  - [x] **Potenciómetres per cada valor de la seqüencia**
  - [x] [**Pulsadors pa el muteat y desmutea**](https://www.notion.so/304bd5980807806a8955d3bfb23b4774)**t**
  - [x] **Botons de control de la pantalla**
  - [x] [**Escogir fácilment la octava de cada una de les notes**](https://www.notion.so/30cbd5980807801a8d3bca6844b82bbb)
  - [x] [**Escogir fácilmente la velocity de cada una de les notes**](https://www.notion.so/313bd5980807800ca0a0ca5a68b020ec)
  - [x] Transportar según nota de entrada de un altre controlador
  - [x] [**Mapear posició del potenciómetre a una escala**](https://www.notion.so/312bd598080780fcbd99f2a58fa7907a)
  - [x] **Botons per iniciar y parar la seqüencia en leds que lo vitzualitzin**
- [ ] **CONEXIONS**
  - [x] **USB**
  - [ ] Opcional DIN4
- [x] **ENVÍO**
  - [x] **Notes a diferents altures**
  - [x] [**Notes en diferents velocitats**](https://www.notion.so/313bd5980807800ca0a0ca5a68b020ec)
  - [x] [**Control Change**](https://www.notion.so/32dbd59808078027b146d4fe8eaf7e7f)
  - [x] [Enviar varies seqüencias a la volta](https://www.notion.so/318bd5980807801e8b10ceddc1424088)
- [x] [**PRESETS**](https://www.notion.so/2edbd5980807802a97d0d0668aec845c)
  - [x] **Guardar les seqüencies y els patterns en una SD**
  - [x] **Guardar les seqüencies en slots para carregar-les automaticament**
# **MATERIAL NECESARI**

1. x1 Arduino mega
1. x1 Pantalla I2C 20x4
1. x16 Potenciometres
1. x16 Botons per els mutis
1. x5 Botons per al play/stop de les seqüències
1. x3 Botons de control
1. 1x Encoder
1. x1 Mega Proto Shield
1. x40 Cables Dupont Mascle-Mascle
1. x40 Cables Dupont Mascle-Femella
1. x5 Resistències per als leds
1. X5 Leds
1. Ribbon amb clavilla FC10P (No recomendable)
1. Cables Dupont x50
# **SOFTWARE**

1. [LoopMidi](https://www.tobias-erichsen.de/software/loopmidi.html)
1. [HairLessMidi](https://projectgus.github.io/hairless-midiserial/)
1. [MIDI-OX](http://www.midiox.com/)
# **BIBLIOGRAFÍA**

## MIDI

[https://www.prometec.net/midi-conexiones-senales-mensajes/](https://www.prometec.net/midi-conexiones-senales-mensajes/)

[https://www.prometec.net/midi-recibir-manipular-mensajes/](https://www.prometec.net/midi-recibir-manipular-mensajes/)

[https://midi.org/expanded-midi-1-0-messages-list](https://midi.org/expanded-midi-1-0-messages-list)

[https://midi.org/spec-detail](https://midi.org/spec-detail)

[https://projectgus.github.io/hairless-midiserial/](https://projectgus.github.io/hairless-midiserial/)

## ARDUINO

[https://www.pjrc.com/teensy/td_libs_TimerOne.html](https://www.pjrc.com/teensy/td_libs_TimerOne.html)

[https://www.luisllamas.es/curso-arduino-iniciacion/](https://www.luisllamas.es/curso-arduino-iniciacion/)

[https://www.youtube.com/playlist?list=PL4_gPbvyebyH2xfPXePHtx8gK5zPBrVkg](https://www.youtube.com/playlist?list=PL4_gPbvyebyH2xfPXePHtx8gK5zPBrVkg)

[https://naylampmechatronics.com/blog/34_tutorial-lcd-conectando-tu-arduino-a-un-lcd1602-y-lcd2004.html](https://naylampmechatronics.com/blog/34_tutorial-lcd-conectando-tu-arduino-a-un-lcd1602-y-lcd2004.html)

[https://naylampmechatronics.com/blog/35_tutorial-lcd-con-i2c-controla-un-lcd-con-solo-dos-pines.html](https://naylampmechatronics.com/blog/35_tutorial-lcd-con-i2c-controla-un-lcd-con-solo-dos-pines.html)

[https://www.luisllamas.es/arduino-encoder-rotativo/](https://www.luisllamas.es/arduino-encoder-rotativo/)

[https://www.luisllamas.es/4-consejos-para-programar-codigo-mas-limpio-en-arduino/](https://www.luisllamas.es/4-consejos-para-programar-codigo-mas-limpio-en-arduino/)

[https://www.luisllamas.es/tarjeta-micro-sd-arduino/](https://www.luisllamas.es/tarjeta-micro-sd-arduino/)

# **SUBPÁGINES DEL NOTION**







1. x1 Arduino mega

2. x1 Pantalla I2C 20x4

3. x16 Potenciometres

4. x16 Botons per els mutis

5. x5 Botons per al play/stop de les seqüències

6. x3 Botons de control

7. 1x Encoder

8. x1 Mega Proto Shield

9. x40 Cables Dupont Mascle-Mascle

10. x40 Cables Dupont Mascle-Femella

11. x5 Resistències per als leds

12. X5 Leds

13. Ribbon amb clavilla FC10P

14. Cables Dupont x50



<!-- END_NOTION -->

esto es un pie de pagina
