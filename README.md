[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-718a45dd9cf7e7f842a935f5ebbe5719a5e09af4491e668f4dbf3b35d5cca122.svg)](https://classroom.github.com/online_ide?assignment_repo_id=12703277&assignment_repo_type=AssignmentRepo)

## Ejemplo base de Jack en C++

Este ejemplo construye una aplicación muy sencilla de "pass-through"
usando Jack, como punto de partida para los proyectos y tareas del
curso.

Esta versión permite además, recibir además una lista de archivos
.wav, que se ejecutan uno tras otro, reemplazando la entrada de
micrófono en tanto hayan datos de los archivos disponibles.  Una vez
que todos los archivos terminan de ejecutarse, regresa al modo
"pass-through".

En esta versión para la tarea 4, el pass-through utiliza una forma muy
poco eficiente en el dominio de la frecuencia, solo como ejemplo de
cómo utilizar la clase freq_filter.

## Dependencias

Requiere C++ en su estándar del 2020 (g++ 12, clang 14).

En derivados de debian (ubuntu, etc):

     sudo apt install jackd2 libjack-jackd2-dev qjackctl build-essential meson ninja-build jack-tools libsndfile1-dev libsndfile1 libboost-all-dev libfftw3-dev libfftw3-bin libfftw3-single3 libgsl-dev

Jack requiere que su usuario pertenezca al grupo audio, o de otro modo
no tendrá privilegios para el procesamiento demandante en tiempo
real...

     sudo usermod -aG audio <su usuario>

## Construcción

Para construir los ejemplos la primera vez utilice

     meson setup builddir
     cd buildir
     ninja

Si requiere reconstruir todo, utilice

     meson setup --wipe builddir
     cd buildir
     ninja

## Latencia y tamaño de bloque

Para reducir la latencia por medio del tamaño del "periodo" (esto es,
el número de "frames" que cada ciclo de procesamiento recibe, en
QjackCtl, en Settings, se indica en Frames/Period.  Eso es un
parámetro del servidor de Jack y no lo puede controlar la aplicación
como tal.

## Respuesta en frecuecia de los filtros

El archivo de octave *get_impulse_response.m*, permite obtener la respuesta al impulso de los filtros IIR a partir de los coeficientes de ganancia, numerador y denominador obtenidos con la herramienta **filterDesign** de Matlab. Además, muestra las gráficas de Amplitud y Fase de respuesta en frecuencia de cada filtro, donde cada figura incluye en el nombre de la misma, el filtro al que pertenece la respuesta mostrada.

La respuesta al impulso de los filtros se almacena en archivos de extension 'txt' que contienen las muestras desde $n=0$ hasta $n=1023$, los cuales se cargan en el programa de C++ al momento de iniciar para obtener la respuesta en frecuencia.

## Ejecución

Para ejecutar el programa una vez compilado, utilice

```
 ./proy2
```

Además, puede ejecutar archivos de audio en formato .wav para reproducirlo al inicio de la ejecución:

```
 ./proy2 -f archivo.wav
```

## Metodo de solapamiento para filtros en frecuencia

Para realizar el cambio entre dominios y evitar que las muestras sufran un traslape, se utiliza el método de solapamiento y almacenamiento.

## Métodos de PDS
Se puede utilizar todos las funcionalidades implementadas en cascada.
### Reverberador

Se implementaron los siguientes reverberadores:

#### Reverberador simple

Este utiliza el modelo facilitado por el profesor que sigue la ecuación: y[n] = x[n] + alpha*y[n-tau]

Para activar este reverberador en la interfaz gráfica se debe presionar la casilla con el nombre de `Reverberación simple`. Una vez activado, se puede configurar el parámetro `alpha` usando la barra con el mismo nombre, también se puede cambiar el valor de `tau` igualmente usando la barra nombrada de la misma forma.

#### Reverberador filtrado

Este segundo reverberador se utiliza al marcar la casilla `Reverberación filtrada`. A diferencia del anterior, este presenta un filtro paso bajos en serie con el retardador de la señal de salida, dando un efecto de `filtrado de eco`. Este se puede configurar de la misma forma que el anterior, además de poder variar la posición del polo del filtro, variando así la frecuencia de corte.

### Filtro peine para eliminar ruido eléctrico

El filtro peine rechaza las frecuencias multiplos de 60Hz, para activar o desactivarlo se utliza el check box the filtro peine.

El filtro de peine se implementa en el tiempo y consiste de un filtro IRR, con polos cerca de los ceros del filtro.

La ecuación completa imeplementa es la siguiente: y[n] = K \cdot(x[n]-x[n-L]) +r\cdot y[n-L] 


### Ecualizador

La ecualización de audio se activa al marcar el checkbox llamado "Ecualizador", el usuario dispone de 20 sliders cuyo valor se puede modificar desde 0 dB hasta -21 dB. Cada slider se asocia a una frecuencia en concreto en el rango de 0 Hz hasta 24 kHz equivalente a la máxima frecuencia representable Fs/2 para bloques de entrada de 1024 muestras. Estas frecuencias se calculan con el archivo *get_frecuencies.m* en octave.



Para obtener la respuesta en magnitud del ecualizador, se utiliza una función de interpolación de tipo Akima que calcula los valores intermedios entre cada muestra modificada por los sliders. En cuanto al cálculo de la fase, se asume fase nula por lo que la parte imaginaria se mantiene completamente en 0.



Esto resulta en que el filtro no es causal por lo que se necesitan muestras futuras para obtener una respuesta estable por lo que se utiliza un bloque de entrada de retraso para obtener estas muestras futuras. 

