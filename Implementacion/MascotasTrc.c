#include    <stdio.h>
#include    <string.h>
#include    <errno.h>
#include    "phtrdsMsgLyr.h"              /* pthreads message layer fucntion prototypes, constants and structs */


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
  state_next = Unlogged;

  for ( ; ; )
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

            OutMsg.signal = (int) sValidarUsuario;
            OutMsg.valueA = us;
            OutMsg.valueB = pd;
            printf ( "packet %d to controlador\n",  OutMsg.signal );
            fflush ( stdout );
            sendMessage ( &(queue [CONTROLADOR_Q]), OutMsg ); /* send message to Controlador process */
            
            state_next = Wait;
          case sRegistrarUsuario:
            dtoUs = InMsg.valueA;

            OutMsg.signal = (int) sRegistrarUsuario;
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

            OutMsg.signal = (int) sActualizarInfo;
            OutMsg.valueA = us;
            OutMsg.valueB = mID;
            printf ( "packet %d to controlador\n",  OutMsg.signal );
            fflush ( stdout );
            sendMessage ( &(queue [CONTROLADOR_Q]), OutMsg ); /* send message to Controlador process */
            
            state_next = Updating;
          case sEliminarMascota:
            us = InMsg.valueA;
            mID = InMsg.valueB;

            OutMsg.signal = (int) sEliminarMascota;
            OutMsg.valueA = us;
            OutMsg.valueB = mID;
            printf ( "packet %d to controlador\n",  OutMsg.signal );
            fflush ( stdout );
            sendMessage ( &(queue [CONTROLADOR_Q]), OutMsg ); /* send message to Controlador process */
            
            state_next = DeletingPet;
          case sRegistrarMascota:
            us = InMsg.valueA;
            dtoMascota = InMsg.valueB;

            OutMsg.signal = (int) sRegistrarMascota;
            OutMsg.valueA = us;
            OutMsg.valueB = mID;
            printf ( "packet %d to controlador\n",  OutMsg.signal );
            fflush ( stdout );
            sendMessage ( &(queue [CONTROLADOR_Q]), OutMsg ); /* send message to Controlador process */
            
            state_next = RegisteringPet;
          case sBuscarMascota:
            us = InMsg.valueA;
            mID = InMsg.valueB;

            OutMsg.signal = (int) sBuscarMascota;
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

static void *pControlador (void *arg)
{
  Controlador_State   state, state_next;
  msg_t               InMsg, OutMsg;

  unsigned int s, us, mID, dtoMascota, dtoUs, pd, m, lat, lon, estado;

  printf ( "Controlador started...\n" );
  state_next = Idle;
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
	state = state_next
    InMsg = receiveMessage ( &(queue [CONTROLADOR_Q]) );
    printf ( "Controlador received signal %d, value A: %d  value B: %d in state %d\n", InMsg.signal, InMsg.valueA, InMsg.valueb, state );
    fflush ( stdout );
	switch ( state )
	{
		case DeletingPet:
			switch (InMsg.signal)
			{
				case sEstadoTransaccion:
					s = InMsg.valueA;

					OutMsg.signal = (int) sEstadoTransaccion;
					OutMsg.valueA = s;
					printf ( "packet %d to Interfaz humano computador\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [INTERFAZHC_Q]), OutMsg ); /* send message to Interfaz humano computadorprocess */
					
					state_next = Idle;
				default:
					break;
			}

		case Idle:
			switch (InMsg.signal)
			{
				case sEliminarMascota:
					us = InMsg.valueA;
					mID = InMsg.valueB;

					OutMsg.signal = (int) sEliminarMascota;
					OutMsg.valueA = us;
					OutMsg.valueB = mID;
					printf ( "packet %d to Eliminar mascota service\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [ELIMINARMAS_Q]), OutMsg ); /* send message to Eliminar mascota service process */

					state_next = DeletingPet;
				
				case sRegistrarMascota:
					us = InMsg.valueA;
					dtoMascota = InMsg.valueB;

					OutMsg.signal = (int) sRegistrarMascota;
					OutMsg.valueA = us;
					OutMsg.valueB = dtoMascota;
					printf ( "packet %d to Registrar mascota service\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [AGREGARMAS_Q]), OutMsg ); /* send message to Agregar mascota service process */

					state_next = RegisteringPet;

				case sRegistrarUsuario:
					dtoUs = InMsg.valueA;
					
					OutMsg.signal = (int) sRegistrarUsuario;
					OutMsg.valueA = dtoUs;
					printf ( "packet %d to Registrar usuario service\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [AGREGARUSU_Q]), OutMsg ); /* send message to Agregar usuario service process */

					state_next = Registering;

				case sValidarUsuario:
					us = InMsg.valueA;
					pd = InMsg.valueB;

					OutMsg.signal = (int) sValidarUsuario;
					OutMsg.valueA = us;
					OutMsg.valueB = pd;
					printf ( "packet %d to Validar usuario service\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [LOGIN_Q]), OutMsg ); /* send message to Login service process */

					state_next = Login;

				case sBuscarMascota:
					us = InMsg.valueA;
					m = InMsg.valueB;

					OutMsg.signal = (int) sBuscarMascota;
					OutMsg.valueA = us;
					OutMsg.valueB = m;
					printf ( "packet %d to Buscar Mascota service\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [LOCALIZAR_Q]), OutMsg ); /* send message to Buscar mascota service process */

					state_next = LocatingPet;
				default:
					break;
			}
		
		case LocatingPet:
			switch (InMsg.signal)
			{
				case sPos:
					lat = InMsg.valueA;
					lon = InMsg.valueB;
					us = InMsg.valueC;
					mID = InMsg.valueD;

					OutMsg.signal = (int) sPos
					OutMsg.valueA = lat;
					OutMsg.valueB = lon;
					OutMsg.valueC = us;
					OutMsg.valueD = mID;
					printf ( "packet %d to Interfaz humando computador\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [INTERFAZHC_Q]), OutMsg ); /* send message to Interfaz humano computador process */

					state_next = Idle;
				default:
					break;
			}

		case Login:
			switch (InMsg.signal)
			{
				case sEstadoTransaccion:
					s = InMsg.valueA;

					OutMsg.signal = (int) sEstadoTransaccion
					OutMsg.valueA = s;
					printf ( "packet %d to Interfaz humando computador\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [INTERFAZHC_Q]), OutMsg ); /* send message to Interfaz humano computador process */

					state_next = Idle;
				default: 
					break;
			}

		case Registering:
			switch (InMsg.signal)
			{
				case sEstadoTransaccion:
					estado = InMsg.valueA;

					OutMsg.signal = (int) sEstadoTransaccion
					OutMsg.valueA = estado;
					printf ( "packet %d to Interfaz humando computador\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [INTERFAZHC_Q]), OutMsg ); /* send message to Interfaz humano computador process */

					state_next = Idle;
				default: 
					break;
			}

		case RegisteringPet:
			switch (InMsg.signal)
			{
				case EstadoTransaccion:
					s = InMsg.valueA;

					OutMsg.signal = (int) sEstadoTransaccion
					OutMsg.valueA = s;
					printf ( "packet %d to Interfaz humando computador\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [INTERFAZHC_Q]), OutMsg ); /* send message to Interfaz humano computador process */

					state_next = Idle;
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
    state_next =Idle;
    for ( ; ; ){
    	state = state_next;
    	InMsg = receiveMessage ( &(queue [AGREGARUSU_Q]) );
		printf ( "Servicio agregar usuario received signal %d, value A: %d  value B: %d in state %d\n", InMsg.signal, InMsg.valueA, InMsg.valueb, state );
    	fflush ( stdout );
  		switch ( state )
    	{
    		case Idle:
    			switch(InMsg.signal)
    			{
    				case sRegistrarUsuario:
    					OutMsg.signal = (int) sRegistrarUsuario;
            			OutMsg.valueA = InMsg.valueA;
						printf ( "packet %d to Consultar info\n",  OutMsg.signal );
						fflush ( stdout );
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
            			OutMsg.valueA = InMsg.valueA;
						printf ( "packet %d to Controlador info\n",  OutMsg.signal );
						fflush ( stdout );
    					sendMessage ( &(queue [CONTROLADOR_Q ]), OutMsg );
    					state_next=Idle;
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
    state_next =Idle;
    for ( ; ; ){
    	state = state_next;
    	InMsg = receiveMessage ( &(queue [AGREGARMAS_Q]) );
		printf ( "Servicio agregar mascota received signal %d, value A: %d  value B: %d in state %d\n", InMsg.signal, InMsg.valueA, InMsg.valueb, state );
    	fflush ( stdout );
  		switch ( state )
    	{
    		case Idle:
    			switch(InMsg.signal)
    			{
    				case sRegistrarUsuario:
    					OutMsg.signal = (int) sRegistrarMascota;
            			OutMsg.valueA = InMsg.valueA;
						printf ( "packet %d to Consultar info \n",  OutMsg.signal );
						fflush ( stdout );
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
            			OutMsg.valueA = InMsg.valueA;
						printf ( "packet %d to Controlador info\n",  OutMsg.signal );
						fflush ( stdout );
    					sendMessage ( &(queue [CONTROLADOR_Q ]), OutMsg );
    					state_next=Idle;
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
    state_next =Idle;
    for ( ; ; ){
    	state = state_next;
    	InMsg = receiveMessage ( &(queue [LOGIN_Q]) );
		printf ( "Servicio Login usuario received signal %d, value A: %d  value B: %d in state %d\n", InMsg.signal, InMsg.valueA, InMsg.valueb, state );
    	fflush ( stdout );
  		switch ( state )
    	{
    		case Idle:
    			switch(InMsg.signal)
    			{
    				case sValidarUsuario:
    					OutMsg.signal = (int) sRegistrarMascota;
            			OutMsg.valueA = InMsg.valueA;
						printf ( "packet %d to Consultar info \n",  OutMsg.signal );
						fflush ( stdout );
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
            			OutMsg.valueA = InMsg.valueA;
						printf ( "packet %d to Controlador \n",  OutMsg.signal );
						fflush ( stdout );
    					sendMessage ( &(queue [CONTROLADOR_Q ]), OutMsg );
    					state_next=Idle;
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
    state_next =Idle;
	
    for ( ; ; ){
    	state = state_next;
    	InMsg = receiveMessage ( &(queue [LOCALIZAR_Q]) );
		printf ( "Servicio Localizar mascota received signal %d, value A: %d  value B: %d in state %d\n", InMsg.signal, InMsg.valueA, InMsg.valueb, state );
    	fflush ( stdout );
  		switch ( state )
    	{
    		case Idle:
    			switch(InMsg.signal)
    			{
    				case sbuscarmascota:
    					OutMsg.signal = (int) sbuscarmascota;
            			OutMsg.valueA = InMsg.valueA;
						printf ( "packet %d to Consultar info \n",  OutMsg.signal );
						fflush ( stdout );
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
            			OutMsg.valueA = InMsg.valueA;
						printf ( "packet %d to Controlador\n",  OutMsg.signal );
						fflush ( stdout );
    					sendMessage ( &(queue [CONTROLADOR_Q ]), OutMsg );
    					state_next=Idle;
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
	
	
	
	ELIMINARMASCOTA_States; state,
                        state_next;
    msg_t                 InMsg,
                        OutMsg;

	printf ( "Servicio Eliminar Mascota started...\n" );
    state_next =Idle;
    for ( ; ; ){
    	state = state_next;
    	InMsg = receiveMessage ( &(queue [ELIMINARMAS_Q]) );
		printf ( "Servicio Eliminar mascota received signal %d, value A: %d  value B: %d in state %d\n", InMsg.signal, InMsg.valueA, InMsg.valueb, state );
    	fflush ( stdout );
  		switch ( state )
    	{
    		case Idle:
    			switch(InMsg.signal)
    			{
    				case sEliminarMascota:
    					OutMsg.signal = (int) sEliminarMascota;
            			OutMsg.valueA = InMsg.valueA;
						printf ( "packet %d to Consultar info\n",  OutMsg.signal );
						fflush ( stdout );
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
            			OutMsg.valueA = InMsg.valueA;
						printf ( "packet %d to Controlador\n",  OutMsg.signal );
						fflush ( stdout );
    					sendMessage ( &(queue [CONTROLADOR_Q ]), OutMsg );
    					state_next=Idle;
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
  state_next = Idle;

  for ( ; ; )
  {
	state = state_next
    InMsg = receiveMessage ( &(queue [CONSULTARI_Q]) );
    printf ( "Consultar info received signal %d, value A: %d  value B: %d in state %d\n", InMsg.signal, InMsg.valueA, InMsg.valueb, state );
    fflush ( stdout );
	switch ( state )
	{
		case DeletingPet:
			switch (InMsg.signal)
			{
				case sEstadoTransaccion:
					OutMsg.signal = (int) sEstadoTransaccion;
					OutMsg.valueA = InMsg.valueA;
					printf ( "packet %d to Servicio Eliminar mascota\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [ELIMINARMAS_Q]), OutMsg ); 
					state_next = Idle;
				default:
					break;
			}

		case Idle:
			switch (InMsg.signal)
			{
				case sEliminarMascota:
					OutMsg.signal = (int) sEliminarMascota;
					OutMsg.valueA = InMsg.valueA;
					OutMsg.valueB = InMsg.valueB;
					printf ( "packet %d to Gestor BD\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [GESTORBD_Q]), OutMsg ); 
					state_next = DeletingPet;
				
				case sRegistrarMascota:
					OutMsg.signal = (int) sRegistrarMascota;
					OutMsg.valueA = InMsg.valueA;
					OutMsg.valueB = InMsg.valueB;
					printf ( "packet %d to Gestor BD\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [GESTORBD_Q]), OutMsg );
					state_next = RegisteringPet;

				case sRegistrarUsuario:
					OutMsg.signal = (int) sRegistrarUsuario;
					OutMsg.valueA = InMsg.valueA;
					printf ( "packet %d to Gestor BD\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [GESTORBD_Q]), OutMsg ); 
					state_next = Registering;

				case sValidarUsuario:
					OutMsg.signal = (int) sValidarUsuario;
					OutMsg.valueA = InMsg.valueA;
					OutMsg.valueB = InMsg.valueB;
					printf ( "packet %d to Gestor BD\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [GESTORBD_Q]), OutMsg );
					state_next = Wait;

				case sBuscarMascota:
					OutMsg.signal = (int) sBuscarMascota;
					OutMsg.valueA = InMsg.valueA;
					OutMsg.valueB = InMsg.valueB;
					printf ( "packet %d to Gestor BD\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [GESTORBD_Q]), OutMsg ); 
					state_next = LocatingPet;
				default:
					break;
			}
		
		case LocatingPet:
			switch (InMsg.signal)
			{
				case sPos:
					OutMsg.signal = (int) sPos
					OutMsg.valueA = InMsg.valueA;
					OutMsg.valueB =  InMsg.valueB;
					OutMsg.valueC = InMsg.valueC;
					OutMsg.valueD = InMsg.valueD;
					printf ( "packet %d to Servicio localizar mascota Service\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [LOCALIZAR_Q]), OutMsg ); 
					state_next = Idle;
				default:
					break;
			}

		case Wait:
			switch (InMsg.signal)
			{
				case sEstadoTransaccion:
					OutMsg.signal = (int) sEstadoTransaccion
					OutMsg.valueA = InMsg.valueA;
					printf ( "packet %d to Servicio Login Service\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [LOGIN_Q]), OutMsg ); 
					state_next = Idle;
				default: 
					break;
			}

		case Registering:
			switch (InMsg.signal)
			{
				case sEstadoTransaccion:
					OutMsg.signal = (int) sEstadoTransaccion
					OutMsg.valueA = InMsg.valueA;
					printf ( "packet %d to Registro Usuario Service\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [AGREGARUSU_Q]), OutMsg ); 
					state_next = Idle;
				default: 
					break;
			}

		case RegisteringPet:
			switch (InMsg.signal)
			{
				case EstadoTransaccion:
					OutMsg.signal = (int) sEstadoTransaccion
					OutMsg.valueA = InMsg.valueA;
					printf ( "packet %d to Registro Mascota Service\n",  OutMsg.signal );
					fflush ( stdout );
					sendMessage ( &(queue [INTERFAZHC_Q]), OutMsg ); 
					state_next = Idle;
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
  	state_next = Idle;
	lat = 0;
	lon = 0;
	localizatorlat = 1;
	localizatorlon = 1;

	for ( ; ; )
	{
		state = state_next
		InMsg = receiveMessage ( &(queue [LOCALIZADOR_Q]) );
		printf ( "Localizador Mascota received signal %d, value A: %d  value B: %d in state %d\n", InMsg.signal, InMsg.valueA, InMsg.valueb, state );
		fflush ( stdout );

		switch ( state )
		{
			case Idle:
				switch (InMsg.signal)
				{
					case sPedirDireccion:
						lat = localizatorlat;
						lon = localizatorlon;

						OutMsg.signal = (int) sPos;
						OutMsg.valueA = lat;
						OutMsg.valueB = lon;
						OutMsg.valueC = InMsg.valueC;
						OutMsg.valueD = InMsg.valueD;
						printf ( "packet %d to Consultar info\n",  OutMsg.signal );
						fflush ( stdout );
						sendMessage ( &(queue [CONSULTARI_Q]), OutMsg ); 
						state_next = Idle;
					default: 
						break;
				}
			break;
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
  state_next = Wait;
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
			case Wait:
				switch (InMsg.signal)
				{
					case sEliminarMascota:
						OutMsg.signal = (int) sEstadoTransaccion;
						OutMsg.valueA = existe;
						printf ( "packet %d to Consutlar info\n",  OutMsg.signal );
						fflush ( stdout );
						sendMessage ( &(queue [CONSULTARI_Q]), OutMsg ); 
						state_next = Wait;
					
					case sRegistrarMascota:
						OutMsg.signal = (int) sEstadoTransaccion;
						OutMsg.valueA = existe;
						printf ( "packet %d to Consutlar info\n",  OutMsg.signal );
						fflush ( stdout );
						sendMessage ( &(queue [CONSULTARI_Q]), OutMsg ); 
						state_next = Wait;

					case sRegistrarUsuario:
						estado = 1;
						OutMsg.signal = (int) sEstadoTransaccion;
						OutMsg.valueA = estado;
						printf ( "packet %d to Consutlar info\n",  OutMsg.signal );
						fflush ( stdout );
						sendMessage ( &(queue [CONSULTARI_Q]), OutMsg ); 
						state_next = Wait;

					case sValidarUsuario:
						estado = 1;
						OutMsg.signal = (int) sEstadoTransaccion;
						OutMsg.valueA = estado;
						printf ( "packet %d to Consutlar info\n",  OutMsg.signal );
						fflush ( stdout );
						sendMessage ( &(queue [CONSULTARI_Q]), OutMsg ); 
						state_next = Wait;

					case sBuscarMascota:
						OutMsg.signal = (int) sEstadoTransaccion;
						OutMsg.valueA = existe;
						printf ( "packet %d to Consutlar info\n",  OutMsg.signal );
						fflush ( stdout );
						sendMessage ( &(queue [CONSULTARI_Q]), OutMsg ); 
						state_next = Wait;
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