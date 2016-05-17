/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "dev/serial-line.h"
#include <string.h>

#define DEBUG DEBUG_PRINT
#include "net/uip-debug.h"

#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])

#define MAX_PAYLOAD_LEN 120

static struct uip_udp_conn *server_conn;


PROCESS(udp_server_process, "UDP server process");
PROCESS(test_serial, "Serial line test process");
AUTOSTART_PROCESSES(&resolv_process,&test_serial,&udp_server_process);
int DROP_FLAG = NULL;
/*---------------------------------------------------------------------------*/
static void
tcpip_handler(void)
{
   
   if (uip_newdata()) {
    ((char *)uip_appdata)[uip_datalen()] = 0;
    PRINTF("Server received: '%s' from ", (char *)uip_appdata);
    PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
    PRINTF("\n");
    PRINTF("Message received:");
    PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
    PRINTF(" \n");

  }
}
// /*---------------------------------------------------------------------------*/


static void
tcpip_respond(void)
{
  static int seq_id;
  char buf[MAX_PAYLOAD_LEN];

  if (DROP_FLAG==0){
    uip_ipaddr_copy(&server_conn->ripaddr, &UIP_IP_BUF->srcipaddr);
    PRINTF("Responding with message: ");
    sprintf(buf, "Hello from the server! (%d)", ++seq_id);
    PRINTF("%s\n", buf);

    uip_udp_packet_send(server_conn, buf, strlen(buf));
    // Restore server connection to allow data from any node 
    memset(&server_conn->ripaddr, 0, sizeof(server_conn->ripaddr));
    }

}

/*---------------------------------------------------------------------------*/
static void
print_local_addresses(void)
{
  int i;
  uint8_t state;

  PRINTF("Server IPv6 addresses: ");
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    state = uip_ds6_if.addr_list[i].state;
    if(uip_ds6_if.addr_list[i].isused &&
       (state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
      PRINT6ADDR(&uip_ds6_if.addr_list[i].ipaddr);
      PRINTF("\n");
    }
  }
}
/*---------------------------------------------------------------------------*/
static void
print_serial(char* data)
{
  if(strcmp((char*)data, "ctrl_msg") == 0){
        printf("Control message received : %s\n", (char *)data);
  }
  else if (strcmp((char*)data, "Drop packet") == 0){
        //uip_flags & UIP_NEWDATA ;
        DROP_FLAG=1;
        printf("Packet dropped\n");
        }
  else if (strcmp((char*)data, "Accept packet") == 0){
        DROP_FLAG=0;
        printf("Packet Accepted\n");
        }

  else{ 
          printf("received line: %s\n", (char *)data);
  }
     
  }
/*---------------------------------------------------------------------------*/
static void
TX_fingerprint_to_controller(void)
{
    // Here the fingerprint is sent to controller to use the learning algorithm for identification
  PRINTF("fingerprint sent to controller \n");
    
}  

/*---------------------------------------------------------------------------*/


static void
TX_result_to_sensor(void)
{
  // Here the result is forwarded back to the sensor to apply a mitigation strategy
  
  int dstAddr = 205;
  PRINTF("result sent to node %d \n", dstAddr);
    
}
/*---------------------------------------------------------------------------*/  
PROCESS_THREAD(udp_server_process, ev, data)
{
#if UIP_CONF_ROUTER
  uip_ipaddr_t ipaddr;
#endif /* UIP_CONF_ROUTER */

   PROCESS_BEGIN();
   PRINTF("UDP server started\n");

#if RESOLV_CONF_SUPPORTS_MDNS
  resolv_set_hostname("contiki-udp-server");
#endif

#if UIP_CONF_ROUTER
  uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
  uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
  uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);
#endif /* UIP_CONF_ROUTER */

  print_local_addresses();

  server_conn = udp_new(NULL, UIP_HTONS(3001), NULL);
  udp_bind(server_conn, UIP_HTONS(3000));

  while(1) {
    PROCESS_YIELD();
    if(ev == tcpip_event) {
      tcpip_handler();
      PRINTF("1 DROP_FLAG = %d \n",DROP_FLAG);
     
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

 PROCESS_THREAD(test_serial, ev, data)
 {
    PROCESS_BEGIN();
    PRINTF("Test serial started\n");

    uart0_set_input(serial_line_input_byte);
    serial_line_init();
 
    while(1) {
      PROCESS_WAIT_EVENT();
      if(ev == serial_line_event_message) {
        print_serial(data);
        PRINTF("2 DROP_FLAG = %d \n",DROP_FLAG);
         if(DROP_FLAG==0){
        tcpip_respond(); 
      }
      }
      
    }

    PROCESS_END();
}
