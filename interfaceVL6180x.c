//#include "main.h"
//#include "piloteI2C1.h"
#include "interfaceVL6180x.h"
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h> //for IOCTL defs
#include <fcntl.h>

//pour des détails: https://www.st.com/resource/en/design_tip/dt0037-vl6180x-range-and-ambient-light-sensor-quick-setup-guide-stmicroelectronics.pdf


//définitions privée
#define UC unsigned char
#define UI unsigned int
#define I2C_FICHIER "/dev/i2c-1" // fichier Linux representant le BUS #2
#define I2C_ADRESSE 0x29 // adresse du Device I2C MPU-9250 (motion tracking)
typedef struct
{
	UI uiAdresse;
	UC ucValeur;
} INTERFACEVL6810X_MESSAGE;

#define INTERFACEVL6180X_ADRESSE 0x29 
#define INTERFACEVL6180X_NOMBRE	40

//variables privées
INTERFACEVL6810X_MESSAGE interfaceVL6810x_message[INTERFACEVL6180X_NOMBRE] =
{
	{0x0207, 0x01}, {0x0208, 0x01}, {0x0096, 0x00}, {0x0097, 0xfd},
	{0x00e3, 0x00}, {0x00e4, 0x04},	{0x00e5, 0x02},	{0x00e6, 0x01},
	{0x00e7, 0x03},	{0x00f5, 0x02},	{0x00d9, 0x05},	{0x00db ,0xce},
	{0x00dc, 0x03},	{0x00dd, 0xf8},	{0x009f, 0x00},	{0x00a3, 0x3c},
	{0x00b7, 0x00},	{0x00bb, 0x3c},	{0x00b2, 0x09},	{0x00ca, 0x09},
	{0x0198, 0x01},	{0x01b0, 0x17},	{0x01ad, 0x00},	{0x00ff, 0x05},
	{0x0100, 0x05},	{0x0199, 0x05},	{0x01a6, 0x1b},	{0x01ac, 0x3e},
	{0x01a7, 0x1f},	{0x0030, 0x00},
	{0x0011, 0x10},// Enables polling for ‘New Sample ready’ when measurement completes
	{0x010a, 0x30},// Set the averaging sample period (compromise between lower noise and increased execution time)
	{0x003f, 0x46},// Sets the light and dark gain (upper nibble). Dark gain should not be changed.
	{0x0031, 0xFF},// sets the # of range measurements after which auto calibration of system is performed 
	{0x0040, 0x63},// Set ALS integration time to 100ms
	{0x002e, 0x01},// perform a single temperature calibration of the ranging sensor 
	{0x001b, 0x09},// *Set default ranging inter-measurement period to 100ms
	{0x003e, 0x31},// *Set default ALS inter-measurement period to 500ms
	{0x0014, 0x24},// *Configures interrupt on ‘New Sample Ready threshold event’
	{0x0016, 0x00} //*change fresh out of set status to 0
};
int fdPortI2C;
//fonctions privées
//pas de fonctions privées

//variables publiques
//pas de variables publiques

//fonctions publiques
int interfaceVL6180x_ecrit(UI Registre, UC Donnee)
{
UC ucMessage[3];
	ucMessage[0] = (UC)(Registre >> 8);
	ucMessage[1] = (UC)(Registre & 0xFF);
	ucMessage[2] = Donnee;
	
	if(write(fdPortI2C, &ucMessage, 3) < 0)
	{
		printf("erreur: Écriture I2C\n");
		close(fdPortI2C);
		return -1;
	}
	//if (piloteI2C1_ecritDesOctets(ucMessage, 3) < 0)
	//{
	//	printf("erreur: interfaceVL6180x_ecrit\n");
	//	return -1;
	//}
	return 0;
}

int interfaceVL6180x_lit(UI Registre, UC *Donnee)
{
UC Commande[2];
	Commande[0] = (UC)(Registre >> 8);
	Commande[1] = (UC)Registre;
	if (read(fdPortI2C, &Donnee, 1) < 0)
	{
		printf("erreur: Lecture I2C\n");
		close(fdPortI2C);
		return -1;
	}
	//printf("octets lus: %#04x\n", Donnee);
	//if (piloteI2C1_litDesOctets(Commande, 2, Donnee, 1) < 0)
	//{
	//	printf("erreur: interfaceVL6180x_lit\n");
	//	return -1;
	//}
	return 0;
}

int interfaceVL6180x_litUneDistance(float *Distance)
{
UC valeur;
	if (interfaceVL6180x_ecrit(0x18, 0x01) < 0)
	{
		printf("erreur: interfaceVL6180x_litUneDistance 1\n");
		return -1;
	}
	if(interfaceVL6180x_lit(0x4F, &valeur) < 0)
	{
		printf("erreur: interfaceVL6180x_litUneDistance 2\n");
		return -1;
	}
	while((valeur & 0x7) != 4)
  {
    if (interfaceVL6180x_lit(0x4F, &valeur) < 0)
    {
    	printf("erreur: interfaceVL6180x_litUneDistance 3\n");
      return -1;
    }
  }
  if (interfaceVL6180x_lit(0x62, &valeur) < 0)
  {
		printf("erreur: interfaceVL6180x_litUneDistance 4\n");
    return -1;
	}
	if (interfaceVL6180x_ecrit(0x15, 0x07) < 0)
  {
  	printf("erreur: interfaceVL6180x_litUneDistance 5\n");
    return -1;
	}
	*Distance = (float)valeur /10.0;
	close(fdPortI2C); /// Fermeture du 'file descriptor'
	return 0;
}

int interfaceVL6810x_initialise(void)
{
UC index;
UC valeur;
	//if (piloteI2C1_configureLAdresse(INTERFACEVL6180X_ADRESSE) < 0)
	//{
		//printf("erreur: interfaceVL6810x_initialise 1\n");
	//	return -1;
	//}

  //if (interfaceVL6180x_lit(0x16, &valeur) < 0)
  //{
  	//printf("erreur: interfaceVL6180x_initialise 2\n");
    //return -1;
  //}
	//if (valeur != 1)
	//{
	//	printf("message interfaceVL6180x: le VL6180x va être reconfiguré\n");
	//}
	
	  // file descriptor I2C

	// Initialisation du port I2C, 
	fdPortI2C = open(I2C_FICHIER, O_RDWR); // ouverture du 'fichier', création d'un 'file descriptor' vers le port I2C
	if(fdPortI2C == -1)
	{
		printf("erreur: I2C initialisation step 1\n");
		return -1;
	}
	if(ioctl(fdPortI2C, I2C_SLAVE_FORCE, I2C_ADRESSE) < 0)  // I2C_SLAVE_FORCE if it is already in use by a driver (i2cdetect : UU)
	{
		printf("erreur: I2C initialisation step 2\n");
		close(fdPortI2C);
		return -1;
	}

	write(fdPortI2C, &interfaceVL6810x_message, INTERFACEVL6180X_NOMBRE) ;
	//for (index = 0; index < INTERFACEVL6180X_NOMBRE; index++)
	//{
	//	WriteByte(interfaceVL6810x_message[index]);
		//if (interfaceVL6180x_ecrit(interfaceVL6810x_message[index].uiAdresse, 
		//		interfaceVL6810x_message[index].ucValeur) < 0)
		//{
		//	printf("erreur: interfaceVL6180x_initialise 3 - index %d\n", index);
		//	return -1;
		//}
	//}
	return 0;
}



