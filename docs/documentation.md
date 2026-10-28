# HITO 1: ALPHA

DESARROLLO COLABORATIVO DE APLICACIONES  
CURSO 2027/28

# “ PAYO BROS ”

---

## ÍNDICE

1. [HITO 1: ALPHA](#hito-1-alpha)
2. [GAME DESIGN DOCUMENT](#game-design-document)
   * [CONCEPTO](#concepto)
   * [MECÁNICA DEL JUEGO](#mecánica-del-juego)
   * [MOVIMIENTO](#movimiento)
   * [COMBATE](#combate)
   * [PROGRESIÓN Y RECOMPENSAS](#progresión-y-recompensas)
   * [FÍSICAS Y ENTORNO](#físicas-y-entorno)
3. [ESTRATEGIA DE GESTIÓN DE RAMAS](#estrategia-de-gestión-de-ramas)
   * [GITHUB FLOW](#github-flow)
4. [SISTEMA DE BUGTRACKING](#sistema-de-bugtracking)
   * [GITHUB ISSUES](#github-issues)

---

# GAME DESIGN DOCUMENT

## CONCEPTO

Payo Bros es un juego de plataformas y acción 2D de desplazamiento lateral, para uno jugador o dos jugadores, en el que (una técnica de mantenimiento) cruza cinco distritos de una ciudad apagada para reactivar su Corazón de Energía. En cada nivel los (hermanos payo) tendrán que enfrentarse a los obstáculos de la calle, trepar por cuerdas y barandillas y combatir a distancia o cuerpo a cuerpo contra los distintos enemigos de la calle. Todo este recorrido con un único fin, acabar con (Jefe final) para recuperar su (energía).

El juego ha sido desarrollado en C++ usando la biblioteca Raylib, una biblioteca de software de código abierto y multiplataforma, creada principalmente para facilitar el desarrollo de videojuegos y aplicaciones gráficas. Se ha usado GitHub Flow como estrategía de gestión de ramas, añadiendo por nuestra parte una rama intermediaria (develop) entre main y las ramas auxiliares. La decisión de usar está estrategía reside en la experiencia previa del equipo con está estrategía, la facilidad de uso y la constante revisión de las ramas antes de fusionarse con la rama principal, en nuestro caso, con la rama develop.

## MECÁNICA DEL JUEGO

El videojuego se basa en cuatro principales mecánicas: movimiento, combate, progresión y físicas. Combinando estas cuatro mecánicas el jugador debe ser capaz de superar obstáculos, enfrentarse a enemigos y avanzar por los distintos niveles hasta alcanzar el objetivo final.

## MOVIMIENTO

El movimiento permite al jugador desplazarse por el nivel, saltar entre plataformas y evitar los obstáculos que encuentra durante la partida. El personaje se controla mediante el teclado, con un sistema de desplazamiento sencillo que facilita recorrer el nivel y ajustar la posición antes de realizar un salto o enfrentarse a un enemigo. 

El sistema de movimiento está condicionado por la gravedad y por las superficies del escenario. El jugador solo puede saltar cuando está apoyado sobre una plataforma, por lo que debe calcular sus movimientos para evitar caer al vacío. En los niveles existirán precipicios donde si el personaje cae fuera de los límites del escenario, pierde la partida.

En el modo individual, el jugador utiliza las teclas A y D para desplazarse y ESPACIO o W para saltar. En el modo cooperativo, el segundo jugador utiliza las flechas izquierda y derecha para moverse y la flecha arriba para saltar. Además, en un futuro se implementarán las funcionalidades de trepar y pasar por pasadizos. Con estás funcionalidades los jugadores tendrán más alternativas para explorar los niveles y superar obstáculos.

## COMBATE

El sistema de combate se basa actualmente en el uso de armas de fuego a distancia. El modo de disparó será en línea recta, el proyectil saldrá disparado hacía donde el jugador esté mirando. Los proyectiles se desplazan por el escenario hasta alcanzar un objetivo o salir de los límites de la pantalla. Cuando uno de ellos impacta contra un enemigo, este es eliminado. Sin embargo, el combate también implica un riesgo, ya que el contacto físico con un enemigo que continúa vivo provoca la muerte inmediata del jugador.

Como ampliación del diseño, se contempla incorporar combate cuerpo a cuerpo mediante armas blancas, así como diferentes armas de fuego que permitan variar la forma de enfrentarse a los enemigos. También se plantea que los adversarios puedan atacar con armas, haciendo necesario disponer de mecanismos defensivos y de una mayor variedad de estrategias durante los enfrentamientos. Estas características permitirían que el combate sea más complicado y que según el tipo de enemigo, el jugador tenga que adaptar su forma de jugar.

## PROGRESIÓN Y RECOMPENSAS

La progresión del juego está planteada para que el jugador avance a través de distintos niveles, superando los desafíos de cada escenario hasta alcanzar el objetivo final: recuperar un preciado objeto que los enemigos le arrebataron de las manos. En la versión actual, la experiencia se centra en completar un único nivel y derrotar al enemigo situado en la segunda plataforma. 

El diseño general contempla que los niveles incluyan diferentes obstáculos y oleadas de enemigos, de manera que el jugador tenga que superar varios enfrentamientos antes de continuar avanzando. Cada nivel terminaría con un jefe que supondría un desafío mayor que los enemigos normales, donde en cada nivel los jefes serán de mayor dificultad.

Además, para un futuro se quiere incorporar un sistema de recompensas que permita conseguir objetos y mejorar el equipamiento durante la aventura. Los cofres repartidos por los escenarios podrían proporcionar distintos objetos, mientras que las armaduras podrían encontrarse en el suelo o ser obtenidas de los enemigos derrotados. Estos elementos ayudarán a dar mayor importancia a la exploración y a las decisiones del jugador, ya que encontrar equipamiento útil podría facilitar los enfrentamientos posteriores. 

## FÍSICAS Y ENTORNO

El sistema de físicas determina cómo se comporta el personaje durante el movimiento y cómo interactúa con los elementos del escenario. La gravedad hace que el jugador descienda después de saltar, mientras que la detección de plataformas permite reconocer cuándo aterriza sobre una superficie y puede volver a saltar.

El entorno actual está compuesto por plataformas y un precipicio que separa dos zonas del escenario. En está primera versión están los elementos básicos, pero en futuro con una distribución técnica, la disposición de estos elementos obligará al jugador a prestar atención al terreno y calcular sus movimientos para evitar caídas. Además, el personaje no puede abandonar la pantalla por los laterales y tampoco puede atravesar el límite superior. Estas restricciones mantienen al jugador dentro del área de juego y ayudan a definir los límites del escenario.

Las colisiones también desempeñan un papel importante en el funcionamiento general. Permiten detectar los aterrizajes sobre las plataformas, los impactos de los proyectiles contra los enemigos y el contacto peligroso entre el protagonista y un adversario.

---

# ESTRATEGIA DE GESTIÓN DE RAMAS

## GITHUB FLOW

Entre todos los miembros del equipo hemos decido usar la estrategia de gestión de ramas GitHub Flow con una pequeña variante. Sabemos que GitHub Flow es una estrategia exigente: despliegues continuos, ramas cortas, *Pull Requests* y un historial donde se puede seguir la evolución del proyecto.

La variante consiste en añadir una rama develop, cómo hace Git Flow. En nuestro caso preferimos que las ramas de funcionalidad se integren primero en *develop*, donde se prueban en conjunto, y que solo se fusione con main cuando el estado sea estable. De este modo, organizamos de manera limpia nuestro proyecto (ramas cortas, pr, tags), prevenimos conflictos con la rama *main* y tenemos una versión estable del juego con el uso de CI/CD.

---

# SISTEMA DE BUGTRACKING

## GITHUB ISSUES

Para la gestión de bugs hemos decidido usar *GitHub Issues* como sistema centralizado de bugtracking, aprovechando su integración nativa con el repositorio y el flujo de Pull Requests. Para la fase de triaje, hemos implementado un script en Python que utiliza Jev para analizar automáticamente cada issue creada y clasificarla según su severidad, prioridady ámbito del proyecto afectado. Jev se encarga de procesar el contenido de la issue y asignarla automáticamente al miembro del equipo responsable del módulo correspondiente, aplicándole las etiquetas pertinentes y eliminando cuellos de botella en la organización. La trazabilidad queda asegurada al vincular cada fix con su issue correspondiente, cerrándola automáticamente tras el merge a main y quedando reflejada en el historial del proyecto.