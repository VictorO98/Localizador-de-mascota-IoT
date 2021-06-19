#include    <stdio.h>
#include    <string.h>
#include    <errno.h>
#include    <pthread.h>
#include    <semaphore.h>
#include    "phtrdsMsgLyr.h"              /* pthreads message layer fucntion prototypes, constants and structs */

/* COMPILE gcc MascotasTrc.c -pthread  ./a.out */


#define     BUFSIZE        8     /* number of slots in queues */
#define     NUM_QUEUES     10    /* number of queues */
#define     INTERFAZHC_Q   0     /* queue 0: Interfaz humano computador process */
#define     CONTROLADOR_Q  1     /* queue 1: Controlador process */
#define     AGREGARUSU_Q   2     /* queue 2: Servicio Agregar Usuario process */
#define     AGREGARMAS_Q   3     /* queue 3: Servicio Agregar Mascota process */
#define     LOGIN_Q        4     /* queue 4: Servicio Login process */
#define     LOCALIZAR_Q    5     /* queue 5: Servicio localizar process */
#define     ELIMINARMAS_Q  6     /* queue 6: Servicio eliminar mascota process */
#define     CONSULTARI_Q   7     /* queue 7: Consultar info process */
#define     LOCALIZADOR_Q  8     /* queue 8: Localizador process */
#define     GESTORBD_Q     9     /* queue 9: Gestor process */
#define     NUM_QUEUES     10    /* number of queues */
#define		MAX_PETS	   10

msgq_t      queue [NUM_QUEUES];             /* declare queue as an array of
                                                message queues  */ 

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
  initialiseQueues();

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

  //enum TO_Controlador_From_InterfazHumanoComputador ControlMessage;

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
  state_next = UnloggedUIS;


  for ( ; ; )
  {
    state = state_next;
    InMsg = receiveMessage ( &(queue [INTERFAZHC_Q]) );
    printf ( "Interfaz received signal %d, value A: %d  value B: %d in state %d\n", InMsg.signal, InMsg.valueA, InMsg.valueB, state );
    fflush ( stdout );
    switch ( state )
    {
      case UnloggedUIS:
        switch (InMsg.signal)
        {
          case sLoginUI:
            us = InMsg.valueA;
            pd = InMsg.valueB;

            OutMsg.signal = (int) sValidarUsuarioCo;
            OutMsg.valueA = us;
            OutMsg.valueB = pd;
            printf ( "packet %d to controlador\n",  OutMsg.signal );
            fflush ( stdout );
            sendMessage ( &(queue [CONTROLADOR_Q]), OutMsg ); /* send message to Controlador process */
            
            state_next = WaitUIS;

          case sRegistrarUsuarioUI:
            dtoUs = InMsg.valueA;

            OutMsg.signal = (int) sRegistrarUsuarioCo;
            OutMsg.valueA = dtoUs;
            printf ( "packet %d to controlador\n",  OutMsg.signal );
            fflush ( stdout );
            sendMessage ( &(queue [CONTROLADOR_Q]), OutMsg ); /* send message to Controlador process */
            
            state_next = RegisteringUIS;
          default:
            break;
        }
        break;
      
      case WaitUIS:
        switch (InMsg.signal)
        {
          case sEstadoTransaccionUI:
            existeUsuario = InMsg.valueA;

            printf ( "User logged...");
            fflush ( stdout );
          
            state_next = LoggedUIS;
          default:
            break;
        }
        break;

      case RegisteringUIS:
        switch (InMsg.signal)
        {
          case sEstadoTransaccionUI:
            estado = InMsg.valueA;

            printf ( "User registered...");
            fflush ( stdout );
            
            state_next = UnloggedUIS;
          default:
            break;
        }
        break;

      case LoggedUIS:
        switch (InMsg.signal)
        {
          case sActualizarInfoUI:
            us = InMsg.valueA;
            mID = InMsg.valueB;

            OutMsg.signal = (int) sActualizarInfoCo;
            OutMsg.valueA = us;
            OutMsg.valueB = mID;
            printf ( "packet %d to controlador\n",  OutMsg.signal );
            fflush ( stdout );
            sendMessage ( &(queue [CONTROLADOR_Q]), OutMsg ); /* send message to Controlador process */
            
            state_next = UpdatingUIS;

          case sEliminarMascotaUI:
            us = InMsg.valueA;
            mID = InMsg.valueB;

            OutMsg.signal = (int) sEliminarMascotaCo;
            OutMsg.valueA = us;
            OutMsg.valueB = mID;
            printf ( "packet %d to controlador\n",  OutMsg.signal );
            fflush ( stdout );
            sendMessage ( &(queue [CONTROLADOR_Q]), OutMsg ); /* send message to Controlador process */
            
            state_next = DeletingPetUIS;

          case sRegistrarMascotaUI:
            us = InMsg.valueA;
            dtoMascota = InMsg.valueB;

            OutMsg.signal = (int) sRegistrarMascotaCo;
            OutMsg.valueA = us;
            OutMsg.valueB = mID;
            printf ( "packet %d to controlador\n",  OutMsg.signal );
            fflush ( stdout );
            sendMessage ( &(queue [CONTROLADOR_Q]), OutMsg ); /* send message to Controlador process */
            
            state_next = RegisteringPetUIS;

          case sBuscarMascotaUI:
            us = InMsg.valueA;
            mID = InMsg.valueB;

            OutMsg.signal = (int) sBuscarMascotaCo;
            OutMsg.valueA = us;
            OutMsg.valueB = mID;
            printf ( "packet %d to controlador\n",  OutMsg.signal );
            fflush ( stdout );
            sendMessage ( &(queue [CONTROLADOR_Q]), OutMsg ); /* send message to Controlador process */
            
            state_next = SearchingUIS;
          default:
            break;
        }
        break;

      case UpdatingUIS:
        switch (InMsg.signal)
        {
          case sEstadoTransaccionUI:
            estado = InMsg.valueA;

            printf ( "Updating...");
            fflush ( stdout );
            
            state_next = LoggedUIS;
          default:
            break;
        }
        break;

      case DeletingPetUIS:
        switch (InMsg.signal)
        {
          case sEstadoTransaccionUI:
            estado = InMsg.valueA;

            printf ( "Deleting pet...");
            fflush ( stdout );
            
            state_next = LoggedUIS;
          default:
            break;
        }
        break;

      case RegisteringPetUIS:
        switch (InMsg.signal)
        {
          case sEstadoTransaccionUI:
            estado = InMsg.valueA;

            printf ( "Registering pet...");
            fflush ( stdout );
            
            state_next = LoggedUIS;
          default:
            break;
        }
        break;

      case SearchingUIS:
        switch (InMsg.signal)
        {
          case sPosUI:
            lat = InMsg.valueA;
            lon = InMsg.valueB;
            us =  InMsg.valueC;
            mID = InMsg.valueD;

            printf ( "Finding pet...");
            fflush ( stdout );
            
            state_next = LoggedUIS;
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

static void *pControlador (void *arg)
{
  Controlador_State   state, state_next;
  msg_t               InMsg, OutMsg;

  unsigned int s, us, mID, dtoMascota, dtoUs, pd, m, lat, lon, estado;

  printf ( "Controlador started...\n" );
  state_next = IdleCS;
  s = 0;
  us = 0;
  mID = 0;
  dtoMascota = 0;
  dtoUs = 0;
  pd = 0;
  m = 0;
  lat = 0;
  lon = 0;
  estado = 0;

  for ( ; ; )
  {
	  state = state_next;
    InMsg = receiveMessage ( &(queue [CONTROLADOR_Q]) );
    printf ( "Controlador received signal %d, value A: %d  value B: %d in state %d\n", InMsg.signal, InMsg.valueA, InMsg.valueB, state );
    fflush ( stdout );
	switch ( state )
	{
		case DeletingPetCS:
			switch (InMsg.signal)
			{
				case sEstadoTransaccionCo:
					s = InMsg.valueA;

					OutMsg.signal = (int) sEstadoTransaccionUI;
					OutMsg.valueA = s;
					printf ( "packet %d to Interfaz humano computador\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [INTERFAZHC_Q]), OutMsg ); /* send message to Interfaz humano computadorprocess */
					
					state_next = IdleCS;
				default:
					break;
			}
 
		case IdleCS:
			switch (InMsg.signal)
			{
				case sEliminarMascotaCo:
					us = InMsg.valueA;
					mID = InMsg.valueB;

					OutMsg.signal = (int) sEliminarMascotaSe;
					OutMsg.valueA = us;
					OutMsg.valueB = mID;
					printf ( "packet %d to Eliminar mascota service\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [ELIMINARMAS_Q]), OutMsg ); /* send message to Eliminar mascota service process */

					state_next = DeletingPetCS;
				
				case sRegistrarMascotaCo:
					us = InMsg.valueA;
					dtoMascota = InMsg.valueB;

					OutMsg.signal = (int) sRegistrarUsuarioSe;
					OutMsg.valueA = us;
					OutMsg.valueB = dtoMascota;
					printf ( "packet %d to Registrar mascota service\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [AGREGARMAS_Q]), OutMsg ); /* send message to Agregar mascota service process */

					state_next = RegisteringPetCS;

				case sRegistrarUsuarioCo:
					dtoUs = InMsg.valueA;
					
					OutMsg.signal = (int) sRegistrarUsuarioSe;
					OutMsg.valueA = dtoUs;
					printf ( "packet %d to Registrar usuario service\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [AGREGARUSU_Q]), OutMsg ); /* send message to Agregar usuario service process */

					state_next = RegisteringCS;

				case sValidarUsuarioCo:
					us = InMsg.valueA;
					pd = InMsg.valueB;

					OutMsg.signal = (int) sValidarUsuarioSe;
					OutMsg.valueA = us;
					OutMsg.valueB = pd;
					printf ( "packet %d to Validar usuario service\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [LOGIN_Q]), OutMsg ); /* send message to Login service process */

					state_next = LoginCS;

				case sBuscarMascotaCo:
					us = InMsg.valueA;
					m = InMsg.valueB;

					OutMsg.signal = (int) sBuscarMascotaSe;
					OutMsg.valueA = us;
					OutMsg.valueB = m;
					printf ( "packet %d to Buscar Mascota service\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [LOCALIZAR_Q]), OutMsg ); /* send message to Buscar mascota service process */

					state_next = LocatingPetCS;
				default:
					break;
			}
		
		case LocatingPetCS:
			switch (InMsg.signal)
			{
				case sPosCo:
					lat = InMsg.valueA;
					lon = InMsg.valueB;
					us = InMsg.valueC;
					mID = InMsg.valueD;

					OutMsg.signal = (int) sPosUI;
					OutMsg.valueA = lat;
					OutMsg.valueB = lon;
					OutMsg.valueC = us;
					OutMsg.valueD = mID;
					printf ( "packet %d to Interfaz humando computador\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [INTERFAZHC_Q]), OutMsg ); /* send message to Interfaz humano computador process */

					state_next = IdleCS;
				default:
					break;
			}

		case LoginCS:
			switch (InMsg.signal)
			{
				case sEstadoTransaccionCo:
					s = InMsg.valueA;

					OutMsg.signal = (int) sEstadoTransaccionUI;
					OutMsg.valueA = s;
					printf ( "packet %d to Interfaz humando computador\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [INTERFAZHC_Q]), OutMsg ); /* send message to Interfaz humano computador process */

					state_next = IdleCS;
				default: 
					break;
			}

		case RegisteringCS:
			switch (InMsg.signal)
			{
				case sEstadoTransaccionCo:
					estado = InMsg.valueA;

					OutMsg.signal = (int) sEstadoTransaccionUI;
					OutMsg.valueA = estado;
					printf ( "packet %d to Interfaz humando computador\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [INTERFAZHC_Q]), OutMsg ); /* send message to Interfaz humano computador process */

					state_next = IdleCS;
				default: 
					break;
			}

		case RegisteringPetCS:
			switch (InMsg.signal)
			{
				case sEstadoTransaccionCo:
					s = InMsg.valueA;

					OutMsg.signal = (int) sEstadoTransaccionUI;
					OutMsg.valueA = s;
					printf ( "packet %d to Interfaz humando computador\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [INTERFAZHC_Q]), OutMsg ); /* send message to Interfaz humano computador process */

					state_next = IdleCS;
				default: 
					break;
			}	

		
		break;
	}
	printf ( "Controlador next state is %d\n", state_next );
    fflush ( stdout );
  }
  printf ( "Controlador exiting...\n" );
  fflush ( stdout );

  return ( NULL );
}

static void *SERVICIOAGREGARUSUARIO ( void *arg )
{
	AGREGARUSUARIO_States state,
                        state_next;
    msg_t                 InMsg,
                        OutMsg;

	printf ( "Servicio agregar usuario started...\n" );
    state_next =IdleAU;
    for ( ; ; ){
    	state = state_next;
    	InMsg = receiveMessage ( &(queue [AGREGARUSU_Q]) );
		printf ( "Servicio agregar usuario received signal %d, value A: %d  value B: %d in state %d\n", InMsg.signal, InMsg.valueA, InMsg.valueB, state );
    	fflush ( stdout );
  		switch ( state )
    	{
    		case IdleAU:
    			switch(InMsg.signal)
    			{
    				case sRegistrarUsuarioSe:
    					OutMsg.signal = (int) sRegistrarUsuarioCI;
            	OutMsg.valueA = InMsg.valueA;
						  printf ( "packet %d to Consultar info\n",  OutMsg.signal );
						  fflush ( stdout );
    					sendMessage ( &(queue [CONSULTARI_Q]), OutMsg );						
    					state_next=WaitAU;
    					break;
    				default:
    					break;
				}
				break;
			case WaitAU:
    			switch(InMsg.signal)
    			{
    				case sEstadoTransaccionA:
    					OutMsg.signal = (int) sEstadoTransaccionCo;
            	OutMsg.valueA = InMsg.valueA;
						  printf ( "packet %d to Controlador info\n",  OutMsg.signal );
						  fflush ( stdout );
    					sendMessage ( &(queue [CONTROLADOR_Q ]), OutMsg );
    					state_next=IdleAU;
    					break;
    				default:
    					break;
				}
				break;
    				
    	}
		printf ( "Servicio Agregar Usuario next state is %d\n", state_next );
    	fflush ( stdout );
	}
	printf ( "Servicio Agregar Usuario exiting...\n" );
  	fflush ( stdout );
	return ( NULL );
}

static void *SERVICIOAGREGARMASCOTA ( void *arg )
{
	
	AGREGARMASCOTA_States state,
                        state_next;
    msg_t                 InMsg,
                        OutMsg;
	
	printf ( "Servicio agregar mascota started...\n" );
    state_next =IdleAM;
    int index=0;
    for ( ; ; ){
    	state = state_next;
    	InMsg = receiveMessage ( &(queue [AGREGARMAS_Q]) );
		printf ( "Servicio agregar mascota received signal %d, value A: %d  value B: %d in state %d\n", InMsg.signal, InMsg.valueA, InMsg.valueB, state );
    	fflush ( stdout );
  		switch ( state )
    	{
    		case IdleAM:
    			switch(InMsg.signal)
    			{
    				case sRegistrarUsuarioSe:
    					if(index<NUM_PETS){
    						OutMsg.signal = (int) sRegistrarMascotaCI;
            				OutMsg.valueA = InMsg.valueA;
							printf ( "packet %d to Consultar info \n",  OutMsg.signal );
							fflush ( stdout );
    						sendMessage ( &(queue [CONSULTARI_Q]), OutMsg );
    						state_next=WaitAM;
    						index++;
    						int value;
    						value=InMsg.valueB;
    						pthread_create ( &Servicio_LocalizarMascota_tid, NULL, SERVICIOLOCALIZARMASCOTA,&value);
    						
						}
						else{
							printf("Servicio agregar mascota muchas mascotas\n");
						}
    					
    					break;
    				default:
    					break;
				}
				break;
			case WaitAM:
    			switch(InMsg.signal)
    			{
    				case sEstadoTransaccionB:
    					OutMsg.signal = (int) sEstadoTransaccionCo;
            			OutMsg.valueA = InMsg.valueA;
						printf ( "packet %d to Controlador info\n",  OutMsg.signal );
						fflush ( stdout );
    					sendMessage ( &(queue [CONTROLADOR_Q ]), OutMsg );
    					state_next=IdleAM;
    					break;
    				default:
    					break;
				}
				break;
    				
    	}
		printf ( "Servicio Agregar Mascota next state is %d\n", state_next );
    	fflush ( stdout );
	}
	printf ( "Servicio Agregar Mascota exiting...\n" );
  	fflush ( stdout );
	return ( NULL );
}

static void *SERVICIOLOGINUSUARIO ( void *arg )
{
	
	
	
	LOGINUSUARIO_States state,
                        state_next;
    msg_t                 InMsg,
                        OutMsg;
	printf ( "Servicio Login Usuario started...\n" );
    state_next =IdleLU;
    for ( ; ; ){
    	state = state_next;
    	InMsg = receiveMessage ( &(queue [LOGIN_Q]) );
		printf ( "Servicio Login usuario received signal %d, value A: %d  value B: %d in state %d\n", InMsg.signal, InMsg.valueA, InMsg.valueB, state );
    	fflush ( stdout );
  		switch ( state )
    	{
    		case IdleLU:
    			switch(InMsg.signal)
    			{
    				case sValidarUsuarioSe:
    					OutMsg.signal = (int) sRegistrarMascotaCI;
            			OutMsg.valueA = InMsg.valueA;
						printf ( "packet %d to Consultar info \n",  OutMsg.signal );
						fflush ( stdout );
    					sendMessage ( &(queue [CONSULTARI_Q]), OutMsg );
    					state_next=WaitLU;
    					break;
    				default:
    					break;
				}
				break;
			case WaitLU:
    			switch(InMsg.signal)
    			{
    				case sEstadoTransaccionC:
    					OutMsg.signal = (int) sEstadoTransaccionCo;
            			OutMsg.valueA = InMsg.valueA;
						printf ( "packet %d to Controlador \n",  OutMsg.signal );
						fflush ( stdout );
    					sendMessage ( &(queue [CONTROLADOR_Q ]), OutMsg );
    					state_next=IdleLU;
    					break;
    				default:
    					break;
				}
				break;
    				
    	}
		printf ( "Servicio Login Usuario next state is %d\n", state_next );
    	fflush ( stdout );
	}
	printf ( "Servicio Login Usuario exiting...\n" );
  	fflush ( stdout );
	return ( NULL );
}

static void *SERVICIOLOCALIZARMASCOTA ( void *arg )
{
	
	
	
	LOCALIZARMASCOTA_States state,
                        state_next;
    msg_t                 InMsg,
                        OutMsg;
	printf ( "Servicio Localizar Mascota started...\n" );
    state_next =IdleLM;
	
    for ( ; ; ){
    	state = state_next;
    	InMsg = receiveMessage ( &(queue [LOCALIZAR_Q]) );
		printf ( "Servicio Localizar mascota received signal %d, value A: %d  value B: %d in state %d\n", InMsg.signal, InMsg.valueA, InMsg.valueB, state );
    	fflush ( stdout );
  		switch ( state )
    	{
    		case IdleLM:
    			switch(InMsg.signal)
    			{
    				case sBuscarMascotaSe:
    					OutMsg.signal = (int) sBuscarMascotaCI;
            			OutMsg.valueA = InMsg.valueA;
						printf ( "packet %d to Consultar info \n",  OutMsg.signal );
						fflush ( stdout );
    					sendMessage ( &(queue [CONSULTARI_Q]), OutMsg );
    					state_next=WaitLM;
    					break;
    				default:
    					break;
				}
				break;
			case WaitLM:
    			switch(InMsg.signal)
    			{
    				case sPosSe:
    					OutMsg.signal = (int) sPosCo;
            			OutMsg.valueA = InMsg.valueA;
						printf ( "packet %d to Controlador\n",  OutMsg.signal );
						fflush ( stdout );
    					sendMessage ( &(queue [CONTROLADOR_Q ]), OutMsg );
    					state_next=IdleLM;
    					break;
    				default:
    					break;
				}
				break;
    				
    	}
		printf ( "Servicio Localizar mascota next state is %d\n", state_next );
    	fflush ( stdout );
	}
	printf ( "Servicio Localizar mascota exiting...\n" );
  	fflush ( stdout );
	return ( NULL );
}

static void *SERVICIOELIMINARMASCOTA ( void *arg )
{
	
	
	
	ELIMINARMASCOTA_States state,
                        state_next;
    msg_t                 InMsg,
                        OutMsg;

	printf ( "Servicio Eliminar Mascota started...\n" );
    state_next =IdleEM;
    for ( ; ; ){
    	state = state_next;
    	InMsg = receiveMessage ( &(queue [ELIMINARMAS_Q]) );
		printf ( "Servicio Eliminar mascota received signal %d, value A: %d  value B: %d in state %d\n", InMsg.signal, InMsg.valueA, InMsg.valueB, state );
    	fflush ( stdout );
  		switch ( state )
    	{
    		case IdleEM:
    			switch(InMsg.signal)
    			{
    				case sEliminarMascotaSe:
    					OutMsg.signal = (int) sEliminarMascotaCI;
            			OutMsg.valueA = InMsg.valueA;
						printf ( "packet %d to Consultar info\n",  OutMsg.signal );
						fflush ( stdout );
    					sendMessage ( &(queue [CONSULTARI_Q]), OutMsg );
    					state_next=WaitEM;
    					break;
    				default:
    					break;
				}
				break;
			case WaitEM:
    			switch(InMsg.signal)
    			{
    				case sEstadoTransaccionD:
    					OutMsg.signal = (int) sEstadoTransaccionCo;
            			OutMsg.valueA = InMsg.valueA;
						printf ( "packet %d to Controlador\n",  OutMsg.signal );
						fflush ( stdout );
    					sendMessage ( &(queue [CONTROLADOR_Q ]), OutMsg );
    					state_next=IdleEM;
    					break;
    				default:
    					break;
				}
				break;
    				
    	}
		printf ( "Servicio Eliminar Mascota next state is %d\n", state_next );
    	fflush ( stdout );
	}
	printf ( "Servicio Localizar mascota exiting...\n" );
  	fflush ( stdout );
	return ( NULL );
}

static void *pConsultarInfo (void *arg)
{
	Consultar_Info_State   state, state_next;
  msg_t               InMsg, OutMsg;

  printf ( "Consultar Info started...\n" );
  state_next = IdleCIS;

  for ( ; ; )
  {
	  state = state_next;
    InMsg = receiveMessage ( &(queue [CONSULTARI_Q]) );
    printf ( "Consultar info received signal %d, value A: %d  value B: %d in state %d\n", InMsg.signal, InMsg.valueA, InMsg.valueB, state );
    fflush ( stdout );
	switch ( state )
	{
		case DeletingPetCIS:
			switch (InMsg.signal)
			{
				case sEstadoTransaccionCI:
					OutMsg.signal = (int) sEstadoTransaccionD;
					OutMsg.valueA = InMsg.valueA;
					printf ( "packet %d to Servicio Eliminar mascota\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [ELIMINARMAS_Q]), OutMsg ); 
					state_next = IdleCIS;
				default:
					break;
			}

		case IdleCIS:
			switch (InMsg.signal)
			{
				case sEliminarMascotaCI:
					OutMsg.signal = (int) sEliminarMascotaBD;
					OutMsg.valueA = InMsg.valueA;
					OutMsg.valueB = InMsg.valueB;
					printf ( "packet %d to Gestor BD\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [GESTORBD_Q]), OutMsg ); 
					state_next = DeletingPetCIS;
				
				case sRegistrarMascotaCI:
					OutMsg.signal = (int) sRegistrarMascotaBD;
					OutMsg.valueA = InMsg.valueA;
					OutMsg.valueB = InMsg.valueB;
					printf ( "packet %d to Gestor BD\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [GESTORBD_Q]), OutMsg );
					state_next = RegisteringPetCIS;

				case sRegistrarUsuarioCI:
					OutMsg.signal = (int) sRegistrarUsuarioBD;
					OutMsg.valueA = InMsg.valueA;
					printf ( "packet %d to Gestor BD\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [GESTORBD_Q]), OutMsg ); 
					state_next = RegisteringCIS;

				case sValidarUsuarioCI:
					OutMsg.signal = (int) sValidarUsuarioBD;
					OutMsg.valueA = InMsg.valueA;
					OutMsg.valueB = InMsg.valueB;
					printf ( "packet %d to Gestor BD\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [GESTORBD_Q]), OutMsg );
					state_next = WaitCIS;

				case sBuscarMascotaCI:
					OutMsg.signal = (int) sBuscarMascotaBD;
					OutMsg.valueA = InMsg.valueA;
					OutMsg.valueB = InMsg.valueB;
					printf ( "packet %d to Gestor BD\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [GESTORBD_Q]), OutMsg ); 
					state_next = LocatingPetCIS;
				default:
					break;
			}
		
		case LocatingPetCIS:
			switch (InMsg.signal)
			{
				case sPosCI:
					OutMsg.signal = (int) sPosSe;
					OutMsg.valueA = InMsg.valueA;
					OutMsg.valueB =  InMsg.valueB;
					OutMsg.valueC = InMsg.valueC;
					OutMsg.valueD = InMsg.valueD;
					printf ( "packet %d to Servicio localizar mascota Service\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [LOCALIZAR_Q]), OutMsg ); 
					state_next = IdleCIS;
				default:
					break;
			}

		case WaitCIS:
			switch (InMsg.signal)
			{
				case sEstadoTransaccionCI:
					OutMsg.signal = (int) sEstadoTransaccionC;
					OutMsg.valueA = InMsg.valueA;
					printf ( "packet %d to Servicio Login Service\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [LOGIN_Q]), OutMsg ); 
					state_next = IdleCIS;
				default: 
					break;
			}

		case RegisteringCIS:
			switch (InMsg.signal)
			{
				case sEstadoTransaccionCI:
					OutMsg.signal = (int) sEstadoTransaccionA;
					OutMsg.valueA = InMsg.valueA;
					printf ( "packet %d to Registro Usuario Service\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [AGREGARUSU_Q]), OutMsg ); 
					state_next = IdleCIS;
				default: 
					break;
			}

		case RegisteringPetCIS:
			switch (InMsg.signal)
			{
				case sEstadoTransaccionCI:
					OutMsg.signal = (int) sEstadoTransaccionB;
					OutMsg.valueA = InMsg.valueA;
					printf ( "packet %d to Registro Mascota Service\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [INTERFAZHC_Q]), OutMsg ); 
					state_next = IdleCIS;
				default: 
					break;
			}	

		
		break;
	}
	printf ( "Consultar Infor next state is %d\n", state_next );
    fflush ( stdout );
  }
  printf ( "Consutlar Info exiting...\n" );
  fflush ( stdout );

  return ( NULL );
}

static void *pLocalizador ( void *arg)
{
	Localizador_States   state, state_next;
  	msg_t               InMsg, OutMsg;

	unsigned int lat, lon, localizatorlat, localizatorlon;

  	printf ( "Localizador Mascota started...\n" );
  	state_next = IdleLoc;
	lat = 0;
	lon = 0;
	localizatorlat = 1;
	localizatorlon = 1;
	int valor=*(int*)arg;
	for ( ; ; )
	{
		state = state_next;
		InMsg = receiveMessage ( &(queue [LOCALIZADOR_Q]) );
		if(InMsg.valueB==valor){
		
			printf ( "Localizador Mascota received signal %d, value A: %d  value B: %d in state %d\n", InMsg.signal, InMsg.valueA, InMsg.valueB, state );
			fflush ( stdout );
	
			switch ( state )
			{
				case IdleLoc:
					switch (InMsg.signal)
					{
						case sPedirDireccionLoc:
							lat = localizatorlat;
							lon = localizatorlon;
	
							OutMsg.signal = (int) sPosCI;
							OutMsg.valueA = lat;
							OutMsg.valueB = lon;
							OutMsg.valueC = InMsg.valueC;
							OutMsg.valueD = InMsg.valueD;
							printf ( "packet %d to Consultar info\n",  OutMsg.signal );
							fflush ( stdout );
							sendMessage ( &(queue [CONSULTARI_Q]), OutMsg ); 
							state_next = IdleLoc;
						default: 
							break;
					}
				break;
			}
		}
		else{
			sendMessage ( &(queue [LOCALIZADOR_Q]), InMsg ); 
		}
		printf ( "Localizador Mascota next state is %d\n", state_next );
    	fflush ( stdout );
	}
	printf ( "Consutlar Info exiting...\n" );
  	fflush ( stdout );
  	return ( NULL );
}

static void *pGestorBD( void *arg )
{
	Gestor_States  state, state_next;
	msg_t               InMsg, OutMsg;

	unsigned int estado, existe;
	
	printf ( "Gestor BD started...\n" );
  state_next = WaitBD;
	estado = 0;
	existe = 0;

	for ( ; ; )
	{
		state = state_next;
		InMsg = receiveMessage ( &(queue [GESTORBD_Q]) );
		printf ( "Gestor BD received signal %d, value A: %d  value B: %d in state %d\n", InMsg.signal, InMsg.valueA, InMsg.valueB, state );
		fflush ( stdout );
		
		switch (state)
		{
			case WaitBD:
				switch (InMsg.signal)
				{
					case sEliminarMascotaBD:
						OutMsg.signal = (int) sEstadoTransaccionCI;
						OutMsg.valueA = existe;
						printf ( "packet %d to Consutlar info\n",  OutMsg.signal );
						fflush ( stdout );
						sendMessage ( &(queue [CONSULTARI_Q]), OutMsg ); 
						state_next = WaitBD;
					
					case sRegistrarMascotaBD:
						OutMsg.signal = (int) sEstadoTransaccionCI;
						OutMsg.valueA = existe;
						printf ( "packet %d to Consutlar info\n",  OutMsg.signal );
						fflush ( stdout );
						sendMessage ( &(queue [CONSULTARI_Q]), OutMsg ); 
						state_next = WaitBD;

					case sRegistrarUsuarioBD:
						estado = 1;
						OutMsg.signal = (int) sEstadoTransaccionCI;
						OutMsg.valueA = estado;
						printf ( "packet %d to Consutlar info\n",  OutMsg.signal );
						fflush ( stdout );
						sendMessage ( &(queue [CONSULTARI_Q]), OutMsg ); 
						state_next = WaitBD;

					case sValidarUsuarioBD:
						estado = 1;
						OutMsg.signal = (int) sEstadoTransaccionCI;
						OutMsg.valueA = estado;
						printf ( "packet %d to Consutlar info\n",  OutMsg.signal );
						fflush ( stdout );
						sendMessage ( &(queue [CONSULTARI_Q]), OutMsg ); 
						state_next = WaitBD;

					case sBuscarMascotaBD:
						OutMsg.signal = (int) sEstadoTransaccionCI;
						OutMsg.valueA = existe;
						printf ( "packet %d to Consutlar info\n",  OutMsg.signal );
						fflush ( stdout );
						sendMessage ( &(queue [CONSULTARI_Q]), OutMsg ); 
						state_next = WaitBD;
					default:
						break;
				}

			break;
		}

		printf ( "Gestor BD next state is %d\n", state_next );
    	fflush ( stdout );
	}
	printf ( "Gestor BD exiting...\n" );
  	fflush ( stdout );
  	return ( NULL );

}


static void PutMsg ( msgq_t *queue_ptr, msg_t msg )
{
  pthread_mutex_lock ( &(queue_ptr->buffer_lock) );
  queue_ptr->buffer [queue_ptr->bufin] = msg;
  queue_ptr->bufin = ( queue_ptr->bufin + 1 ) % BUFSIZE;
  pthread_mutex_unlock ( &(queue_ptr->buffer_lock) );
}

static msg_t GetMsg ( msgq_t *queue_ptr )
{
  msg_t msg;
 
  pthread_mutex_lock ( &(queue_ptr->buffer_lock) );
  msg = queue_ptr->buffer [queue_ptr->bufout];
  queue_ptr->bufout = ( queue_ptr->bufout + 1 ) % BUFSIZE;
  pthread_mutex_unlock ( &(queue_ptr->buffer_lock) );

  return ( msg );
}

void initialiseQueues ( void )
{
  int i;

  for ( i = 0; i < NUM_QUEUES; i++ )
  {
    queue [i].bufin = 0;
    queue [i].bufout = 0;
    pthread_mutex_init ( &(queue [i].buffer_lock), NULL );
    /* queue [i].buffer_lock = PTHREAD_MUTEX_INITIALIZER; */
    /* Create semaphores */
    sem_init ( &(queue [i].items), LOCAL, 0 );          /* There are no messages */
    sem_init ( &(queue [i].slots), LOCAL, BUFSIZE );    /* There are BUFSIZE slots */
  }
}

void destroyQueues ( void )
{
  int i;

  for ( i = 0; i < NUM_QUEUES; i++ )
  {
    /* Destroy mutex */
    pthread_mutex_destroy ( &(queue [i].buffer_lock) );
    /* Destroy semaphores */
    sem_destroy ( &(queue [i].items) );
    sem_destroy ( &(queue [i].slots) );
  }
}

void sendMessage ( msgq_t *queue_ptr, msg_t msg )
{ 
  sem_wait ( &(queue_ptr->slots) );
  PutMsg ( queue_ptr, msg );
  sem_post ( &(queue_ptr->items) );
}

msg_t receiveMessage ( msgq_t *queue_ptr )
{
  msg_t msg;

  sem_wait ( &(queue_ptr->items) );
  msg = GetMsg ( queue_ptr );
  sem_post ( &(queue_ptr->slots) );

  return ( msg );
}
