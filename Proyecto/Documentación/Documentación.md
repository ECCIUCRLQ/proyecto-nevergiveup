# Documentación 

## Especificación

Este dispositivo se conecta vía PCI-Express, en donde se simula, por medio de Simics, el hardware para capturar, transformar y filtrar fotos o imágenes y enviar el resultado de la imagen filtrada a un entorno virtual que usa Ubuntu como sistema operativo. La imagen de entrada es un archivo bmp (bitmap image). A la imagen se le pueden aplicar varios filtros. Finalmente, el resultado de aplicar el filtro se almacena en el dispositivo junto con otros datos como fecha de la captura, tamaño en bytes, entre otros. Los datos que se almacenan son enviados a una computadora que usa Ubuntu y desde ahí se puede visualizar la imagen. 


### Filtros a aplicar
1. Escala de grises
2. Infrarojo
3. Verde
4. Azul

### Ejemplo de resultados

![Filtros](https://github.com/ECCIUCRLQ/proyecto-nevergiveup/blob/main/Proyecto/Img/filtros.png)

Descripción: La imagen central es la original y a sus extremos están sus respectivos filtros.

## Archivos BMP

Según Estefanía Mac (2019) es conocido como archivo de mapa de bits, también llamado como archivo de imagen Bitmap, el cual es un formato de imagen que está compuesto por pixeles dispuesto en forma de matriz que constituyen diferentes profundidades de color determinadas por los encabezados de archivo, cada pixel puede tener un tamaño de 1, 4, 8, 15, 24, 32 o 64 bits, entre ellas la más común es de 24 bits por pixel.

Este formato produce imágenes detallas en alta calidad, imagen bidimensional y buenos perfiles de color. No obstante, no se comprimen de forma óptima y suelen ser de gran tamaño, por lo que no es recomendable su uso en páginas web, ya que no son los más adecuados para trasferencia de datos vía web.

### Estándar de una imagen en formato BMP
Los datos son almacenados en little-endian

| Bytes | Información encabezado |
| ------------- | ------------- |
| 0, 1  | Tipo de fichero "BM"  |
| 2, 3, 4, 5  | Tamaño del archivo  |
| 6,7 | Reservado |
| 8,9 | Reservado |
| 10, 11, 12, 13 | Inicio de los datos de la imagen |
| 14, 15, 16, 17 | Tamaño de la cabecera del bitmap |
| 18, 19, 20, 21 | Anchura (píxels) |
| 22, 23, 24, 25 | Altura (píxels) |
| 26, 27 | Número de planos |
| 28, 29 | Tamaño de cada punto |
| 30, 31, 32, 33 | Compresión (0=no comprimido) |
| 34, 35, 36, 37 | Tamaño de la imagen |
| 38, 39, 40, 41 | Resolución horizontal |
| 42, 43, 44, 45 | Resolución vertical |
| 46, 47, 48, 49 | Tamaño de la tabla de color |
| 50, 51, 52, 53 | Contador de colores importantes |

### Procesamiento de una imagen en formato BMP

Según el blog Tips para estudiantes de Sistemas Computacionales (2012), para el procesamiento de imágenes en el formato BMP,  se recibe la dirección de memoria de la imagen que ya debe estar cargada en el sistema operativo. Después es necesario almacenar el archivo en el buffer,  y para esto hay que asegurarse que el archivo BMP es válido y que contiene la información correspondiente, esto se realiza mediante una lectura del archivo, ya que este consta de un encabezado de 54 bytes que contiene toda la información del BMP. Una vez almacenado el archivo en el buffer, este se utiliza para modificar la paleta de colores. Es importante recordar que los colores en un archivo BMP se guardan como BGR y no como RGB. Para cargar la imagen se hace una lectura línea por línea y se despliega comenzando con la línea inferior hasta la superior. La esquina superior izquierda de la imagen se pinta en la esquina superior de la pantalla. 

En cuanto a La memoria de video, la cual es un tipo de memoria RAM que utiliza el controlador gráfico para poder manejar toda la información visual que le envía la CPU del sistema, y que es un arreglo bidimensional de bytes que se pueden manipular individualmente. Cada byte representa un pixel en la pantalla y contiene el color del pixel que se va a pintar en esa posición, de esta manera, al cargar todos los bytes se logra obtener la imagen deseada. En caso de querer aplicar un filtro, se carga la imagen con diferentes valores por pixel y así obtener diferentes colores aplicados en la misma imagen (filtros). 





## Protocolo

Los protocolos son muy utilizados por las personas. En informática al igual que en muchas otras ámbitos un protocolo se puede definir como un conjunto formal de estándares y normas. Según (Imagar, 2021) estos pueden llegar a regir tanto el formato como el control de la interacción entre los distintos dispositivos dentro de una red o sistema de comunicación entre otras. Los protocolos tienen varias funciones algunas de las mencionados por (Imagar, 2021) son: permiten la interoperabilidad, sirve además para regular el control de flujo, sirve para administrar la velocidad de transmisión de los datos entre dos dispositivos, administrar congestiones y también administra la verificación de errores, entre otras funciones.

Un claro ejemplo de los protocolos importantes y comunes son los mencionados por (Jithin, 2016) de red los cuáles utilizamos todos los días como puede ser el protocolo de internet (IP), el cual es el protocolo principal en el conjunto de protocolos de Internet, por otro lado tenemos otros protocolos igual de comunes como lo puede ser el protocolo de transferencia de hipertexto (HTTP), el  cuál es la base de la comunicación de datos para la World Wide Web.

Como menciona (Chai y Irei, sf) para enviar y recibir información con éxito, los dispositivos en ambos lados de un intercambio de comunicación deben aceptar y seguir las convenciones de protocolo. Para nuestro proyecto es importante definir un protocolo que permita la implementación del controlador de manera satisfactoria, esto se llevará acabo mediante una negociación con el resto de equipos.

##  Drivers
Para este proyecto es necesario conocer las bases de como crear un driver, entender su funcionamiento y las funcionalidades que brinda. Esto con la finalidad de enlazar el programa con simics y de esta manera poder obtener los resultados esperados.

Un controlador o Driver es el software que controla un dispositivo en una PC y algunos otros aparatos tecnológicos. 
Por ejemplo, una tarjeta de vídeo, de sonido, un mouse, una cámara, impresoras entre muchas otros dispositivos.
Los controladores actúan como “puentes” entre las aplicaciones y los dispositivos, encargándose de que ambos interactúen. 
Para que esta interacción sea posible es necesario crear un software que logre acoplar la PC con el dispositivo o programa en ejecución, de esta manera una vez acoplados los programas se podrá realizar la tarea deseada. 

Como primer avance en la creación de dicho software, se creara un ejemplo de driver en clases lo cual servirá como base para el diseño y creación de nuestro propio driver, el cual con los conocimientos adquiridos en clases y con la recopilacione de informacion podra ser diseñado y utilizado en el proyecto. 

## Diseño del producto final

El diseño del producto final consta de las sigueintes partes:
1. Un dispositivo virtualizado que modela la simulación del hardware para capturar, transformar y filtrar fotografías o imágenes y enviar el resultado de la imagen filtrada a un entorno virtual que usa Ubuntu como sistema operativo
2. Un producto de sofware dependiente del sistema operativo, conocido como driver o controlador que permite que cualquier proceso en el sistema operativo tenga acceso al dispositivo
3. Un programa de usuario que permite verificar el funcionamiento del dispositivo y por consiguiente del driver o controlador
4. Pruebas de validación que demuestran el cumplimiento de los requisitos mínimos para el funcionamiento del dispositivo
5. Un protocolo que permite la comunicación entre el dispositivo y el driver o controlador

![Diseño final](https://github.com/ECCIUCRLQ/proyecto-nevergiveup/blob/main/Proyecto/Img/Diagrama.png)

## Lista de temas, conocimientos o habilidades necesarios para lograr el proyecto
- Entender qué es un driver y cómo funciona
- Saber cómo manipular los bits de una imagen
- Conocer las características del formato bmp
- Aprender a programar en ensamblador
- Aprender a utilizar Simics
- Conocer qué es un protocolo y cómo implementarlo
- Conocer las instrucciones de x86-64 y AVX2

## Lista de Tareas
| Tarea | Encargado | Duración |
| ------------- | ------------- | ------------- |
| Especificación del dispositivo | Todos los miembros  | 1 semana |
| Implementar Driver | James Araya | 3 semanas |
| Implementar Programa de Usuario | David Obando | 3 semanas |
| Implementar Protocolo | Sebastián Rodríguez | 3 semanas |
| Realizar Pruebas de Validación | Lizeth Corrales | 1 semana |

## Código y bibliografía de terceros

- [Imagar Solutions Company. (2021). ¿Qué es el protocolo en informática?](https://www.imagar.com/blog-desarrollo-web/que-es-el-protocolo-en-informatica/)

- [Jithin. Interserver. (2016). Protocolos de red comunes](https://www.interserver.net/tips/kb/common-network-protocols-ports/)

- [Chai, Wesley y Irei, Alissa. TechTarget. (sf). Network Protocol](https://searchnetworking.techtarget.com/definition/protocol)

- [Mac, E (2019) ¿Cuál es el significado de BMP?. Technolandia](https://techlandia.com/significado-bmp-hechos_103910/)

- [Tabla: Estándar de una imagen en formato BMP](https://www.youtube.com/watch?v=eRtL6TsLKfU)

- [Tips para estudiantes de Sistemas Computacionales (2012)](https://tipsparaisc.blogspot.com/2012/05/ensamblador-8086-desplegar-una-imagen.html)


## Programadores:

James Araya Rodríguez james.araya@ucr.ac.cr

Lizeth Corrales Cortés lizeth.corrales@ucr.ac.cr

David Obando Cortés dailyn.obando@ucr.ac.cr

Sebastián Rodríguez Tencio sebastian.rodrigueztencio@ucr.ac.cr

## License

Copyrigth 2022. James Araya Rodríguez james.araya@ucr.ac.cr, Lizeth Corrales Cortés lizeth.corrales@ucr.ac.cr, David Obando Cortés dailyn.obando@ucr.ac.cr, Sebastián Rodríguez Tencio sebastian.rodrigueztencio@ucr.ac.cr, Universidad de Costa Rica. CC BY 4.0
