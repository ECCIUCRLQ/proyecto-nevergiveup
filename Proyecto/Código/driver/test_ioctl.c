#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

extern void filtroImagen(uint32_t *img, int32_t opcion);

#define CHARACTER_DEVICE_DRIVER_PATH "/dev/pci_capture_chr_dev-0"
#define WR_VALUE _IOW('a','a',int32_t *)
#define WRITE_ACCESS_BUFFER _IOW('h', 'h', int32_t *)
#define WRITE_FOR_RESET _IOW('k', 'k', int32_t *)
#define OVERWRITE_BUFFER _IOW('l', 'l', int32_t *)
#define WRITE_IMAGE _IOW('m', 'm', int32_t *)
#define RD_VALUE _IOR('b','b',int32_t *)
#define READ_VALUE_FROM_PCI_DEVICE _IOR('c', 'c', int32_t *)
#define LOAD_PICTURE _IOR('d', 'd', int32_t *)
#define READ_WIDTH_FROM_PCI_DEVICE _IOR('e', 'e', int32_t *)
#define READ_HEIGHT_FROM_PCI_DEVICE _IOR('f', 'f', int32_t *)
#define READ_BUFFER _IOR('g', 'g', int32_t *)
#define READ_VALUE_BUFFER_FROM_PCI_DEVICE _IOR('i', 'i', int32_t *)
#define READ_IMAGE_SIZE_FROM_PCI_DEVICE _IOR('j', 'j', int32_t *)

int main()
{
    int fd;
    uint32_t value, number, option;
    const char *chr_dev_name = CHARACTER_DEVICE_DRIVER_PATH;
    uint32_t* buffer;

    printf("*********************************\n");
    printf(">>> Opening character device\n");
    fd = open(chr_dev_name, O_RDWR);
    if (fd < 0) {
        printf("Cannot open character device file...\n");
        return 0;
    }

    int32_t width = 0, height = 0, image_size = 0; 
    printf("Desea leer la imagen? (Si = 1/No = 0): ");
    scanf("%d",&option);

    switch (option) {
    case 0:
        printf("La imagen no ha sido cargada\n");
        break;
    case 1:
        ioctl(fd, LOAD_PICTURE, 0);
        ioctl(fd, READ_WIDTH_FROM_PCI_DEVICE, (int32_t*) &width);
        ioctl(fd, READ_HEIGHT_FROM_PCI_DEVICE, (int32_t*) &height);
        ioctl(fd, READ_IMAGE_SIZE_FROM_PCI_DEVICE, (int32_t*) &image_size);
        printf("Favor esperar mientras se procesa...\n");
        buffer = (uint32_t*)malloc((image_size)*sizeof(uint32_t));
        if (buffer) {
            for (int i = 0; i < image_size; i++) {
                ioctl(fd, WRITE_ACCESS_BUFFER, (int32_t*) &value); 
                ioctl(fd, READ_VALUE_BUFFER_FROM_PCI_DEVICE, (int32_t*) &value);
                buffer[i] = value;
            }
            printf("La imagen ha sido cargada\n");
            printf("Ancho: %d, Alto: %d, Tamanio Imagen: %d\n", width, height, image_size);
            ioctl(fd, WRITE_FOR_RESET, 0);
            do {
                printf("\n>>>>>> Menu Filtros <<<<<<\n1.Rojo\n2.Azul\n3.Verde\n4.Fucsia\n\nDigite el numero de filtro a aplicar: ");
                scanf("%d",&option);
                if (option < 1 || option > 4) {
                    printf("Error: Valor fuera de rango\n");
                }
            } while (option < 1 || option > 4);
            filtroImagen(&*buffer, option);   
            for (int i = 0; i < image_size; i++) {          
                value = buffer[i];
                ioctl(fd, OVERWRITE_BUFFER, (uint32_t*) &value); 
            }
            ioctl(fd, WRITE_IMAGE, 0);
            ioctl(fd, WRITE_FOR_RESET, 0);
            free(buffer);
        } else {
            printf("Error: el buffer no ha podido ser inicializado\n");
        }
        break;
    default:
        break;
    }

    printf("Closing character device file\n");
    close(fd);
}
