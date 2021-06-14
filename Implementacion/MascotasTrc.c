#include    <stdio.h>
#include    <string.h>
#include    <errno.h>
#include    "phtrdsMsgLyr.h"              /* pthreads message layer fucntion prototypes, 
                                              constants and structs */

static void *pInterfazHumanoComputador ( void *arg );         /* Interfaz Humano Computador process code */
static void *pControlador ( void *arg );                      /* Controlador process code */
static void *SERVICIOAGREGARUSUARIO ( void *arg );
static void *SERVICIOAGREGARMASCOTA ( void *arg );
static void *SERVICIOLOGINUSUARIO ( void *arg );
static void *SERVICIOLOCALIZARMASCOTA ( void *arg );
static void *SERVICIOELIMINARMASCOTA ( void *arg );
static void *pConsultarInfo( void *arg );                     /* Consultar info process code */
static void *pLocalizador( void *arg );                       /* Localizador process code */
static void *pGestorBD( void *arg );                          /* GestorBD process code */

int main ( void )
{
  pthread_t   InterfazHumanoComputador_tid;                   /* Interfaz humano computador process tid */
  pthread_t   Controlador_tid;                                /* Controlador process tid */
  pthread_t   Servicio_AgregarUsuario_tid;                    /* Servicio agregar usuario process tid */
  pthread_t   Servicio_AgregarMascota_tid;                    /* Servicio agregar mascota process tid */
  pthread_t   Servicio_LoginUsuario_tid;                      /* Servicio Login process tid */
  pthread_t   Servicio_LocalizarMascota_tid;                  /* Servicio localizar masctoa process tid */
  pthread_t   Servicio_EliminarMascota_tid;                   /* Servicio eliminar mascota process tid */
  pthread_t   ConsultarInfo_tid;                              /* Consultar info tid */
  pthread_t   Localizador_tid;                                /* Localizador tid */
  pthread_t   GestorBD_tid;                                   /* Gestor BD tid */

  /* Create queues */
  initialiseQueues ();

  /* Create threads */
  pthread_create ( &InterfazHumanoComputador_tid, NULL, pInterfazHumanoComputador, NULL );
  pthread_create ( &Controlador_tid, NULL, pControlador, NULL );
  pthread_create ( &Servicio_AgregarUsuario_tid, NULL, SERVICIOAGREGARUSUARIO, NULL );
  pthread_create ( &Servicio_AgregarMascota_tid, NULL, SERVICIOAGREGARMASCOTA, NULL );
  pthread_create ( &Servicio_LoginUsuario_tid, NULL, SERVICIOLOGINUSUARIO, NULL );
  pthread_create ( &Servicio_LocalizarMascota_tid, NULL, SERVICIOLOCALIZARMASCOTA, NULL );
  pthread_create ( &Servicio_EliminarMascota_tid, NULL, SERVICIOELIMINARMASCOTA, NULL );
  pthread_create ( &ConsultarInfo_tid, NULL, pConsultarInfo, NULL );
  pthread_create ( &Localizador_tid, NULL, pLocalizador, NULL );
  pthread_create ( &GestorBD_tid, NULL, pGestorBD, NULL );

  /* Wait for threads to finish */
  pthread_join ( InterfazHumanoComputador_tid, NULL );
  pthread_join ( Controlador_tid, NULL );
  pthread_join ( Servicio_AgregarUsuario_tid, NULL);
  pthread_join ( Servicio_AgregarMascota_tid, NULL);
  pthread_join ( Servicio_LoginUsuario_tid, NULL);
  pthread_join ( Servicio_LocalizarMascota_tid, NULL);
  pthread_join ( Servicio_EliminarMascota_tid, NULL);
  pthread_join ( ConsultarInfo_tid, NULL );
  pthread_join ( Localizador_tid, NULL );
  pthread_join ( GestorBD_tid, NULL );

  /* Destroy queues */
  destroyQueues ();

  return ( 0 );
}

/* Interfaz humano computador process thread */
static void *pInterfazHumanoComputador ( void *arg)
{
  InterfazHumano_States   state, state_next;
  msg_t                   InMsg, OutMsg;

  unsigned int us, pd, dtoUs, existeUsuario, estado, mID, dtoMascota, lat, lon;

  printf ( "Interfaz humano computador started...\n" );
  us = 0;
  pd = 0;
  dtoUs = 0;
  existeUsuario = 0;
  estado = 0;
  mID = 0;
  dtoMascota = 0;
  lat = 0;
  lon = 0;
  LOC_req = 1;
  state_next = Unlogged;

  for ( ; LOC_req < 8 ; )
  {
    state = state_next
    InMsg = receiveMessage ( &(queue [INTERFAZHC_Q]) );
    printf ( "Interfaz received signal %d, value A: %d  value B: %d in state %d\n", InMsg.signal, InMsg.valueA, InMsg.valueb, state );
    fflush ( stdout );
    switch ( state )
    {
      case Unlogged:
        switch (InMsg.signal)
        {
          case sLogin:
            us = InMsg.valueA;
            pd = InMsg.valueB;

            OutMsg.signal = sValidarUsuario;
            OutMsg.valueA = us;
            OutMsg.valueB = pd;
            printf ( "packet %d to controlador\n",  OutMsg.signal );
            fflush ( stdout );
            sendMessage ( &(queue [CONTROLADOR_Q]), OutMsg ); /* send message to Controlador process */
            
            state_next = Wait;
          case sRegistrarUsuario:
            dtoUs = InMsg.valueA;

            OutMsg.signal = sRegistrarUsuario;
            OutMsg.valueA = dtoUs;
            printf ( "packet %d to controlador\n",  OutMsg.signal );
            fflush ( stdout );
            sendMessage ( &(queue [CONTROLADOR_Q]), OutMsg ); /* send message to Controlador process */
            
            state_next = Registering;
          default:
            break;
        }
        break;
      
      case Wait:
        switch (InMsg.signal)
        {
          case sEstadoTransaccion:
            existeUsuario = InMsg.valueA;

            printf ( "User logged...");
            fflush ( stdout );
            
            state_next = logged;
          default:
            break;
        }
        break;

      case Registering:
        switch (InMsg.signal)
        {
          case sEstadoTransaccion:
            estado = InMsg.valueA;

            printf ( "User registered...");
            fflush ( stdout );
            
            state_next = Unlogged;
          default:
            break;
        }
        break;

      case Logged:
        switch (InMsg.signal)
        {
          case sActualizarInfo:
            us = InMsg.valueA;
            mID = InMsg.valueB;

            OutMsg.signal = sActualizarInfo;
            OutMsg.valueA = us;
            OutMsg.valueB = mID;
            printf ( "packet %d to controlador\n",  OutMsg.signal );
            fflush ( stdout );
            sendMessage ( &(queue [CONTROLADOR_Q]), OutMsg ); /* send message to Controlador process */
            
            state_next = Updating;
          case sEliminarMascota:
            us = InMsg.valueA;
            mID = InMsg.valueB;

            OutMsg.signal = sEliminarMascota;
            OutMsg.valueA = us;
            OutMsg.valueB = mID;
            printf ( "packet %d to controlador\n",  OutMsg.signal );
            fflush ( stdout );
            sendMessage ( &(queue [CONTROLADOR_Q]), OutMsg ); /* send message to Controlador process */
            
            state_next = DeletingPet;
          case sRegistrarMascota:
            us = InMsg.valueA;
            dtoMascota = InMsg.valueB;

            OutMsg.signal = sRegistrarMascota;
            OutMsg.valueA = us;
            OutMsg.valueB = mID;
            printf ( "packet %d to controlador\n",  OutMsg.signal );
            fflush ( stdout );
            sendMessage ( &(queue [CONTROLADOR_Q]), OutMsg ); /* send message to Controlador process */
            
            state_next = RegisteringPet;
          case sBuscarMascota:
            us = InMsg.valueA;
            mID = InMsg.valueB;

            OutMsg.signal = sBuscarMascota;
            OutMsg.valueA = us;
            OutMsg.valueB = mID;
            printf ( "packet %d to controlador\n",  OutMsg.signal );
            fflush ( stdout );
            sendMessage ( &(queue [CONTROLADOR_Q]), OutMsg ); /* send message to Controlador process */
            
            state_next = Searching;
          default:
            break;
        }
        break;

      case Updating:
        switch (InMsg.signal)
        {
          case sEstadoTransaccion:
            estado = InMsg.valueA;

            printf ( "Updating...");
            fflush ( stdout );
            
            state_next = Logged;
          default:
            break;
        }
        break;

      case DeletingPet:
        switch (InMsg.signal)
        {
          case sEstadoTransaccion:
            estado = InMsg.valueA;

            printf ( "Deleting pet...");
            fflush ( stdout );
            
            state_next = Logged;
          default:
            break;
        }
        break;

      case RegisteringPet:
        switch (InMsg.signal)
        {
          case sEstadoTransaccion:
            estado = InMsg.valueA;

            printf ( "Registering pet...");
            fflush ( stdout );
            
            state_next = Logged;
          default:
            break;
        }
        break;

      case Searching:
        switch (InMsg.signal)
        {
          case sPos:
            lat = InMsg.valueA;
            lon = InMsg.valueB;
            us =  InMsg.valueC;
            mID = InMsg.valueD;

            printf ( "Finding pet...");
            fflush ( stdout );
            
            state_next = Logged;
          default:
            break;
        }
        break;
    }
    printf ( "Interfaz next state is %d\n", state_next );
    fflush ( stdout );
  }
  printf ( "Interfaz exiting...\n" );
  fflush ( stdout );

  return ( NULL );
}

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


