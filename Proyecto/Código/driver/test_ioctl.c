#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define CHARACTER_DEVICE_DRIVER_PATH "/dev/pci_capture_chr_dev-0"
#define WR_VALUE _IOW('a','a',int32_t *)
#define RD_VALUE _IOR('b','b',int32_t *)
#define READ_VALUE_FROM_PCI_DEVICE _IOR('c', 'c', int32_t *)
#define LOAD_PICTURE _IOR('d', 'd', int32_t *)
#define READ_WIDTH_FROM_PCI_DEVICE _IOR('e', 'e', int32_t *)
#define READ_HEIGHT_FROM_PCI_DEVICE _IOR('f', 'f', int32_t *)
#define READ_BUFFER _IOR('g', 'g', int32_t *)
#define WRITE_ACCESS_BUFFER _IOW('h', 'h', int32_t *)
#define READ_VALUE_BUFFER_FROM_PCI_DEVICE _IOR('i', 'i', int32_t *)

int main()
{
    int fd;
    int32_t option, value;
    const char *chr_dev_name = CHARACTER_DEVICE_DRIVER_PATH;

    printf("*********************************\n");
    printf(">>> Opening character device\n");
    fd = open(chr_dev_name, O_RDWR);
    if (fd < 0) {
        printf("Cannot open character device file...\n");
        return 0;
    }
    int32_t offset = 0x2a, width = 0, height = 0; 
    ioctl(fd, READ_VALUE_FROM_PCI_DEVICE, (int32_t*) &offset);
    ioctl(fd, READ_WIDTH_FROM_PCI_DEVICE, (int32_t*) &width);
    offset = 0x2e;
    ioctl(fd, READ_VALUE_FROM_PCI_DEVICE, (int32_t*) &offset);
    ioctl(fd, READ_HEIGHT_FROM_PCI_DEVICE, (int32_t*) &height);
    printf("Ancho: %d, Alto: %d\n", width, height);
    printf("¿Desea leer la imagen? (Sí = 1/No = 0): ");
    scanf("%d",&option);
    int32_t* buffer =  malloc((height*width)*sizeof(int32_t));

    for (int i = 0; i < width*height; i++) {
        ioctl(fd, WRITE_ACCESS_BUFFER, (int32_t*) &value); 
        ioctl(fd, READ_VALUE_BUFFER_FROM_PCI_DEVICE, (int32_t*) &value);
        buffer[i] = value;
    }

    switch (option) {
    case 0:
        printf("La imagen no ha sido cargada\n");
        break;
    case 1:
        ioctl(fd, LOAD_PICTURE, 0);
        printf("La imagen ha sido cargada\n");
        break;
    default:
        break;
    }

    printf("Closing character device file\n");
    close(fd);
}
