# Plan — Spec 004: Generador, simulación e informe

Parte de la spec 004 en borrador. Firmas y retornos exactos: `docs/primitivas.md` (sección Spec 004).
Las mediciones que motivan el volumen de datos y el modo con seek están en `docs/arquitectura.md`,
sección "Volumen de datos y costo de acceso".

## Archivos y responsabilidades

| Archivo | Responsabilidad | Usa |
| --- | --- | --- |
| `dominio/posteo.h` / `.c` | `tPosteo`, `tLectorPosteo`, los dos modos de lectura (`leerPosteoPorId`, `leerPosteoConSeek`), escritura, `cmpIdPosteo` | `comun.h` |
| `dominio/usuario.h` / `.c` | `tUsuario`, lectura secuencial y escritura | `comun.h` |
| `sim/generador.h` / `.c` | `generarPosteos`, `generarLote` con estrategia (puntero a función) y las tres estrategias | `posteo`, `usuario` |
| `sim/metricas.h` / `.c` | `tMetricas`, contadores, tiempo y promedios | — |
| `sim/simulador.h` / `.c` | `simular`: calienta el archivo, recorre el lote, pide cada posteo a la caché o al archivo con el lector recibido, mide | `cache`, `posteo`, `usuario`, `metricas` |
| `sim/reporte.h` / `.c` | Tabla por pantalla y CSV, con columna de modo de acceso | `metricas`, `cache` |
| `apps/main_generador.c` | Lee parámetros de `argv`, `srand` una vez, genera posteos y los tres lotes | `generador` |
| `apps/main_simulador.c` | Recorre modos × lotes × variantes × capacidades, llama a `simular`, `reporte` y `vaciarCache` | `simulador`, `reporte`, `cache`, `posteo` |
| `pruebas/pruebas_004.c` | `int pruebas004(void)`: casos de `casos-prueba.md` | todo lo anterior |

Mientras la interfaz (spec 003) no esté, el simulador se prueba con `crearSinCache` y con los stubs de
`posteo` y `usuario` de `docs/primitivas.md`.

## Parámetros

- `generador <semilla> <usuarios> <grupo_concentrado>`: escribe `datos/posteos.dat`,
  `datos/lote_concentrado.dat`, `datos/lote_disperso.dat` y `datos/lote_ordenado.dat`.
- `simulador [directo | seek | ambos]`: sin argumento, `ambos`.
- Valores del informe (27/10/2026): `generador 2026 80000 300` (1,6 millones de pedidos, lotes de
  ~7,7 MB), capacidades 10, 100, 1000 y 5000. Con 50000 usuarios el lote ordenado sin caché duraba
  0,72 s y no cumplía RNF-04.

## Cómo resuelve cada parte (sin código)

- **Reproducibilidad:** `srand(semilla)` una sola vez en `main_generador`, antes de generar todo, siempre
  en el mismo orden (posteos, concentrado, disperso, ordenado).
- **Estrategias:** `generarLote` no sabe qué lote genera; recibe la estrategia y un contexto. El
  concentrado recibe `G`. El ordenado recibe la cantidad de usuarios y calcula el id del pedido
  j = nUsuario × 20 + i como ⌊j × 50000 / (usuarios × 20)⌋, **en `double`**: con 50000 usuarios,
  j × 50000 llega a 5 × 10^10 y desborda el `long` de 32 bits de MinGW (y C89 no tiene `long long`).
- **Azar en todo el rango:** `aleatorioEntre` combina dos `rand()` porque `RAND_MAX` de MinGW es 32767.
- **Modo de acceso:** `leerPosteoConSeek` mueve el puntero al final (`fseek(fp, 0L, SEEK_END)`) y después
  hace exactamente lo mismo que `leerPosteoPorId`. `main_simulador` elige cuál pasar a `simular` como
  `tLectorPosteo`; el simulador solo llama al puntero en cada fallo.
- **Calentamiento:** dentro de `simular`, después de abrir los archivos y antes del primer `clock()`, se
  lee el archivo de posteos de punta a punta con `fread` y se vuelve al principio. No toca las métricas.
- **Simulación:** inicia métricas, toma `clock()`, por cada usuario y cada id llama a `obtenerPosteo`
  (caché → si falla, lector + agregar), toma `clock()`, fija el tiempo y cierra. Recibe la `tCache` ya
  creada: no sabe qué variante es.
- **Altura:** `main_simulador` la consulta con `alturaIndiceCache` después de `simular` y antes de
  `vaciarCache`; `reporte` muestra "-" si es -1.

## Decisiones (y alternativa descartada)

| Decisión | Por qué | Alternativa descartada |
| --- | --- | --- |
| Estrategia de feed como puntero a función | Un lote nuevo (por ejemplo en la defensa) es una función más | Un `switch` por tipo de lote dentro de `generarLote` |
| Modo de acceso como puntero a función (`tLectorPosteo`) | El simulador no cambia y no tiene `if` (RNF-05); otro modo es una función más | Un parámetro `int modo` y un `if` en cada lectura |
| Seek simulado moviendo el puntero al final | Recomendación de la cátedra; en la prueba duplicó el costo por lectura (~630 → ~1350 ns) sin código artificial | Un bucle de espera fijo por lectura: el costo sería arbitrario y más difícil de justificar |
| Correr y reportar los dos modos | Muestra cómo las conclusiones dependen del costo de leer el archivo, y evita que el seek parezca un truco | Reportar solo el modo con seek |
| Calentamiento en cada corrida | Ninguna variante paga la lectura real del disco por haber corrido primero | Descartar la primera corrida: depende del orden y gasta una corrida entera |
| Lote ordenado proporcional (ids parejos en todo el rango) | Funciona con cualquier cantidad de usuarios; con un millón de pedidos cada id se repite 20 veces seguidas | Salto aleatorio entre ids: con muchos usuarios el salto tendría que ser menor que 1 y el lote se clava en 49999 |
| `simular` recibe la caché ya creada | El simulador no depende de cómo se crea cada variante (RNF-02) | Que `simular` reciba un "tipo" y cree la caché |
| Sin caché, una sola corrida por lote y modo | Su resultado no depende de la capacidad | Repetirla con cada capacidad |
| CSV con `;` | Excel en español abre bien los decimales con coma | Coma como separador |
| Lote mini escrito a mano para las pruebas | Los aciertos y fallos esperados se calculan de antemano | Probar con lotes generados: los resultados dependen del azar |

## Estrategia de pruebas

- Primero `posteo` y `usuario` (09, 10, 27), después métricas (11, 12), generador (01 a 08, 28) y
  simulación con el lote mini (13 a 22, 29 a 31).
- La simulación con la Parte 1 y la Parte 2 se prueba recién cuando las specs 001 a 003 estén en OK; antes
  alcanza con "sin caché".
- Las pruebas nunca comparan segundos absolutos (dependen de la máquina): verifican que los promedios
  salen del total y que el modo no cambia los contadores. La duración mínima (RNF-04) se comprueba con el
  CSV real en la máquina del informe (CP-004-33).

## Trazabilidad

| RF | Dónde se resuelve | Casos |
| --- | --- | --- |
| RF-01 | `generarPosteos` | 01 |
| RF-02 | `srand` único en `main_generador` | 02 |
| RF-03 a RF-05 | `generarLote` + estrategias + `aleatorioEntre` | 03, 04, 05 |
| RF-06 | `feedOrdenado` (cálculo en `double`) | 06, 28 |
| RF-07 | Validación en `generarLote` y `main_generador` | 07, 08 |
| RF-08 | `simular` + `obtenerPosteo` + lector | 09, 10, 13 |
| RF-09 | `simular` sin salida entre los `clock()` | 21 |
| RF-10 | `metricas` | 11, 12, 19 |
| RF-11 | Contadores en `obtenerPosteo` | 13 a 15, 24 |
| RF-12 | `alturaIndiceCache` + `reporte` | 17, 24 |
| RF-13 | `main_simulador` + `reporte` | 23, 32 |
| RF-14 | Determinismo de las dos LRU y de los dos modos | 16, 18, 24, 29 |
| RF-15 | Apertura de archivos en `simular` y `main_simulador` | 20, 21 |
| RF-16 | Informe | 25 |
| RF-17 | Calentamiento en `simular` | 13, 30 |
| RF-18 | `tLectorPosteo`, `leerPosteoConSeek`, columna de modo en `reporte` | 23, 27, 29, 31, 32 |
