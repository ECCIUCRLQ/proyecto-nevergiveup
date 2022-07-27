#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

extern void filtro_red(uint32_t* red, uint32_t* blue, uint32_t* green, float number);
extern void filtro_blue(uint32_t* red, uint32_t* blue, uint32_t* green, float number);
extern void filtro_green(uint32_t* red, uint32_t* blue, uint32_t* green, float number);
extern void filtro_fucsia(uint32_t* red, uint32_t* blue, uint32_t* green, float number);

typedef struct cabeceraBMP
{
    uint16_t bm; //(2 Bytes) BM (Tipo de archivo)
    uint32_t tamanoArchivo; //(4 Bytes) Tamaño del archivo en bytes
    uint32_t reservado; //(4 Bytes) Reservado
    uint32_t offset; //(4 Bytes) offset, distancia en bytes entre la img y los píxeles
    uint32_t tamanoCabecera; //(4 Bytes) Tamaño de Metadatos (tamaño de esta estructura = 40) 
    uint32_t alto; //(4 Bytes) Ancho (numero de píxeles horizontales)
    uint32_t ancho; //(4 Bytes) Alto (numero de pixeles verticales)
    uint16_t numeroPlanos; //(2 Bytes) Numero de planos de color
    uint16_t bitsPorPixel; //(2 Bytes) Profundidad de color (debe ser 24 para nuestro caso) 
    uint32_t tipoCompresion; //(4 Bytes) Tipo de compresión (Vale 0, ya que el bmp es descomprimido)
    uint32_t tamanoImagen; //(4 Bytes) Tamaño de la Imagen 
    uint32_t pxmh; //(4 Bytes) Píxeles por metro horizontal
    uint32_t pxmv ; //(4 Bytes) Píxeles por metro vertical
    uint32_t coloresUsados; //(4 Bytes) Cantidad de colores usados 
    uint32_t coloresImportantes; //(4 Bytes) Cantidad de colores importantes
} bmpInfoCabezado;


void CargarImagen(char* nombreArchivo, bmpInfoCabezado *infoCabezado);
void ImprimirEncabezado(bmpInfoCabezado *info);

void filtroImagen(uint32_t *img, int32_t opcion) {
    bmpInfoCabezado info;
    CargarImagen("/home/simics/driver/Paisaje.bmp", &info);
    uint32_t blue, green, red;
    switch (opcion) {
        case 1:
            /******************** filtro rojo ********************/
            for (int i = 0; i < info.alto; i++) {
                for (int j = 0; j < info.ancho; j++) {
                    blue = img[3*(info.ancho*i+j)];
                    green = img[3*(info.ancho*i+j)+1];
                    red = img[3*(info.ancho*i+j)+2];
                    filtro_red(&red, &blue, &green, 0.3);
                    img[3*(info.ancho*i+j)] = blue;
                    img[3*(info.ancho*i+j)+1] = green;
                    img[3*(info.ancho*i+j)+2] = red;
                }
            }
            break;
        case 2:
            /******************** filtro azul ********************/
            for (int i = 0; i < info.alto; i++) {
                for (int j = 0; j < info.ancho; j++) {
                    blue = img[3*(info.ancho*i+j)];
                    green = img[3*(info.ancho*i+j)+1];
                    red = img[3*(info.ancho*i+j)+2];
                    filtro_blue(&red, &blue, &green, 0.4);
                    img[3*(info.ancho*i+j)] = blue;
                    img[3*(info.ancho*i+j)+1] = green;
                    img[3*(info.ancho*i+j)+2] = red;
                }
            }
            break;
        case 3:
            /******************** filtro verde ********************/
            for (int i = 0; i < info.alto; i++) {
                for (int j = 0; j < info.ancho; j++) {
                    blue = img[3*(info.ancho*i+j)];
                    green = img[3*(info.ancho*i+j)+1];
                    red = img[3*(info.ancho*i+j)+2];
                    filtro_green(&red, &blue, &green, 0.4);
                    img[3*(info.ancho*i+j)] = blue;
                    img[3*(info.ancho*i+j)+1] = green;
                    img[3*(info.ancho*i+j)+2] = red;
                }
            }
            break;
        case 4:
        /******************** filtro fucsia ********************/
        for (int i = 0; i < info.alto; i++) {
            for (int j = 0; j < info.ancho; j++) {
                blue = img[3*(info.ancho*i+j)];
                green = img[3*(info.ancho*i+j)+1];
                red = img[3*(info.ancho*i+j)+2];
                filtro_fucsia(&red, &blue, &green, 0.4);
                img[3*(info.ancho*i+j)] = blue;
                img[3*(info.ancho*i+j)+1] = green;
                img[3*(info.ancho*i+j)+2] = red;
            }
        }
        break;
    }
}

void CargarImagen(char* nombreArchivo, bmpInfoCabezado *infoCabezado) {
    FILE* file;

    file=fopen (nombreArchivo, "r");
    if (!file) {
        printf("Error: El archivo no pudo ser abierto\n");
    }
    /* Leemos los dos primeros bytes */

    /* Leemos la cabecera de información completa */
    fseek(file, 0, SEEK_SET);
    fread(&infoCabezado->bm, sizeof(uint16_t), 1, file);
    fread(&infoCabezado->tamanoArchivo, sizeof(uint32_t), 1, file);
    fread(&infoCabezado->reservado, sizeof(uint32_t), 1, file);
    fread(&infoCabezado->offset, sizeof(uint32_t), 1, file);
    fread(&infoCabezado->tamanoCabecera, sizeof(uint32_t), 1, file);
    fread(&infoCabezado->ancho, sizeof(uint32_t), 1, file);
    fread(&infoCabezado->alto, sizeof(uint32_t), 1, file);
    fread(&infoCabezado->numeroPlanos, sizeof(uint16_t), 1, file);
    fread(&infoCabezado->bitsPorPixel, sizeof(uint16_t), 1, file);
    fread(&infoCabezado->tipoCompresion, sizeof(uint32_t), 1, file);
    fread(&infoCabezado->tamanoImagen, sizeof(uint32_t), 1, file);
    fread(&infoCabezado->pxmh, sizeof(uint32_t), 1, file);
    fread(&infoCabezado->pxmv, sizeof(uint32_t), 1, file);
    fread(&infoCabezado->coloresUsados, sizeof(uint32_t), 1, file);
    fread(&infoCabezado->coloresImportantes, sizeof(uint32_t), 1, file);
    if (infoCabezado->bm != 0x4D42) {       /* Comprobamos el formato */
        printf("Error: La imagen no está en formato BMP\n");
        fclose(file);
    }    

    /* Cerramos */
    fclose(file);
}

void ImprimirEncabezado(bmpInfoCabezado *info) {
  printf("Tamaño de la cabecera: %u\n", info->tamanoCabecera);
  printf("Anchura: %d\n", info->ancho);
  printf("Altura: %d\n", info->alto);
  printf("Planos (1): %d\n", info->numeroPlanos);
  printf("Bits por pixel: %d\n", info->bitsPorPixel);
  printf("Compresión: %d\n", info->tipoCompresion);
  printf("Tamaño de datos de imagen: %u\n", info->tamanoImagen);
  printf("Resolución horizontal: %u\n", info->pxmh);
  printf("Resolución vertical: %u\n", info->pxmv);
  printf("Colores en paleta: %d\n", info->coloresUsados);
  printf("Colores importantes: %d\n", info->coloresImportantes);
}
