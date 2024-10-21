#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h> //for IOCTL defs
#include <fcntl.h>
#include "interfaceVL6180x.h"

#define I2C_FICHIER "/dev/i2c-1" // fichier Linux representant le BUS #2
#define I2C_ADRESSE 0x29 // adresse du Device I2C MPU-9250 (motion tracking)

int main()
{
	float Distance ;
	// Écriture et Lecture sur le port I2C
	uint8_t Source[2] = {0x00, 0x00}; // registre d'ID du chip I2C
	uint8_t Destination;
	uint8_t NombreDOctetsALire = 1;
	uint8_t NombreDOctetsAEcrire = 2;

	interfaceVL6810x_initialise();
	interfaceVL6180x_litUneDistance(&Distance);
	printf("/n La distance est de: %e", Distance);

	//if(write(fdPortI2C, &Source, NombreDOctetsAEcrire) != NombreDOctetsAEcrire)
	//{
	//	printf("erreur: Écriture I2C\n");
	//	close(fdPortI2C);
	//	return -1;
	//}
	//if (read(fdPortI2C, &Destination, NombreDOctetsALire) != NombreDOctetsALire)
	//{
	//	printf("erreur: Lecture I2C\n");
	//	close(fdPortI2C);
	//	return -1;
	//}
	//printf("octets lus: %#04x\n", Destination);

	
	return 0;
}
