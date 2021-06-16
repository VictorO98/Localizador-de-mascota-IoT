/*******************************************************************************
*
*  pMLusrConf.h -   Manifest Constants and Types for concurrent access to a
*                   circular buffer modelling a message queue
*
*   Notes:          Defined according to the Pet Locator project
*
*******************************************************************************/

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
    sLoginUI,   
    sEliminarMascotaUI,
    sRegistrarMascotaUI,
    sRegistrarUsuarioUI,
    sBuscarMascotaUI,
    sActualizarInfoUI
} TO_InterfazHumanoComputador_From_UI;                             /* Signals sent to process Interfaz humano computador from user*/

typedef enum
{
    sEstadoTransaccionUI,
    sPosUI
} TO_InterfazHumanoComputador_From_Controlador;                    /* Signals sent to process Interfaz humano computador from Controlador */

typedef enum
{
    sValidarUsuarioCo,
    sEliminarMascotaCo,
    sRegistrarMascotaCo,
    sRegistrarUsuarioCo,
    sBuscarMascotaCo,
    sActualizarInfoCo
} TO_Controlador_From_InterfazHumanoComputador;                     /* Signals sent to process Controlador from Interfaz humano computador */

typedef enum
{
    sEstadoTransaccionCo,
    sPosCo
} TO_Controlador_From_Servicios;                                    /* Signals sent to process Controlador from Servicios */

typedef enum
{
    sValidarUsuarioSe,
    sEliminarMascotaSe,
    sRegistrarMascotaSe,
    sRegistrarUsuarioSe,
    sBuscarMascotaSe,
    sActualizarInfoSe
} TO_Servicios_From_Controlador;                                    /* Signals sent to process Servicios from Controlador */

typedef enum
{
    sEstadoTransaccionA,
    sEstadoTransaccionB,
    sEstadoTransaccionC,
    sEstadoTransaccionD,
    sPosSe
} TO_Servicios_From_ConsultarInfo;                                    /* Signals sent to process Servicios from Consultar info */

typedef enum
{
    sValidarUsuarioCI,
    sEliminarMascotaCI,
    sRegistrarMascotaCI,
    sRegistrarUsuarioCI,
    sBuscarMascotaCI,
    sActualizarInfoCI 
} TO_ConsultarInfo_From_Servicios;                                    /* Signals sent to process Consultar info from servicios */

typedef enum
{
    sPosCI
} TO_ConsultarInfo_From_Localizador;                                   /* Signals sent to process Consultar info from localizador */

typedef enum
{
    sEstadoTransaccionCI
} TO_ConsultarInfo_From_GestorBD;                                      /* Signals sent to process Consultar info from Gestor BD */

typedef enum
{
    sPedirDireccionLoc
} TO_Localizador_From_ConsultarInfo;                                    /* Signals sent to Localizador from Consultar info */

typedef enum
{
    sValidarUsuarioBD,
    sEliminarMascotaBD,
    sRegistrarMascotaBD,
    sRegistrarUsuarioBD,
    sBuscarMascotaBD,
    sActualizarInfoBD
} TO_GestorBD_From_ConsultarInfo;                                      /* Signals sent to process Gestor BD from Consultar info */
 
/***( EFSM states )************************************************************/

typedef enum
{
    WaitUIS,
    UnloggedUIS,
    LoggedUIS,
    RegisteringUIS,
    UpdatingUIS,
    DeletingPetUIS,
    RegisteringPetUIS,
    LocatingPetUIS,
    SearchingUIS,
} InterfazHumano_States;

typedef enum
{
    DeletingPetCS,
    IdleCS,
    LocatingPetCS,
    LoginCS,
    RegisteringCS,
    RegisteringPetCS
} Controlador_State; 

typedef enum
{
    DeletingPetCIS,
    IdleCIS,
    LocatingPetCIS,
    WaitCIS,
    RegisteringCIS,
    RegisteringPetCIS
} Consultar_Info_State;

typedef enum
{
    IdleLoc
} Localizador_States;

typedef enum
{
    WaitBD
} Gestor_States;

typedef enum {
	IdleAU, WaitAU
} AGREGARUSUARIO_States;

typedef enum {
	IdleAM, WaitAM
} AGREGARMASCOTA_States;

typedef enum {
	IdleLU, WaitLU
} LOGINUSUARIO_States;

typedef enum {
	IdleLM, WaitLM
} LOCALIZARMASCOTA_States;

typedef enum {
	IdleEM, WaitEM
} ELIMINARMASCOTA_States;
