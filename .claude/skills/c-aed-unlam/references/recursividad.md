# Recursividad (estilo cátedra UNLaM)

Eje de la materia: se usa mucho para recorrer listas y, sobre todo, árboles. Una función
recursiva es una que se llama a sí misma; cada llamada repite el código, como si fuera un
ciclo. Si falta la condición de corte, el stack no para de crecer → **stack overflow**.

## Método de la cátedra (3 pasos)

1. **Encontrar la definición recursiva del problema y decirla en voz alta.** Ej.: "el
   factorial de n es n por el factorial de n-1". Si el problema no se puede definir en
   términos de sí mismo, no conviene resolverlo recursivo.
2. **Definir la condición de corte** (caso base): el caso que se resuelve sin volver a
   llamar. Va **primero** en la función.
3. **Reducción del universo**: cada llamada tiene que acercarse al caso base (n-1, un
   subárbol, etc.), o el corte nunca se alcanza.

Regla práctica: al pasar de iterativo a recursivo, el `while` que recorría **se transforma en
un `if`**, porque quien cicla ahora es la propia recursión. Un `while` dentro de una función
recursiva suele ser señal de que algo está mal.

## Factorial y MCD (Euclides)

```c
long factorial(int n)
{
    if(n == 0)                     // caso base
        return 1;
    return n * factorial(n - 1);   // caso recursivo
}

int mcd(int m, int n)             // algoritmo de Euclides
{
    if(m % n == 0)
        return n;
    return mcd(n, m % n);
}
```

## El orden de las operaciones = pila

Dónde se pone la acción respecto de la llamada recursiva cambia el orden del resultado:

```c
void mostrar(char *s)
{
    if(*s)
    {
        putchar(*s);              // ANTES de bajar: imprime en orden  -> "abcd"
        mostrar(s + 1);
        // putchar(*s);           // DESPUÉS de bajar: imprime al revés -> "dcba"
    }
}
```

Las llamadas se apilan en orden y se desapilan en orden inverso (LIFO). Muchas cuentas (contar
nodos, altura) se resuelven **al volver** de la recursión, no al bajar: el último llamado
devuelve el caso base y los anteriores completan el cálculo en cascada.

## El `return` en la llamada recursiva

Hay que devolver el valor de la llamada recursiva cuando:
- se **propaga un cálculo** hacia atrás (`return 1 + cant(izq) + cant(der);`), o
- se quiere **cortar tras una acción única** (insertar/encontrar una sola vez).

Olvidar el `return` es un error clásico y grave: p. ej. insertar en un árbol sin `return`
hace que, al volver, cada nivel vuelva a reservar memoria e insertar el nodo otra vez (fugas y
árbol roto). Distinto es cuando la acción se hace por efecto lateral en cada nodo (imprimir):
ahí puede no hacer falta devolver nada.

## Función envoltorio (llamadora)

Cuando la función recursiva necesita un parámetro interno que el usuario no debería ver ni
tocar (el nivel actual, un contador, un flag de "primera vez"), se expone una **función
envoltorio** que lo inicializa y delega en la recursiva:

```c
// recursiva: recibe el nivel actual
static int contarHastaNivelR(tArbol *arbol, int nivel, int objetivo)
{
    if(!*arbol)
        return 0;
    if(nivel == objetivo)
        return 1;
    return contarHastaNivelR(&(*arbol)->izq, nivel + 1, objetivo)
         + contarHastaNivelR(&(*arbol)->der, nivel + 1, objetivo);
}

// envoltorio: lo que usa quien llama; arranca el nivel en 0
int contarHastaNivel(tArbol *arbol, int objetivo)
{
    return contarHastaNivelR(arbol, 0, objetivo);
}
```

## Lo que penaliza un corrector

- Falta o error en la condición de corte (recursión infinita → stack overflow).
- No reducir el universo en cada llamada.
- Olvidar el `return` cuando hay que propagar el valor o cortar.
- Usar un `while` para "recorrer" dentro de una función que ya es recursiva.
