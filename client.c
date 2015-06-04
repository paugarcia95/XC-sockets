/*
* Copyright (C) 2015  Sergi Granell 	  (sergi.granell@est.fib.upc.edu)
*                     Pau Garcia Gozalvez (pau.garcia.gozalvez@est.fib.upc.edu)
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>


int main(int argc, char *argv[])
{
	struct sockaddr_in sockUDP, sockTCP;
	int fdUDP, fdTCP;
	struct sockaddr_in sockClient, sockServidor;
	int fdClient, fdServidor;
	int addrlen1, addrlen2;
	int enable = 1;
	
	char buff1[256], buff2[256];
	
	if (argc < 3) {
		printf("Usage:\n\tclient adreça_servidor port_entrada\n");
		exit(1);
	}
	
	sprintf(buff2, "Introdueix el nom de l'estudiant: ");
	write(1, buff2, strlen(buff2));
	
	while (read(0, buff1, sizeof(buff1)) > 0) {

		memset(&sockUDP, 0, sizeof(sockUDP));
		memset(&sockTCP, 0, sizeof(sockTCP));
		
		if ((fdUDP = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
			perror("socket");
			exit(1);
		}
		
		if ((fdTCP = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			close(fdUDP);
			perror("socket");
			exit(1);
		}
		
		//enviar UDP al 8080
		//rebre TCP al argv[2]
		
		sockUDP.sin_family = AF_INET;
		sockUDP.sin_addr.s_addr = inet_addr(argv[1]);
		sockUDP.sin_port = htons(8080);

		sockTCP.sin_family = AF_INET;
		sockTCP.sin_addr.s_addr = INADDR_ANY;
		sockTCP.sin_port = htons(atoi(argv[2]));


		/*
		 * This socket option tells the kernel that even if this port is busy
		 * (in the TIME_WAIT state), go ahead and reuse it anyway.
		 */
		if (setsockopt(fdTCP, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
			close(fdUDP);
			close(fdTCP);
			error("setsockopt(SO_REUSEADDR) failed");
			exit(1);
		}

		if (bind(fdTCP, (struct sockaddr *)&sockTCP, sizeof(sockTCP)) == -1) {
			close(fdUDP);
			close(fdTCP);
			perror("bind");
			exit(1);
		}
		
		if (listen(fdTCP, 10) == -1) {
			close(fdUDP);
			close(fdTCP);
			perror("listen");
			exit(1);
		}
		
		if (connect(fdUDP, (struct sockaddr *)&sockUDP, sizeof(sockUDP)) == -1) {
			close(fdUDP);
			close(fdTCP);
			perror("connect");
			exit(1);
		}
		
		// Enviar petició al servidor
		write(fdUDP, argv[2], strlen(argv[2]));
		
		// Acceptem una connexió des del servidor.jar
		addrlen1 = sizeof(sockClient);
		if ((fdClient = accept(fdTCP, (struct sockaddr *)&sockClient, &addrlen1)) == -1) {
			close(fdUDP);
			close(fdTCP);
			perror("accept");
			exit(1);
		}
		
		// Enviem el nom de l'estudiant a servidor.jar per TCP
		write(fdClient, buff1, strlen(buff1));

		close(fdClient);
		close(fdUDP);
		close(fdTCP);
		
		write(1, buff2, strlen(buff2));
	}
	
	return 0;
}
