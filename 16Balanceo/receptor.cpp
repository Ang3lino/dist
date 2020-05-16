
// Author: Angel Manriquez


#include <bits/stdc++.h>

#include "./lib/MulticastSocket.h"
#include "pack_t.h"

using namespace std;


void parse_args(int argc, char const *argv[], string &multicast, int &port) {
	if (argc != 3) {
		printf("[%s] [MULTICAST] [PORT]\n", argv[0]);
		exit(1);
	} 
	multicast = argv[1];
	port = atoi(argv[2]);
}

int main(int argc, char const *argv[]) {
	int nbd = 0;
	pack_t frame;
	set<unsigned> hist; // historial que guardara los id para idempotencia

	string multicast_ip = "224.0.0.0";
	int port = 7777;
	// parse_args(argc, argv, multicast, port);

	MulticastSocket msock(port);
	msock.joinGroup(0, multicast_ip); 
	cout << "Esperando la primer solicitud" << endl;
	while (true) {
		DatagramPacket packet((char *) &frame, sizeof(frame));
		int receive_code = msock.receiveReliable(packet);
		if (receive_code < 0) {
			cerr << ("No se pudo recibir la trama\n");
		} else {
			if (hist.count(frame.id) == 0) {  // el id no esta en el historial
				hist.insert(frame.id);
				nbd += frame.data[0];
				cout << nbd << endl;
			} else {
				printf("Paquete reenviado\n");
			}
		}
	}	
	return 0;
}