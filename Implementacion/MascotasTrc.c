#include    <stdio.h>
#include    <string.h>
#include    <errno.h>
#include    "phtrdsMsgLyr.h"              /* pthreads message layer fucntion prototypes, 
                                              constants and structs */

static void *SERVICIOAGREGARUSUARIO ( void *arg );
static void *SERVICIOAGREGARMASCOTA ( void *arg );
static void *SERVICIOLOGINUSUARIO ( void *arg );
static void *SERVICIOLOCALIZARMASCOTA ( void *arg );
static void *SERVICIOELIMINARMASCOTA ( void *arg );

static void *SERVICIOAGREGARUSUARIO ( void *arg ){
	AGREGARUSUARIO_States state,
                        state_next;
    msg_t                 InMsg,
                        OutMsg;
    state_next =Idle;
    for ( ; ; ){
    	state = state_next;
    	InMsg = receiveMessage ( &(queue [CONTROLLER_Q]) );
  		switch ( state )
    	{
    		case Idle:
    			switch(InMsg.signal)
    			{
    				case sRegistrarUsuario:
    					OutMsg.signal = (int) sRegistrarUsuario;
            			OutMsg.value = InMsg.value;
    					sendMessage ( &(queue [CONSULTARI_Q]), OutMsg );
    					state_next=Wait;
    					break;
    				default:
    					break;
				}
				break;
			case Wait:
    			switch(InMsg.signal)
    			{
    				case sEstadoTransaccionA:
    					OutMsg.signal = (int) sEstadoTransaccion;
            			OutMsg.value = InMsg.value;
    					sendMessage ( &(queue [CONTROLADOR_Q ]), OutMsg );
    					state_next=Idle;
    					break;
    				default:
    					break;
				}
				break;
    				
    	}
	}
	  return ( NULL );
}

static void *SERVICIOAGREGARMASCOTA ( void *arg ){
	
	AGREGARMASCOTA_States state,
                        state_next;
    msg_t                 InMsg,
                        OutMsg;
    state_next =Idle;
    for ( ; ; ){
    	state = state_next;
    	InMsg = receiveMessage ( &(queue [CONTROLLER_Q]) );
  		switch ( state )
    	{
    		case Idle:
    			switch(InMsg.signal)
    			{
    				case sRegistrarUsuario:
    					OutMsg.signal = (int) sRegistrarMascota;
            			OutMsg.value = InMsg.value;
    					sendMessage ( &(queue [CONSULTARI_Q]), OutMsg );
    					state_next=Wait;
    					break;
    				default:
    					break;
				}
				break;
			case Wait:
    			switch(InMsg.signal)
    			{
    				case sEstadoTransaccionB:
    					OutMsg.signal = (int) sEstadoTransaccion;
            			OutMsg.value = InMsg.value;
    					sendMessage ( &(queue [CONTROLADOR_Q ]), OutMsg );
    					state_next=Idle;
    					break;
    				default:
    					break;
				}
				break;
    				
    	}
	}
	  return ( NULL );
}

static void *SERVICIOLOGINUSUARIO ( void *arg ){
	
	
	
	LOGINUSUARIO_States state,
                        state_next;
    msg_t                 InMsg,
                        OutMsg;
    state_next =Idle;
    for ( ; ; ){
    	state = state_next;
    	InMsg = receiveMessage ( &(queue [CONTROLLER_Q]) );
  		switch ( state )
    	{
    		case Idle:
    			switch(InMsg.signal)
    			{
    				case sValidarUsuario:
    					OutMsg.signal = (int) sRegistrarMascota;
            			OutMsg.value = InMsg.value;
    					sendMessage ( &(queue [CONSULTARI_Q]), OutMsg );
    					state_next=Wait;
    					break;
    				default:
    					break;
				}
				break;
			case Wait:
    			switch(InMsg.signal)
    			{
    				case sEstadoTransaccion:
    					OutMsg.signal = (int) sEstadoTransaccion;
            			OutMsg.value = InMsg.value;
    					sendMessage ( &(queue [CONTROLADOR_Q ]), OutMsg );
    					state_next=Idle;
    					break;
    				default:
    					break;
				}
				break;
    				
    	}
	}
	  return ( NULL );
}

static void *SERVICIOLOCALIZARMASCOTA ( void *arg ){
	
	
	
	LOCALIZARMASCOTA_States state,
                        state_next;
    msg_t                 InMsg,
                        OutMsg;
    state_next =Idle;
    for ( ; ; ){
    	state = state_next;
    	InMsg = receiveMessage ( &(queue [CONSULTARI_Q]) );
  		switch ( state )
    	{
    		case Idle:
    			switch(InMsg.signal)
    			{
    				case sbuscarmascota:
    					OutMsg.signal = (int) sbuscarmascota;
            			OutMsg.value = InMsg.value;
    					sendMessage ( &(queue [CONSULTARI_Q]), OutMsg );
    					state_next=Wait;
    					break;
    				default:
    					break;
				}
				break;
			case Wait:
    			switch(InMsg.signal)
    			{
    				case sPos:
    					OutMsg.signal = (int) sPos;
            			OutMsg.value = InMsg.value;
    					sendMessage ( &(queue [CONTROLADOR_Q ]), OutMsg );
    					state_next=Idle;
    					break;
    				default:
    					break;
				}
				break;
    				
    	}
	}
	  return ( NULL );
}

static void *SERVICIOELIMINARMASCOTA ( void *arg ){
	
	
	
	ELIMINARMASCOTA_States; state,
                        state_next;
    msg_t                 InMsg,
                        OutMsg;
    state_next =Idle;
    for ( ; ; ){
    	state = state_next;
    	InMsg = receiveMessage ( &(queue [CONSULTARI_Q]) );
  		switch ( state )
    	{
    		case Idle:
    			switch(InMsg.signal)
    			{
    				case sEliminarMascota:
    					OutMsg.signal = (int) sEliminarMascota;
            			OutMsg.value = InMsg.value;
    					sendMessage ( &(queue [CONSULTARI_Q]), OutMsg );
    					state_next=Wait;
    					break;
    				default:
    					break;
				}
				break;
			case Wait:
    			switch(InMsg.signal)
    			{
    				case sEstadoTransaccionD:
    					OutMsg.signal = (int) sEstadoTransaccion;
            			OutMsg.value = InMsg.value;
    					sendMessage ( &(queue [CONTROLADOR_Q ]), OutMsg );
    					state_next=Idle;
    					break;
    				default:
    					break;
				}
				break;
    				
    	}
	}
	  return ( NULL );
}


