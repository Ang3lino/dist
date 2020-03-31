#include "DatagramSocket.h"


DatagramSocket::DatagramSocket(): DatagramSocket(0) {}

DatagramSocket::DatagramSocket(uint16_t iport): DatagramSocket(iport, "0.0.0.0") {}

DatagramSocket::DatagramSocket(uint16_t iport, const std::string &addr): timeout_set(false) {
	s = socket(AF_INET, SOCK_DGRAM, 0);
	// bzero((char *)&localAddress, sizeof(localAddress));
	memset((char *) &localAddress, 0, sizeof(localAddress));
	localAddress.sin_family = AF_INET;
	localAddress.sin_addr.s_addr = inet_addr(addr.c_str());
	localAddress.sin_port = htons(iport);
	bind(s, (struct sockaddr *) &localAddress, sizeof(localAddress));
}

void DatagramSocket::setTimeout(time_t secs, time_t u_secs) {
	timeout = { 
		.tv_sec = secs, 
		.tv_usec = u_secs };  
	timeout_set = true;
	#ifdef linux
		setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout));
	#else 
		struct fd_set fds;
		// Setup fd_set structure
		FD_ZERO(&fds);
		FD_SET(s, &fds);
	#endif
}

DatagramSocket::~DatagramSocket() {
	unbind();
}

void DatagramSocket::unbind() {
	#ifdef linux
		close(s);
		s = 0;
	#else 
		closesocket(s);
		WSACleanup();
	#endif
}

int DatagramSocket::receive(DatagramPacket &p) {
	socklen_t len = sizeof(remoteAddress);
	int n = recvfrom(s, p.getData(), p.getLength(), 0, (struct sockaddr*) &remoteAddress, &len);
	p.setPort(ntohs(remoteAddress.sin_port));
	p.setAddress(std::string(inet_ntoa(remoteAddress.sin_addr)));
	p.setLength(n);
	return n;
}

int DatagramSocket::receiveTimeout(DatagramPacket & p, time_t secs, time_t u_secs) {
	setTimeout(secs, u_secs);  // setting timeout is required once 
	socklen_t len = sizeof(remoteAddress);
	#ifdef linux
		int n = recvfrom(s, p.getData(), p.getLength(), 0, (struct sockaddr *) &remoteAddress, &len);
		if (n < 0) {  // deal with errors if so
			if (errno == EWOULDBLOCK) 
				fprintf(stderr, "Timeout \n");
			else 
				fprintf(stderr, "Error in recvfrom. \n");
			return n;
		}
	#else 
		int n = select(0, &fds, 0, 0, &timeout);
		if (n == -1) {
			fprintf(stderr, "Error with the connection"); 
			return n;
	 	} else if (n == 0) {
			fprintf(stderr, "Timeout \n"); 
			return n;
		}
		n = recvfrom(s, p.getData(), p.getLength(), 0, (struct sockaddr *) &remoteAddress, &len);
	#endif
	p.setPort(ntohs(remoteAddress.sin_port));  // everything is ok
	p.setAddress(std::string(inet_ntoa(remoteAddress.sin_addr)));
	p.setLength(n);
	return n;
}

int DatagramSocket::send(DatagramPacket &p) {
	// bzero((char *)&remoteAddress, sizeof(remoteAddress));
	memset((char *) &remoteAddress, 0, sizeof(remoteAddress));

	remoteAddress.sin_family = AF_INET;
	remoteAddress.sin_port = htons(p.getPort());
	remoteAddress.sin_addr.s_addr = inet_addr(p.getAddress().c_str());
	return sendto(s, p.getData(), p.getLength(), 0, (struct sockaddr*) &remoteAddress, sizeof(remoteAddress));
}
