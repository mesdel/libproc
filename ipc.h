/*
 * Copyright PolySat, California Polytechnic State University, San Luis Obispo. cubesat@calpoly.edu
 * This file is part of libproc, a PolySat library.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file ipc.h Inter-process communication header file.
 *
 * Header for IPC component of the polysat library.
 *
 * @author Greg Manyak <gregmanyak@gmail.com>
 */
#ifndef IPC_H
#define IPC_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Maximum size of an IP packet
#define MAX_IP_PACKET_SIZE 65535

/**
 * Gets a file descriptor for a socket based on a named service.
 * Named services can be used by adding an entry to the /etc/services file.
 *
 * @param   service Name of the service to open the UDP port of.
 *
 * @return  A socket file descriptor.
 *
 * @retval  -1  On error.
 */
int socket_named_init(const char * service);

/**
 * Initializes a UDP socket on the specified port.
 *
 * @param   port Host order port number to open socket on.
 *
 * @return  A socket file descriptor.
 *
 * @retval  -1  On error.
 */
int socket_init(int port);

/**
 * Reads data from a specified socket fd.
 * Loads source of data into src pointer.
 *
 * @param   fd      A socket file descriptor.
 * @param   buf     Pointer to memory for putting recv'd data into.
 * @param   bufSize Number of bytes available at buf.
 * @param   src     Source address of data.
 *
 * @return  Number of bytes read.
 *
 * @retval  -1     On error.
 */
int socket_read(int fd, void * buf, size_t bufSize, struct sockaddr_in * src);

/**
 * Writes data to a socket based on the service name.
 *
 * @param   fd      A socket file descriptor.
 * @param   buf     Pointer to data to be sent.
 * @param   bufSize Number of bytes to be sent.
 * @param   name    Name of service to send to.
 *
 * @return  Number of bytes written to socket.
 * @retval  -1       On error
 */
int socket_named_write(int fd, void * buf, size_t bufSize, const char * name);

/**
 * Writes data on the provided socket fd to the dest sockaddr.
 *
 * @param   fd      A socket file descriptor.
 * @param   buf     Pointer to data to be sent.
 * @param   bufSize Number of bytes to be sent.
 * @param   dest    Destination socket address.
 *
 * @return  Number of bytes written.
 *
 * @retval  -1  On error.
 */
int socket_write(int fd, void * buf, size_t bufSize, struct sockaddr_in * dest);

/**
 * Closes a socket.
 *
 * Acts as a wrapper to close right now, may be more useful later.
 *
 * @param   fd   A socket file descriptor.
 *
 * @retval  0  On success.
 * @retval  -1 On failure.
 */
int socket_close(int fd);

/**
 * Looks up a udp service by name and returns port in host order.
 *
 * @param   service Name of service to be looked up in /etc/services.
 *
 * @return  UDP port number in host order.
 *
 * @retval  -1 On error.
 */
int socket_get_addr_by_name(const char * service);

/**
 * Looks up a system service by name and returns the associated multicast
 *    source address.
 *
 * @param   service Name of service to be looked up
 *
 * @return  IP Adress in network order.
 *
 * @retval  0.0.0.0 on error
 */
struct in_addr socket_multicast_addr_by_name(const char * service);
uint16_t socket_multicast_port_by_name(const char * service);


/**
 * Looks up a udp service name by port number in the socket address.
 * Will try internal look up table if /etc/services lookup fails.
 *
 * NOTE: Port number should be in network order,
 *       this is already the case if it was returned from a socket operation.
 *
 * @param   addr    Socket address structure.
 * @param   buf     Buffer for service name.
 * @param   bufSize Number of bytes in the buffer available.
 *
 * @return  Length of the name of the service, in bytes.
 *
 * @retval  0   On failed lookup.
 * @retval  -1  On error.
 */
int socket_get_name_by_addr(struct sockaddr_in * addr, char * buf, size_t bufSize);


/**
 * Uses blocking system calls to setup an IPC port, transmit the given
 * UDP data, and receive a single response.  This is a utility function meant
 * to simplify development of client decoder programs.  Because it is blocking
 * you should not use it in FSW service processes
 *
 * @param   dstAddr  The name (e.g., watchdog) of the destination process
 * @param   dstProc  The IP address, in a dotted quad string, of the
 *                     destination host.  Pass NULL to use the current host.
 * @param   txCmd    The bytes to send in the UDP packet, including the command
 *                     byte.
 * @param   txCmdLen The total length of the txCmd buffer
 * @param   rxResp   A pointer to an allocated buffer to hold the response,
 *                     or NULL to forgo receiving a response.
 * @param   rxRespLen   The maximum length of the rxResp buffer
 * @param   responseTimeoutMS The amount of time to wait, in ms, for the
 *                               response before giving up
 *
 * @return  The actual number of bytes in the received response, or a
 *             negative number if an error occured.
 */
int socket_send_packet_and_read_response(const char *dstAddr,
      const char *dstProc, void *txCmd, size_t txCmdLen,
      void *rxResp, size_t rxRespLen, int responseTimeoutMS);

/**
  * Resolve a host name or dotted-quad into an in_addr
  */
int socket_resolve_host(const char *host, struct in_addr *addr);

#ifdef __cplusplus
}
#endif

#endif
