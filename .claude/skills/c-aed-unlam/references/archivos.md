# Archivos binarios y de texto (estilo cátedra UNLaM)

La cátedra arma funciones **genéricas** de archivo: unas reciben el nombre y abren/cierran el
archivo, otras reciben un `FILE*` ya abierto y hacen el trabajo con una **acción** por puntero
a función (mismo estilo que `recorrerLista`).

## Archivos binarios

```c
#include <stdio.h>

/* accion aplicada a cada registro leido */
typedef void (*tAccion)(void*, void*);

int crearArchBin(char *nombArch, void *vec, size_t lon, size_t tamEl)
{
    FILE *arch;

    if(!(arch = fopen(nombArch, "wb")))
        return ERROR_ARCH;

    fwrite(vec, tamEl, lon, arch);   // escribe lon elementos de tamEl bytes
    fclose(arch);
    return EXITO;
}

/* recibe un archivo YA abierto y ejecuta accion en cada registro */
int leerDesdeArchBin(FILE *arch, size_t tamInfo, tAccion accion, void *param)
{
    void *buffer;

    if(!(buffer = malloc(tamInfo)))
        return ERROR_MALLOC;

    while(fread(buffer, tamInfo, 1, arch) == 1)
        accion(buffer, param);

    free(buffer);
    return EXITO;
}

/* version que abre por nombre y delega en la anterior */
int leerArchBin(char *nombArch, size_t tamInfo, tAccion accion, void *param)
{
    FILE *arch;
    int ret;

    if(!(arch = fopen(nombArch, "rb")))
        return ERROR_ARCH;

    ret = leerDesdeArchBin(arch, tamInfo, accion, param);
    fclose(arch);
    return ret;
}

/* escribir en una posicion determinada (registro pos, base 0) */
int escribirPosDesdeArchBin(FILE *arch, void *dato, size_t tamDato, size_t pos)
{
    if(fseek(arch, (long)(pos * tamDato), SEEK_SET) != 0)
        return ERROR_ARCH;
    if(fwrite(dato, tamDato, 1, arch) != 1)
        return ERROR_ARCH;
    return EXITO;
}
```

Idea de la librería: separar **"abrir por nombre"** (crea/cierra) de **"trabajar sobre `FILE*`
abierto"**. Así se pueden encadenar operaciones sobre el mismo archivo sin reabrirlo, y la
acción genérica decide qué hacer con cada registro (imprimir, acumular, filtrar, etc.).

## Archivos de texto

Mismo esquema pero con `fopen(..., "wt"/"rt")`, `fprintf`/`fscanf` o `fgets`. Ejemplo de
lectura en recorrido línea por línea:

```c
int leerArchTexto(char *nombArch, void accion(char*, void*), void *param)
{
    FILE *arch;
    char linea[256];

    if(!(arch = fopen(nombArch, "rt")))
        return ERROR_ARCH;

    while(fgets(linea, sizeof(linea), arch))
        accion(linea, param);

    fclose(arch);
    return EXITO;
}
```

Recordar: `fgets` conserva el `'\n'` final; si molesta, quitarlo antes de procesar.

## Lo que penaliza un corrector

- No chequear que `fopen` no devolvió `NULL`.
- No cerrar el archivo con `fclose` (fuga de descriptor).
- Verificar el retorno de `fread`/`fwrite` (cantidad de elementos) y de `fseek`.
- Mezclar apertura y trabajo en una sola función cuando el enunciado pide la librería genérica.
