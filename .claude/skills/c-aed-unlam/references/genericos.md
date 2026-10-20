# Genéricos sobre vectores (estilo cátedra UNLaM)

Base conceptual de toda la materia: escribir algoritmos que sirvan para **cualquier tipo de
dato** sin reescribirlos. La idea es la misma que ya usa la biblioteca estándar (`fwrite`,
`qsort`): trabajar con `void*` + el tamaño del elemento + (cuando hace falta) una función que
diga cómo comparar.

## Las dos propiedades de un tipo de dato

Todo tipo tiene **tamaño** (bytes que ocupa) y **forma de interpretación** (cómo se leen esos
bytes: un `int` es signo + complemento, un `float` es IEEE-754, un `char[]` es una cadena, una
estructura la interpreta el usuario). De ahí sale la regla práctica:

- Si la operación **solo mueve bytes** (copiar, intercambiar) → alcanza con el **tamaño**.
- Si la operación **necesita comparar** → hace falta además **cómo interpretar**, y eso se
  pasa con un **puntero a función** de comparación.

## Puntero `void` (reglas del estándar)

```c
void *vec;      // dirección genérica; NO se sabe qué tipo hay adentro
```

- **No se desreferencia**: `*vec` es error (el compilador no sabe cuántos bytes leer).
- **No se le hace aritmética** por estándar: `vec + 1` no es válido en C estricto. MinGW lo
  deja pasar moviéndose de a 1 byte, pero con `-pedantic` es warning/error. Para avanzar hay
  que **castear a `char*`** (1 byte):

```c
vec = (char*)vec + tam;                 // avanzar un elemento
void *fin = (char*)vec + (n - 1) * tam; // pararse en el ultimo elemento
```

- **Comparar dos `void*` sí está permitido** (son direcciones): `while(vec < fin)`.

## Función de comparación (contrato tipo qsort)

Devuelve `int`: **positivo** si el izquierdo es mayor, **negativo** si es menor, **0** si son
iguales (cualquier positivo/negativo sirve). La firma debe ser `const void*, const void*`
para que la misma función se pueda pasar también a `qsort` sin warnings.

```c
int cmpEnt(const void *a, const void *b)
{
    return *(int*)a - *(int*)b;         // enteros: la resta da el signo correcto
}

int cmpFloat(const void *a, const void *b)
{
    float fa = *(float*)a, fb = *(float*)b;

    if(fa < fb)                         // en flotantes NO se hace la resta:
        return -1;                      // castear el resultado a int perderia
    if(fa > fb)                         // la parte decimal y marcaria iguales
        return 1;                       // valores que no lo son
    return 0;
}

int cmpAlumnoPorApeNom(const void *a, const void *b)
{
    return strcmp(((tAlumno*)a)->apeNom, ((tAlumno*)b)->apeNom);
}
```

Tip de lectura del profe: al leer `cmp(x, y) < 0`, reemplazar la coma por el operador →
"¿*x* < *y*?". Así `cmp(vec, men) < 0` se lee "vec es menor que men".

## Puntero a función (dos formas válidas según el profe)

El nombre de una función **es** un puntero a su código; con paréntesis se ejecuta, sin
paréntesis es la dirección. Se recibe por parámetro de dos maneras equivalentes:

```c
// Forma explicita (Pesola la escribe asi en la firma):
void ordSeleccion(void *vec, size_t n, size_t tam,
                  int (*cmp)(const void*, const void*));

// Forma con typedef (estilo Nestor): mas prolija, no se repite la firma larga
typedef int (*tCmp)(const void*, const void*);
void ordSeleccion(void *vec, size_t n, size_t tam, tCmp cmp);
```

Ambas se corrigen como correctas; es cuestión de criterio del docente.

## Intercambio genérico (tres formas, y cuál conviene)

1. **`memcpy` + auxiliar dinámico**: correcto, pero pide/libera memoria en cada llamada; los
   `syscall` repetidos lo hacen lento en un algoritmo de ordenamiento.
2. **Buffer `static` de tamaño fijo** (`char aux[1000]`): rápido, pero el tamaño es arbitrario
   y si el dato lo supera se rompe. **El profe pidió explícitamente no usarlo.**
3. **Byte a byte con dos `char*`** (recomendada): rápida y anda para cualquier tamaño.

```c
void intercambiar(void *a, void *b, size_t tam)
{
    char *pa = (char*)a, *pb = (char*)b, aux;
    size_t i;

    for(i = 0; i < tam; i++)           // intercambia byte por byte
    {
        aux = pa[i];
        pa[i] = pb[i];
        pb[i] = aux;
    }
}
```

## Ordenar por selección genérico

Algoritmo de referencia de la cátedra (más simple que burbujeo). Tres pasos por elemento:
**buscar el menor hacia adelante**, **intercambiarlo con la posición actual**, **avanzar**.

```c
void *buscarMenor(void *vec, void *fin, size_t tam,
                  int (*cmp)(const void*, const void*))
{
    void *men = vec;

    for(vec = (char*)vec + tam; vec <= fin; vec = (char*)vec + tam)
        if(cmp(vec, men) < 0)          // "vec < men"
            men = vec;
    return men;
}

void ordSeleccion(void *vec, size_t n, size_t tam,
                  int (*cmp)(const void*, const void*))
{
    void *fin = (char*)vec + (n - 1) * tam;
    void *men;

    for(; vec < fin; vec = (char*)vec + tam)
    {
        men = buscarMenor(vec, fin, tam, cmp);
        if(men != vec)
            intercambiar(vec, men, tam);
    }
}
```

## qsort (biblioteca estándar)

Mismos primeros parámetros que la función propia; se puede usar **salvo que el enunciado lo
prohíba** (el enunciado siempre aclara si se permiten funciones de biblioteca).

```c
qsort(vec, n, tam, cmpEnt);            // <stdlib.h>
```

## Lo que penaliza un corrector

- Hacer aritmética o desreferenciar sobre `void*` sin castear a `char*` (rompe con `-pedantic`).
- Comparador de flotantes con resta casteada a `int`.
- Comparador con firma sin `const` cuando se lo pasa a `qsort` (da warning; el parcial exige
  compilar sin warnings).
- Usar el buffer `static` fijo para intercambiar.
- Declarar variables fuera del inicio del bloque (C89).
