### Preguntas 1 y 2

Medido con 8 núcleos:

COMANDO                         REAL    USR     SYS
make clean && time make -j1     6.61    6.05    0.54
make clean && time make -j2     3.84    6.62    0.55
make clean && time make -j4     2.75    7.57    0.61
make clean && time make -j8     2.26    9.84    0.71
make clean && time make -j16    2.07    9.58    0.75

A partir de N=4 ya la mejora no supone una mejora sustancial, no se han producido fallos en la compilación.

### Pregunta 3
COMANDO                                        REAL    USR     SYS
make clean && time make                        6.56    6.04    0.51

ccache --clear
ccache --zero-stats
make clean && time make CXX="ccache g++"       8.48    7.41    1.03

Al limpiar la caché con ccache, se introduce una sobrecarga adicional (aproximadamente un 29 % más de tiempo real). Esto pasa porque que la herramienta debe calcular los hashes de los ficheros fuente, luego comprueba la caché (provocando *cache misses*) y pasa al disco los objetos generados para usarlos más adelantes. El beneficio de ccache no se aprecia en la primera ejecución en frío, sino en compilaciones sucesivas e incrementales.

### Pregunta 4
COMANDO                                     REAL USR SYS    
ccache --clear
ccache --zero-stats
make clean && time make CXX="ccache g++" (1ª)   8.48   7.41   1.03
make clean && time make CXX="ccache g++" (2ª)   0.38   0.19   0.19

Como se puede ver, cuando usamos ccache por 2da vez la mejora es considerablemente notoria.
En la primera ejecución se producen todo misses en la caché, puesto que el almacén de la caché está vacío, por ello se tienen que compilar los archivos 1 a 1 y calcular los hashes y guardar las copias en el almacén de la caché. Para la segunda ejecución se detecta que los hashes no han cambiado, en lugar de misses se producen hits, por lo tanto se reduce el tiempo drásticamente.
