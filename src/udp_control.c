#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "ev.h"
#include "utils.h"
#include "balancing.h"
#include "udp_control.h"

#define BUF_SIZE        4096

typedef struct udp_control_ctx {
    ev_io io;
    int fd;
} udp_control_ctx_t;

udp_control_ctx_t control;

// This callback is called when data is readable on the UDP socket.
static void udp_cb(EV_P_ ev_io *w, int revents) {
	char buffer[BUF_SIZE];
	struct sockaddr_in addr;
	int addr_len = sizeof(addr);

	udp_control_ctx_t *control = (udp_control_ctx_t *)w;

    puts("udp socket has become readable");
    socklen_t bytes = recvfrom(control->fd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*) &addr, (socklen_t *) &addr_len);

    // add a null to terminate the input, as we're going to use it as a string
    buffer[bytes] = '\0';

    printf("udp client said: %s", buffer);

    size_t length = 0;
    size_t weights[MAX_BALANCING_SIZE];
    int read_chars, offset=0;

    for(; length < MAX_BALANCING_SIZE && sscanf(buffer+offset, "%lu%n", &weights[length], &read_chars) == 1; length++){
    	offset += read_chars;
    }

    printf("Weight inputs: ");

    for(int i=0; i<length; i++){
    	printf("%lu ", weights[i]);
    }

    printf("\n");

    update_balancing_weights(weights, length);

    size_t *weight_output;
    int balancing_size = get_balancing_weights(&weight_output);

    buffer[0]=0;
    bytes=0;

    printf("Weight outputs: ");

    for(int i=0; i<balancing_size; i++){
    	printf("%lu ", weight_output[i]);
    	bytes += sprintf(buffer+bytes, "%lu ", weight_output[i]);
    }

    printf("\n");
    bytes += sprintf(buffer+bytes, "\n");

    sendto(control->fd, buffer, bytes, 0, (struct sockaddr*) &addr, sizeof(addr));
}

int init_udp_control(char *addr, char *port)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int s, sock;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family   = AF_UNSPEC;   /* Return IPv4 and IPv6 choices */
	hints.ai_socktype = SOCK_DGRAM; /* We want a TCP socket */

	s = getaddrinfo(addr, port, &hints, &result);
	if (s != 0) {
		LOGI("getaddrinfo: %s", gai_strerror(s));
		return -1;
	}

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sock == -1) {
			continue;
		}

		s = bind(sock, rp->ai_addr, rp->ai_addrlen);
		if (s == 0) {
			/* We managed to bind successfully! */
			break;
		} else {
			ERROR("bind");
		}

		close(sock);
	}

	if (rp == NULL) {
		LOGE("Could not bind");
		return -1;
	}

	freeaddrinfo(result);

	control.fd = sock;

	struct ev_loop *loop = EV_DEFAULT;

	ev_io_init(&control.io, udp_cb, sock, EV_READ);
	ev_io_start(loop, &control.io);

	return sock;
}
