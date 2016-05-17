 #include "contiki.h"
 #include "dev/serial-line.h"
 #include <stdio.h>
 
 PROCESS(test_serial, "Serial line test process");
 AUTOSTART_PROCESSES(&test_serial);
 
 PROCESS_THREAD(test_serial, ev, data)
 {
   PROCESS_BEGIN();
 
while(1) {
     PROCESS_YIELD();
     if(ev == serial_line_event_message) {
     	if((char*)data=="msg"){
     		printf("ir8e minima!!!%s\n", );
     	}
       printf("received line: %s\n", (char *)data);
     }
   }
   PROCESS_END();
 }