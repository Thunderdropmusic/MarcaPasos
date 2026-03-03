# MARCAPASOS - THE ARDUINO MIDI SEQUENCER

🔗 Consulta la documentación técnica completa y detallada en nuestro Notion:

https://www.notion.so/MARCAPASOS-SEQUENCIADOR-MIDI-2cebd5980807801b87b1fa9e6536f0fe?source=copy_link
<!-- START_NOTION -->

Este es un proyecto realizado para la asignatura Laboratorio de Sonido II en la Esmuc. 

El Marcapasos es un secuenciador MIDI que funciona como un secuenciador analógico modular sin las limitaciones que supone tener todos los módulos que requiere un sintetizador modular.

Puedes encontrar como funciona cada apartado dando clic en los enlaces de las prestaciones.

Las prestaciones son todas aquellas características que me gustaría añadir en algún momento al dispositivo, aunque no sean posibles en las cuatro semanas de la asignatura.

# **PRESTACIONES**

- [ ] **SINCRONIZACIÓN CON EL DAW**
  - [x] Tempo
  - [ ] Posición
  - [x] [Subdivisiones:](https://www.notion.so/2f5bd598080780a48fc1de89fd2955de)
    - [x] Binarias 
    - [x] Ternarias
    - [x] Compuestas
  - [ ] Ligado de los pasos
  - [x] [Decidir si recibe MIDI_CLOCK o si lo envía](https://www.notion.so/316bd598080780cbaf06ceae53ab0743)
- [ ] **PASOS EDITABLES**
  - [x] [Muteado de los pasos](https://www.notion.so/304bd5980807806a8955d3bfb23b4774)
  - [x] [Edición del número de pasos (Max de 16)](https://www.notion.so/303bd598080780b78244d991088aca70)
  - [ ] Extensión del paso
- [ ] **CONTROL**
  - [x] Potenciómetros para cada valor de la secuencia
  - [x] [Pulsadores para el muteado y desmuteado](https://www.notion.so/304bd5980807806a8955d3bfb23b4774)
  - [ ] Botones de control de la pantalla
  - [x] [Escoger fácilmente la octava de cada una de las notas](https://www.notion.so/30cbd5980807801a8d3bca6844b82bbb)
  - [x] [Escoger fácilmente la velocity de cada una de las notas](https://www.notion.so/313bd5980807800ca0a0ca5a68b020ec)
  - [ ] Transportar según nota de entrada de otro controlador
  - [x] [Mapear posición del potenciómetro a una escala](https://www.notion.so/312bd598080780fcbd99f2a58fa7907a)
- [ ] **CONEXIONES**
  - [x] USB
  - [ ] Opcional DIN4
- [ ] **ENVÍO**
  - [x] Notas a diferentes alturas (cada pote una octava)
  - [x] Notas con diferentes velocidades (seleccionar la nota con el mute y al mover el pote, que cambie la velocity de la nota)
  - [x] diferentes melodías en diferentes voces
  - [ ] Control Change
  - [x] Intentar poder enviar varias cosas a la vez y ver cual es el limite.
- [ ] [**PRESETS**](https://www.notion.so/2edbd5980807802a97d0d0668aec845c)
  - [ ] Guardar las secuencias que hayas hecho antes y poder cargarlas en tiempo real
  - [ ] Cargarlas sincronizadas
- [ ] [INTERFAZ Y TERMINADO FINAL](https://www.notion.so/2f6bd598080780b680e5d2abfcd1f758)
  - [ ] 
# **MATERIAL NECESARIO**

1. Arduino mega
1. Pantalla I2C 20x4
1. 16 Interruptores mutes
1. 16 potenciometros
1. botones de control
1. Mega Proto Shield
1. Cables Dupont Macho-Macho
1. Cables Dupont Macho-Hembra
1. Resistencias para los botones
# SOFTWARE

1. [LoopMidi](https://www.tobias-erichsen.de/software/loopmidi.html)
1. [HairLessMidi](https://projectgus.github.io/hairless-midiserial/)
1. [MIDI-OX](http://www.midiox.com/)
# ARDUINOS

Arduino mega 2560 [https://es.aliexpress.com/item/1005006332489610.html?spm=a2g0o.productlist.main.16.74566db7ZHqOpr&utparam-url=scene%3Asearch%7Cquery_from%3Apc_back_same_best%7Cx_object_id%3A1005006332489610%7C_p_origin_prod%3A&algo_pvid=1baab506-43cd-4bca-a47a-dc72475b8f29&algo_exp_id=1baab506-43cd-4bca-a47a-dc72475b8f29&pdp_ext_f=%7B%22order%22%3A%2229%22%2C%22fromPage%22%3A%22search%22%7D&pdp_npi=6%40dis%21EUR%2136.04%2115.57%21%21%21290.16%21125.35%21%4021038da617661517526873988ecab8%2112000036791409533%21sea%21ES%216210724025%21X%211%210%21n_tag%3A-29919%3Bd%3Abda35c35%3Bm03_new_user%3A-29895%3BpisId%3A5000000196522867](https://es.aliexpress.com/item/1005006332489610.html?spm=a2g0o.productlist.main.16.74566db7ZHqOpr&utparam-url=scene%3Asearch%7Cquery_from%3Apc_back_same_best%7Cx_object_id%3A1005006332489610%7C_p_origin_prod%3A&algo_pvid=1baab506-43cd-4bca-a47a-dc72475b8f29&algo_exp_id=1baab506-43cd-4bca-a47a-dc72475b8f29&pdp_ext_f=%7B%22order%22%3A%2229%22%2C%22fromPage%22%3A%22search%22%7D&pdp_npi=6%40dis%21EUR%2136.04%2115.57%21%21%21290.16%21125.35%21%4021038da617661517526873988ecab8%2112000036791409533%21sea%21ES%216210724025%21X%211%210%21n_tag%3A-29919%3Bd%3Abda35c35%3Bm03_new_user%3A-29895%3BpisId%3A5000000196522867)

* pines digitales máximos: 52
* pines analógicos máximos: 16
# PLANIFICACIÓN





# BIBLIOGRAFÍA

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













<!-- END_NOTION -->

esto es un pie de pagina
