# MARCAPASOS - THE ARDUINO MIDI SEQUENCER
<img width="1920" height="1080" alt="Marcapasos_banner" src="https://github.com/user-attachments/assets/2285efc9-fc4f-4331-9ff3-3ceb220bad96" />




🔗 Consulta la documentación técnica completa y detallada en mi página de Notion:

https://www.notion.so/MARCAPASOS-SEQUENCIADOR-MIDI-2cebd5980807801b87b1fa9e6536f0fe?source=copy_link
<!-- START_NOTION -->

Este es un proyecto realizado para la asignatura Laboratorio de Sonido II en la Esmuc. 

El Marcapasos es un secuenciador MIDI que funciona como un secuenciador analógico modular sin las limitaciones que supone tener todos los módulos que requiere un sintetizador modular.

Puedes encontrar como funciona cada apartado dando clic en los enlaces de las prestaciones.

Las prestaciones son todas aquellas características que me gustaría añadir en algún momento al dispositivo, aunque no sean posibles en los cuatro meses de la asignatura.

# **PRESTACIONES**

- [ ] **SINCRONIZACIÓN CON EL DAW**
  - [x] **Tempo**
  - [ ] Posición
  - [x] [**Subdivisiones:**](https://www.notion.so/36fbd598080780d9bde0de2cc4f7dc47)
    - [x] **Binarias **
    - [x] **Ternarias**
    - [x] **Compuestas**
  - [ ] Ligado de los pasos
  - [x] [**Decidir si recibe MIDI_CLOCK o si lo envía**](https://www.notion.so/36fbd598080780c8bdfbc918d2e3e9ef)
- [ ] **PASOS EDITABLES**
  - [x] [**Muteado de los pasos**](https://www.notion.so/36fbd598080780419057ef35fa2849cf)
  - [x] [**Edición del número de pasos (Max de 16)**](https://www.notion.so/36fbd59808078011a2aadc185fa75882)
  - [x] **Extender la secuencia a 32 mediante los botones de slot**
  - [ ] Extensión del paso
- [x] **CONTROL**
  - [x] **Potenciómetros para cada valor de la secuencia**
  - [x] [**Pulsadores para el muteado y desmuteado**](https://www.notion.so/36fbd598080780419057ef35fa2849cf)
  - [x] **Botones de control de la pantalla**
  - [x] [**Escoger fácilmente la octava de cada una de las notas**](https://www.notion.so/36fbd598080780fea1caeed5433170bc)
  - [x] [**Escoger fácilmente la velocity de cada una de las notas**](https://www.notion.so/36fbd598080780eea508ef71374ae587)
  - [x] Transportar según nota de entrada de otro controlador
  - [x] [**Mapear posición del potenciómetro a una escala**](https://www.notion.so/36fbd598080780b19d55cec38a8eafa3)
  - [x] **Botones para iniciar y parar la secuencia con leds que lo visualicen**
- [ ] **CONEXIONES**
  - [x] **USB**
  - [ ] Opcional DIN4
- [x] **ENVÍO**
  - [x] **Notas a diferentes alturas**
  - [x] [**Notas con diferentes velocidades**](https://www.notion.so/36fbd598080780eea508ef71374ae587)
  - [x] [**Control Change**](https://www.notion.so/36fbd598080780c189a0f2511ea4e083)
  - [x] [**Enviar varias secuencias a la vez**](https://www.notion.so/36fbd59808078066aa01e511496fedfd)
- [x] [**PRESETS**](https://www.notion.so/36fbd59808078064a17fe55298546050)
  - [x] **Guardar las secuencias y los patterns en una SD**
  - [x] **Guardar las secuencias en slots para cargarlas automaticamente**
# **MATERIAL NECESARIO**

1. x1 Arduino mega
1. x1 Pantalla I2C 20x4
1. x16 Potenciometros
1. x16 Botones para los mutes
1. x5 Botones para el play/stop de las secuencias
1. x3 Botones de control
1. 1x Encoder
1. x1 Mega Proto Shield
1. x40 Cables Dupont Macho-Macho
1. x40 Cables Dupont Macho-Hembra
1. x5 Resistencias para los leds
1. X5 Leds
1. Ribbon con clavija FC10P
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





# **SUBPÁGINAS DEL NOTION**









<!-- END_NOTION -->

esto es un pie de pagina
