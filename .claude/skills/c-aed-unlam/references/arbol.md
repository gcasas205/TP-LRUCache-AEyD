# TDA Árbol binario (estilo cátedra UNLaM)

Casi todo es **recursivo** y usa el desplazamiento puntero a puntero sobre `izq`/`der`. El
tipo del árbol es un alias del puntero al nodo raíz.

## Estructura

```c
typedef struct sNodoArbol
{
    void *info;
    size_t tamInfo;
    struct sNodoArbol *izq;
    struct sNodoArbol *der;
} tNodoArbol;

typedef tNodoArbol* tArbol;

void crearArbol(tArbol *arbol)
{
    *arbol = NULL;
}
```

## Insertar (ABB, con función de comparación)

`cmp(a, b)` devuelve <0, 0 o >0. Menores a la izquierda, mayores a la derecha.

```c
int insArb(tArbol *arbol, void *info, size_t tamInfo, int cmp(void*, void*))
{
    int c;

    if(!*arbol)                    // caso base: lugar libre, crear el nodo
    {
        tNodoArbol *nue;

        if(!(nue = (tNodoArbol*)malloc(sizeof(tNodoArbol))))
            return ERROR_MALLOC;
        if(!(nue->info = malloc(tamInfo)))
        {
            free(nue);
            return ERROR_MALLOC;
        }
        memcpy(nue->info, info, tamInfo);
        nue->tamInfo = tamInfo;
        nue->izq = NULL;
        nue->der = NULL;
        *arbol = nue;
        return EXITO;
    }

    c = cmp(info, (*arbol)->info);
    if(c < 0)
        return insArb(&(*arbol)->izq, info, tamInfo, cmp);
    if(c > 0)
        return insArb(&(*arbol)->der, info, tamInfo, cmp);
    return EXITO;                  // igual: no se inserta (clave duplicada)
}
```

También vale la **versión iterativa**: un `while(*p)` que baja por `&(*p)->izq` o `&(*p)->der`
según `cmp` hasta que `*p` sea `NULL` (una hoja libre) y ahí crea el nodo. Salvo que el
enunciado pida recursivo, la iterativa se acepta y a muchos les resulta más simple. La clave
duplicada no se inserta (parte de la definición del ABB); si se quisiera actualizar el nodo
existente sería otra función (ej. `insertarOActualizar`).

## Recorridos

Nombres de la cátedra: **Pre-Orden (RID)** raíz-izq-der, **In Orden (IRD)** izq-raíz-der,
**Pos-Orden (IDR)** izq-der-raíz. La versión genérica pasa una acción por puntero a función
(a menudo con el nivel actual). Versión simple que imprime enteros:

```c
void inOrden(tArbol *arbol)
{
    if(*arbol)
    {
        inOrden(&(*arbol)->izq);
        printf("%d ", *(int*)(*arbol)->info);
        inOrden(&(*arbol)->der);
    }
}
```

Pre-Orden imprime antes de bajar; Pos-Orden imprime después de ambos hijos. Las variantes
"inversas" recorren der antes que izq.

## Estadísticas del árbol

```c
size_t alturaArbol(tArbol *arbol)
{
    size_t hi, hd;

    if(!*arbol)
        return 0;
    hi = alturaArbol(&(*arbol)->izq);
    hd = alturaArbol(&(*arbol)->der);
    return 1 + (hi > hd ? hi : hd);
}

size_t cantNodos(tArbol *arbol)
{
    if(!*arbol)
        return 0;
    return 1 + cantNodos(&(*arbol)->izq) + cantNodos(&(*arbol)->der);
}

size_t cantHojas(tArbol *arbol)
{
    if(!*arbol)
        return 0;
    if(!(*arbol)->izq && !(*arbol)->der)   // sin hijos = hoja
        return 1;
    return cantHojas(&(*arbol)->izq) + cantHojas(&(*arbol)->der);
}
```

### Completo, balanceado y AVL

La cátedra distingue tres grados de balanceo:
- **Completo**: para su altura tiene todos los nodos posibles. Se verifica calculando la altura
  y comparando la cantidad de nodos con `2^altura - 1`.
- **Balanceado**: es completo **hasta un nivel menos** que la altura del árbol (no importa cómo
  esté el último nivel).
- **AVL** (balanceado por altura): en **cada** nodo, `|altura_izq - altura_der| <= 1`.

Completo y balanceado se resuelven bien con **una sola función reutilizable** `esCompletoHasta`
(recursiva, va bajando el nivel): para "completo" se la llama con el nivel máximo del árbol;
para "balanceado", con un nivel menos. Devuelve verdadero solo si **ambas** ramas lo son, por
eso se combina con `&&` (no con suma):

```c
// completo hasta n niveles a partir de este nodo
int esCompletoHasta(tArbol *arbol, int n)
{
    if(!*arbol)
        return n < 0;                 // no hay nodo: ok solo si ya pasé el nivel objetivo
    if(n == 0)
        return 1;                     // llegué al nivel pedido con nodo: ok
    return esCompletoHasta(&(*arbol)->izq, n - 1)
        && esCompletoHasta(&(*arbol)->der, n - 1);
}
```

Para AVL, la condición de corte devuelve **1** (un subárbol vacío tiene altura válida) y se
compara `abs(alturaIzq - alturaDer) > 1` en cada nodo, propagando con `&&`. Variantes de conteo
**hasta el nivel N**, **en el nivel N** y **a partir del nivel N**: se pasa el nivel actual
(con función envoltorio que arranca en 0, ver `recursividad.md`) y se compara con N.

Un detalle que suele pedirse en el fundamento: en `esCompleto`, la `altura` (o el nivel) no se
debe pasar "a mano" — se calcula primero con `alturaArbol`, si no la verificación no vale.

## Búsqueda, mínimo y máximo

Búsqueda en ABB: bajar por izq/der según `cmp` (iterativa con `while(*p && cmp(...) != 0)`, o
recursiva con `return` al encontrar). El **mínimo** está bajando siempre por `izq`; el
**máximo**, siempre por `der`, sin importar el nivel.

En la búsqueda, el parámetro del dato suele ser `const void *` porque solo se **lee** el árbol
(no se modifica): el `const` protege lo apuntado, no el puntero de recorrido. En insertar no
lleva `const` porque ahí sí se hace `*p = nue`.

### Buscar por clave con "dato parcial" (esto entra al parcial)

Igual que en lista: se pasa una estructura con solo la **clave** cargada; al encontrar el nodo
se **copia el registro completo en ese mismo puntero** con `memcpy(dato, encontrado->info,
MIN(...))`. El profe lo marcó como patrón que "va de cabeza al parcial".

### Mayor / menor por un campo NO clave

Distinto del máximo/mínimo del ABB: acá se busca el nodo con el mayor (o menor) valor de un
campo que **no** es la clave de orden (ej.: árbol ordenado por legajo, pero se quiere el alumno
con el DNI más grande). No se puede bajar por una rama: hay que recorrer **todo** el árbol
comparando por ese otro campo, arrastrando el mejor hasta el momento (se pasa como valor por
defecto y se actualiza con la recursión por izquierda y por derecha).

## Eliminar un nodo

Con 0 o 1 hijo: se engancha el hijo (o `NULL`) en lugar del nodo y se libera. Caso difícil:
nodo con **dos hijos** → se reemplaza por un sucesor y luego se elimina ese sucesor. Para no
desbalancear, la cátedra elige el sucesor según cuál subárbol es más alto: si pesa el
izquierdo, se toma el **máximo del subárbol izquierdo**; si pesa el derecho, el **mínimo del
subárbol derecho**. Se copia la info del sucesor en el nodo y se elimina el sucesor (que a lo
sumo tiene un hijo).

## Cargar un ABB balanceado desde datos ordenados (por bisección)

Si los datos vienen **ordenados** y se insertan uno por uno, el árbol degenera en una lista.
Para que quede balanceado se carga por **bisección**: tomar el elemento del **medio** como
raíz, y recursivamente el medio de la mitad izquierda y el de la derecha.

```c
// desde un arreglo: li = límite inferior, ls = límite superior (índices)
int cargarOrdenado(tArbol *arbol, void *vec, int li, int ls, size_t tam,
                   int cmp(void*, void*))
{
    int m, r;

    if(li > ls)                                  // corte: universo agotado
        return EXITO;
    m = (li + ls) / 2;                           // el medio es la raíz de este subárbol
    if((r = insArb(arbol, (char*)vec + m * tam, tam, cmp)) != EXITO)
        return r;
    if((r = cargarOrdenado(arbol, vec, li, m - 1, tam, cmp)) != EXITO)  // izquierda
        return r;
    return cargarOrdenado(arbol, vec, m + 1, ls, tam, cmp);             // derecha
}
```

Si el origen es un **archivo binario ordenado**, es lo mismo pero en vez de indexar un arreglo
se hace `fseek` al registro `m` (`m * tam` bytes) y `fread`. Conviene una función envoltorio
que calcule la cantidad de registros con `fseek(...SEEK_END)` + `ftell` / `tam` y llame a la
recursiva con `li = 0`, `ls = cant - 1`. Truco de eficiencia: al leer, hacer el `fread`
directamente sobre `nue->info` (ya reservado con `malloc`), evitando una copia intermedia.

## Índice

Un uso natural del ABB es un **índice**: vincula una **clave** con el **número de registro** de
un archivo, para llegar por búsqueda directa (log de la altura) en vez de leer secuencialmente.
Se arma como cualquier ABB donde la info del nodo es el par clave→posición.

## Vaciar el árbol (postorden)

```c
void vaciarArbol(tArbol *arbol)
{
    if(*arbol)
    {
        vaciarArbol(&(*arbol)->izq);
        vaciarArbol(&(*arbol)->der);
        free((*arbol)->info);
        free(*arbol);
        *arbol = NULL;
    }
}
```

## Lo que penaliza un corrector

- Olvidar el caso base `!*arbol`.
- Liberar en pre/in orden en vez de postorden (se pierden los subárboles).
- No liberar `info` además del nodo.
- En eliminar, no manejar el caso de dos hijos.
- Confundir el orden RID / IRD / IDR de los recorridos.
