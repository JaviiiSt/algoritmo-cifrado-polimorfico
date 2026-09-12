# Algoritmo de Cifrado Pólimorfico

## Descripción del Proyecto
Este repositorio contiene el código comentado y la documentación para un sistema de comunicación cifrada entre dos nodos (Emisor y Receptor) utilizando microcontroladores ESP32 en el simulador Wokwi. El proyecto implementa un motor criptográfico polimórfico y un protocolo de máquina de estados para la transmisión de datos a través de un broker MQTT público.

## Tecnologías utilizadas
* ESP32
* C/C++
* Wowki
* MQTT
* Criptografía
* Maquina de estados

## Equipo de Trabajo
* **Integrante 1:** Diego Jese Castellanos Valladares CV240473
* **Integrante 2:** Javier Stanley Escobar Portillo EP240471
* **Integrante 3:** Aaron Jaziel Pacas Ramos PR252213
* **Integrante 4:** Marco Alejandro Rivas Ochoa RO240441

**Universidad Don Bosco**  
**Materia:** Diseño de sistemas de seguridad en redes de datos  
**Docente:** Mg. Denis Alfredo Altuve Santamaria

## Simulaciones en Wokwi
A continuación se presentan los enlaces directos a las simulaciones duales en vivo:
* **Nodo A (Emisor):** https://wokwi.com/projects/474900900195298305
* **Nodo B (Receptor):** https://wokwi.com/projects/474900922194428929

## Estructura del Repositorio
* `/Nodo_A_Emisor`: Contiene `sketch.ino`, `crypto.h` y `protocol.h` del nodo emisor.
* `/Nodo_B_Receptor`: Contiene `sketch.ino`, `crypto.h` y `protocol.h` del nodo receptor.
* `/Documentacion`: Contiene los diagramas de flujo y el documento final de investigación.

## Instrucciones de Uso
1. Abrir en pestañas independientes los enlaces de Wokwi para el Nodo Emisor (Nodo A) y el Nodo Receptor (Nodo B).
2. Iniciar primero el nodo receptor: Presionar el botón de inicio (Play) en el Receptor y esperar a que la consola confirme la conexión WiFi y la suscripción al tópico MQTT.
3. Iniciar el nodo emisor: Presionar el botón de inicio (Play) en el Emisor para comenzar la transmisión de la secuencia del protocolo.
4. Observar cómo el emisor cifra el contenido y envía las tramas serializadas, mientras el receptor deserializa y descifra el texto en claro en tiempo real.
