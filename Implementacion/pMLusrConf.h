/*******************************************************************************
*
*  pMLusrConf.h -   Manifest Constants and Types for concurrent access to a
*                   circular buffer modelling a message queue
*
*   Notes:          Defined according to the Pet Locator project
*
*******************************************************************************/

#define     BUFSIZE     8     /* number of slots in queues */
#define     NUM_QUEUES  10    /* number of queues */
#define     INTERFAZHC_Q      /* queue 0: Interfaz humano computador process */
#define     CONTROLADOR_Q     /* queue 1: Controlador process */
#define     AGREGARUSU_Q      /* queue 2: Servicio Agregar Usuario process */
#define     AGREGARMAS_Q      /* queue 3: Servicio Agregar Mascota process */
#define     LOGIN_Q           /* queue 4: Servicio Login process */
#define     LOCALIZAR_Q       /* queue 5: Servicio localizar process */
#define     ELIMINARMAS_Q     /* queue 6: Servicio eliminar mascota process */
#define     CONSULTARI_Q      /* queue 7: Consultar info process */
#define     LOCALIZADOR_Q     /* queue 8: Localizador process */
#define     GESTORBD_Q        /* queue 9: Gestor process */

/***( message structure )******************************************************/

typedef struct
{
  int   signal;
  int   valueA;
  int   valueB;
  int   valueC;
  int   valueD;
} msg_t;

/***( signals )****************************************************************/

typedef enum
{
    sOk
} TO_USER;                              /* Signals sent to User */

typedef enum
{
    sLogin,   
    sEliminarMascota,
    sRegistrarMascota,
    sRegistrarUsuario,
    sBuscarMascota,
    aActualizarInfo
} TO_InterfazHumanoComputador_From_UI;                             /* Signals sent to process Interfaz humano computador from user*/

typedef enum
{
    sEstadoTransaccion,
    SPos
} TO_InterfazHumanoComputador_From_Controlador;                    /* Signals sent to process Interfaz humano computador from Controlador */

typedef enum
{
    sValidarUsuario,
    sEliminarMascota,
    sRegistrarMascota,
    sRegistrarUsuario,
    sBuscarMascota,
    sActualizarInfo
} TO_Controlador_From_InterfazHumanoComputador;                     /* Signals sent to process Controlador from Interfaz humano computador */

typedef enum
{
    sEstadoTransaccion,
    SPos
} TO_Controlador_From_Servicios;                                    /* Signals sent to process Controlador from Servicios */

typedef enum
{
    sValidarUsuario,
    sEliminarMascota,
    sRegistrarMascota,
    sRegistrarUsuario,
    sBuscarMascota,
    sActualizarInfo
} TO_ Servicios_From_Controlador;                                    /* Signals sent to process Servicios from Controlador */

typedef enum
{
    sEstadoTransaccionA,
    sEstadoTransaccionB,
    sEstadoTransaccionC,
    sEstadoTransaccionD,
    sPos
} TO_Servicios_From_ConsultarInfo;                                    /* Signals sent to process Servicios from Consultar info */

typedef enum
{
    sValidarUsuario,
    sEliminarMascota,
    sRegistrarMascota,
    sRegistrarUsuario,
    sBuscarMascota,
    sActualizarInfo 
} TO_ConsultarInfo_From_Servicios;                                    /* Signals sent to process Consultar info from servicios */

typedef enum
{
    SPos
} TO_ConsultarInfo_From_Localizador;                                   /* Signals sent to process Consultar info from localizador */

typedef enum
{
    sEstadoTransaccion
} TO_ConsultarInfo_From_GestorBD;                                      /* Signals sent to process Consultar info from Gestor BD */

typedef enum
{
    sPedirDireccion
} TO_Localizador_From_ConsultarInfo;                                    /* Signals sent to Localizador from Consultar info */

typedef enum
{
    sValidarUsuario,
    sEliminarMascota,
    sRegistrarMascota,
    sRegistrarUsuario,
    sBuscarMascota,
    sActualizarInfo 
} TO_GestorBD_From_ConsultarInfo;                                      /* Signals sent to process Gestor BD from Consultar info */
 
/***( EFSM states )************************************************************/

typedef enum
{
    Wait,
    Unlogged,
    Logged,
    Registering,
    Updating,
    DeletingPet,
    RegisteringPet,
    LocatingPet,
    Searching,
} InterfazHumano_States;

typedef enum
{
    DeletingPet,
    Idle,
    LocatingPet,
    Login,
    Registering,
    RegisteringPet
} Controlador_State; 

typedef enum
{
    DeletingPet,
    Idle,
    LocatingPet,
    Wait,
    Registering,
    RegisteringPet
} Consultar_Info_State;

typedef enum
{
    Idle
} Localizador_States;

typedef enum
{
    Wait
} Gestor_States;

typedef enum {
	Idle,Wait
} AGREGARUSUARIO_States;
typedef enum {
	Idle,Wait
} AGREGARMASCOTA_States;
typedef enum {
	Idle,Wait
} LOGINUSUARIO_States;

typedef enum {
	Idle,Wait
} LOCALIZARMASCOTA_States;
typedef enum {
	Idle,Wait
} ELIMINARMASCOTA_States;
