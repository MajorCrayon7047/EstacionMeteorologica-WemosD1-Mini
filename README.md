# EstacionMeteorologica-WemosD1-Mini

Una estacion meteorologica hecha por mi con la idea de ser accedida via web con la utilizacion de dispositivos embebidos como placas de desarrollo Wemos D1 Mini con sus respectivos sensores orientados a la meteorologia. Esta funciona con una parte de recoleccion de datos que dispone de un Wemos D1 con un sensor DHT11 (para medir humedad) y un sensor BMP180 (para medir presion atmosferica y temperatura) y para transmitir los datos se conecta via protocolo MQTT a un servidor con NODE-RED. El servidor de NODE-RED mencionado anteriormente tambien se encarga de administrar las peticiones https para mantener levantada una pagina web que muestra los datos enviados por la estacion. Por ultimo todos los datos son guardados en MongoDB (una base de datos no relacional).


Detalles a tener en cuenta:
  - El archivo que representa todos los nodos de NODE-RED esta incluido en el repositorio como "flow.json", es importante editar el JSON de configuracion de NODE-RED con la linea que se lee en "config.json", esto le permite a node-red utilizar los recursos de la carpeta especificada para por ejemplo mostrarlos en una pagina web.
  - Hay que configurar el broker de MQTT para aceptar dispositivos desconocidos, el cambio que tenga que hacer dependera segun el broker que utilize, si usa mosquitto tiene que ir al archivo de configuracion "mosquitto.conf" y poner la siguiente linea "allow_anonymous true".
  - No se olvode de editar el codigo del dispositivo ESP que utilize con las credenciales de la conexion a WI-FI y del broker MQTT.
