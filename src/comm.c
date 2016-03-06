/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   ___           _______     ________      |                             *
 *  |###|         |#######|   |########| (R) |             Lac             *
 *  |###|        |###|^|###| |###|^^|###|    |                             *
 *  |###|        |###| |###| |###|   ^^^     |    ekipa programistyczna:   *
 *  |###|        |###|_|###| |###|           |   Lam, Qwert, Ulryk, Vigud  *
 *  |###|        |#########| |###|           |      Tissarius, Cadaver     *
 *  |###|        |###|^|###| |###|           |        Alandar, Gimza       *
 *  |###|        |###| |###| |###|   ___     |    trzy grosze dolozyli:    *
 *  |###|______  |###| |###| |###|__|###|    |      Varda, Nop, Fuyara     *
 *  |##########| |###| |###|  |########|     |     Albi, Malven, Muzgus    *
 *   ^^^^^^^^^^   ^^^   ^^^    ^^^^^^^^      |                             *
 * ----------------------------------------------------------------------- *
 *  LACMud improvements copyright (C) 1997 - 2012 by Leszek Matok et al.   *
 *  EnvyMud 2.2 improvements copyright (C) 1996, 1997 by Michael Quan.     *
 *  EnvyMud 2.0 improvements copyright (C) 1995 by Michael Quan and        *
 *   Mitchell Tse.                                                         *
 *  Envy Diku Mud improvements copyright (C) 1994 by Michael Quan, David   *
 *   Love, Guilherme 'Willie' Arnold, and Mitchell Tse.                    *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *   Chastain, Michael Quan, and Mitchell Tse.                             *
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *   Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.  *
 * ----------------------------------------------------------------------- *
 *  In order to use any part of this Lac Diku Mud, you must comply with    *
 *  the original Diku license in 'license.doc', the Merc license in        *
 *  'license.txt', the Envy license in 'license.nvy', as well as the Lac   *
 *  license in 'license.lac'. In particular, you may not remove either of  *
 *  these copyright notices.                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * This file contains all of the OS-dependent stuff:
 *   startup, signals, BSD sockets for tcp/ip, i/o, timing.
 *
 * The data flow for input is:
 *    Game_loop ---> Read_from_descriptor ---> Read
 *    Game_loop ---> Read_from_buffer
 *
 * The data flow for output is:
 *    Game_loop ---> Process_Output ---> Write_to_descriptor -> Write
 *
 * The OS-dependent functions are Read_from_descriptor and Write_to_descriptor.
 * -- Furey  26 Jan 1993
 */

#include "merc.h"
#include "update.h"


#if defined( WIN32 )
char version_str[] = "$VER: LACMud Windows 32 Bit Version";
#endif

#if defined( AmigaTCP )
char version_str[] = "$VER: LACMud AmiTCP Version";
/*
 * You must rename or delete the sc:sys/types.h, so the
 * amitcp:netinclude/sys/types.h will be used instead.
 * Also include these assigns in your user-startup (After the SC assigns)
 *    assign lib: Amitcp:netlib add
 *    assign include: Amitcp:netinclude add
 * If you haven't allready :)
 * Compilled with SasC 6.56 and AmiTCP 4.2
 * http://www.geocities.com/SiliconValley/1411
 * dkaupp@netcom.com (May be defunct soon)
 * or davek@megnet.net
 * 4-16-96
 */
#endif

#if defined( MSDOS )
char version_str[] = "$VER: LACMud DOS Version";
#endif

#if !defined( WIN32 ) && !defined( AmigaTCP ) && !defined( MSDOS )
char version_str[] = "$VER: LACMud *NIX";
#endif

#if !defined( WIN32 )
#include <sys/time.h>
#endif

#if defined( AmigaTCP )
/* Vigud: nie wiem, jakie pliki beda chcialy inne amigowe systemy. Testowalem
   wylacznie na AROSie i tam jest tak, jak ponizej. Na innych amigowych zapewne
   nie ma zadnego <aros/symbolsets.h>. */
#include <proto/exec.h>
#include <proto/bsdsocket.h>
#include <aros/symbolsets.h>
struct Library *SocketBase;
#endif

#include <errno.h>
#include <stdio.h>
#include <stdarg.h> /* va_list */

/* byc moze to wystarczy, zeby pozbyc sie problemow z WIN32 */
#if defined( IPV6 )
# define SOCK_AF AF_INET6
#else
# if defined( PF_INET )
#  define SOCK_AF PF_INET
# else
#  define SOCK_AF AF_INET
# endif
#endif

#if defined( MSDOS ) && !defined( NO_SOUND )
#include <allegro.h> /* Allegro wykorzystuje chwilowo do odtwarzania MIDI */
#endif

#if defined( WIN32 )
#include <winsock.h>
#include <sys/timeb.h> /* for _ftime( ), uses _timeb struct */
#else
# define closesocket close
#endif

#if defined( unix ) && !defined( AmigaTCP ) && !defined( WIN32 )
#include <signal.h>
#endif

#if defined( unix ) || defined( AmigaTCP )
# include <fcntl.h>
# include <netdb.h>
# include <netinet/in.h>
# include <sys/socket.h>
# if !defined( __minix ) && !defined( PLAN9 )
#  define TELCMDS
#  define TELOPTS
#  include <arpa/telnet.h>
# endif
# include <arpa/inet.h>
#else
# define socklen_t (unsigned int)
#endif

#if defined( PLAN9 )
# include <sys/select.h>
#endif

#if defined( WIN32 )
# define IS_INVALID_SOCKET( s ) ( ( s ) == ( 0 ) )
#else
# define IS_INVALID_SOCKET( s ) ( ( s ) < ( 0 ) )
#endif

#if !defined( IAC )
# if defined( MSDOS )
#  define	IAC	0
# else
#  define	IAC	255
# endif
#define	DONT		254
#define	DO		253
#define	WONT		252
#define	WILL		251
#define	SB		250
#define	GA		249
#define	SE		240
#define	TELOPT_NAWS	31
#define	TELOPT_TTYPE	24
#define TELOPT_ECHO	1
#define TELOPT_BINARY	0
#define	TELQUAL_SEND	1
#define	TELQUAL_IS	0
#endif

#if defined( WIN32 )
int	gettimeofday	args( ( struct timeval *tp, void *tzp ) );
#endif

#if defined( MSDOS )
int	kbhit	args( ( void ) );
#endif

#if defined( ultrix )
/* Ultrix 4.5 po prostu nie ma deklaracji w naglowkach systemowych.
   Oryginalny fragment z Envy 2.2, ale usunalem read() i write(), bo z kolei
   gcc ma wbudowane deklaracje. --Lam */
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
void	bzero		args( ( char *b, int length ) );
int	close		args( ( int fd ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			       fd_set *exceptfds, struct timeval *timeout ) );
int	setsockopt	args( ( int s, int level, int optname, void *optval,
			       int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
#endif


const unsigned char echo_off_str[ ]	= { IAC, WILL, TELOPT_ECHO, '\0' };
const unsigned char echo_on_str[ ]	= { IAC, WONT, TELOPT_ECHO, '\0' };
/* linuksowy telnet zawsze odpowiada na DO ECHO (chyba zgodnie z RFC), ale
   zmud nie chce sie powtarzac, wiec: */
const unsigned char keepalive_str[ ]	= { IAC, WILL, TELOPT_ECHO, IAC, WONT, TELOPT_ECHO, '\0' };
const unsigned char go_ahead_str[ ]	= { IAC, GA, '\0' };
const unsigned char term_type_str[ ]	= { IAC, DO, TELOPT_TTYPE };
const unsigned char get_ttype_str[ ]	= { IAC, SB, TELOPT_TTYPE, TELQUAL_SEND, IAC, SE };
const unsigned char naws_str[ ]		= { IAC, DO, TELOPT_NAWS };
const unsigned char binary_in[ ]	= { IAC, DO, TELOPT_BINARY };
const unsigned char binary_out[ ]	= { IAC, WILL, TELOPT_BINARY };


/*
 * Global variables.
 */
DESCRIPTOR_DATA *   descriptor_free;    /* Free list for descriptors    */
DESCRIPTOR_DATA *   descriptor_list;    /* All open descriptors         */
WHO_DESCRIPTOR_DATA *who_descriptor_free;
WHO_DESCRIPTOR_DATA *who_descriptor_list;
DESCRIPTOR_DATA *   d_next;             /* Next descriptor in loop      */
FILE *              fpReserve;          /* Reserved file handle         */
FILE *              fpBugReserve;       /* osobny plik dla bledow       */
bool                merc_down;          /* Shutdown                     */
bool                wizlock;            /* Game is wizlocked            */
int                 numlock = 0;        /* Game is numlocked at <level> */
char                str_boot_time     [ MAX_INPUT_LENGTH ];
time_t              current_time;       /* Time of this pulse           */

#if defined ( MSDOS )
# if !defined( NO_SOUND )
SAMPLE *            damsound   [ 8 ];
SAMPLE *            deathsound [ 4 ];
# endif
#else
# if defined( IPV6 )
struct sockaddr_in6 who_sa;
# else
struct sockaddr_in who_sa;
# endif
unsigned int       who_socket;

unsigned int LAC_port; /* przeniesione z main( ) */
#endif /* defined ( MSDOS ) */


/*
 * Lam 25.4.98: obsluga InterMuda w Lacu
 */
#if defined( IMUD )
int                imud_socket; /* gniazdo polaczone z IM-em */
#if !defined( MSDOS )
struct sockaddr_in imud_sa;
bool               imud_down; /* jesli TRUE, to nie ma polaczenia z IM-em */
#else
bool               imud_down = TRUE; /* pod dosem nie ma IM-a */
#endif
char              *imud_outbuf; /* tedy wychodza dane do imuda */
int                imud_outtop;
int                imud_outsize;
#endif

/*
 * Lam 11.3.98: rekordy ilosci graczy i polaczen, od restartu i wszechczasow
 * rekordy dzienne dopisal Ulryk
 */
struct stats_type max_play;
int max_play_all_time;
char max_play_all_time_time[ 200 ];
struct stats_type max_desc;
int max_desc_all_time;
char max_desc_all_time_time[ 200 ];


/*
 * OS-dependent local functions.
 */
#if defined( MSDOS )
void    game_loop_mac_msdos     args( ( void ) );
bool    read_from_descriptor    args( ( DESCRIPTOR_DATA *d ) );
#endif

#if defined( unix ) || defined( AmigaTCP ) || defined( WIN32 )
void    game_loop_unix          args( ( int control ) );
int     init_socket             args( ( unsigned int port ) );
void    new_descriptor          args( ( int control ) );
void    new_who_descriptor      args( ( int control ) );
void	close_who_socket	args( ( WHO_DESCRIPTOR_DATA *dclose ) );
bool    read_from_descriptor    args( ( DESCRIPTOR_DATA *d ) );
#endif




/*
 * Other local functions (OS-independent).
 */
int	licz_linie		args( ( const char *arg ) );
bool	poprawne_haslo		args( ( CHAR_DATA *ch, char *arg ) );
bool    check_reconnect         args( ( DESCRIPTOR_DATA *d, bool fConn ) );
bool    check_playing           args( ( DESCRIPTOR_DATA *d, char *name,
					bool nowa ) );
int     main                    args( ( int argc, char **argv ) );
void    nanny                   args( ( DESCRIPTOR_DATA *d, char *argument ) );
bool    process_output          args( ( DESCRIPTOR_DATA *d, bool fPrompt ) );
bool	process_who_output	args( ( WHO_DESCRIPTOR_DATA *d ) );
void    stop_idling             args( ( CHAR_DATA *ch ) );
void    bust_a_prompt           args( ( DESCRIPTOR_DATA *d ) );
char   *imud_one_command	args( ( const char *txt, char *cmd ) );
void	imud_interp		args( ( const char *txt ) );
void	zawin			args( ( const char *in, char *out, int szer,
					bool uni ) );
void	*welcome		args( ( void *vo ) );
void	interp_telnet_seq	args( ( DESCRIPTOR_DATA *d, int gdzie ) );
const char *odwroc_wyrazy	args( ( const char *zrodlo ) );
char	*prompt_wyjscia		args( ( ROOM_INDEX_DATA *room ) );

/* Lac uruchamiajacy sie tylko w celu zapisania wszystkich krain (lac -k) */
bool TylkoKonwersja;


#if defined( MSDOS ) && !defined( NO_SOUND )
/* funkcje puszczajace probki */
void damage_sound( void )
{
    play_sample( damsound[ number_bits( 3 ) ], 80, 100, 1000, FALSE );

    return;
}

void death_sound( void )
{
    play_sample( deathsound[ number_bits( 2 ) ], 255, 100, 1000, FALSE );

    return;
}
#endif


/* comm_main jest wywolywane z main( ) znajdujacej sie w lac.c albo glaca.c */
int comm_main( bool edytor, int argc, char **argv )
{
    struct timeval now_time;
    bool pszukaj = FALSE;
#if defined( unix ) || defined( AmigaTCP ) || defined( WIN32 )
    int control = 0; /* to zeby nie bylo ostrzezenia kompilatora */
#endif

#if defined( MSDOS ) && !defined( NO_SOUND )
    allegro_init( );
    install_timer( );
    if ( install_sound( 1, 1, argv[ 0 ] ) != 0 )
    {
	printf( "\nError initialising sound system.\n%s\n", allegro_error );
	allegro_exit( );
	exit( 1 );
    }
    printf( "\nSterownik dzwieku: %s\n", digi_driver->name );
    printf( "Szczegoly: %s\n\n", digi_driver->desc );
    printf( "Odtwarzacz MIDI: %s\n", midi_driver->name );
    printf( "Szczegoly: %s\n\n", midi_driver->desc );
    play_midi( load_midi( "lac.mid" ), TRUE );
    {
	int pomoc;
	char diplodok[ MIL ];
	for ( pomoc = 1; pomoc <= 8; pomoc++ )
	{
	    sprintf( diplodok, "wav\\pain%d.wav", pomoc );
	    damsound[ pomoc - 1 ] = load_sample( diplodok );
	}
	for ( pomoc = 1; pomoc <= 4; pomoc++ )
	{
	    sprintf( diplodok, "wav\\death%d.wav", pomoc );
	    deathsound[ pomoc - 1 ] = load_sample( diplodok );
	}
    }
#endif

    /*
     * Init time.
     */
    gettimeofday( &now_time, NULL );
    current_time = (time_t) now_time.tv_sec;
    strcpy( str_boot_time, asctime_pl( localtime( &current_time ) ) );

#if defined( AmigaTCP )
    SocketBase = (APTR) OpenLibrary( "bsdsocket.library", 4L );
#endif

    /*
     * Reserve one channel for our use.
     */
    if ( !( fpReserve = fopen( NULL_FILE, "r" ) ) )
    {
	lac_perror( NULL_FILE );
	exit( 1 );
    }

    /*
     * bug( ) i log_string( ) uzywaja innego pliku zeby nie interferowac
     * z plikami graczy
     */
    if ( !( fpBugReserve = fopen( NULL_FILE, "r" ) ) )
    {
	lac_perror( NULL_FILE );
	exit( 1 );
    }

    /*
     * Get the port number.
     */
#if !defined( MSDOS )
    LAC_port = 4000;
#endif
    /* Kiedys zastosuje sie tu getopt( ), chwilowo nie ma potrzeby */
    if ( argc > 1 )
    {
	int i;
	for ( i = 1; i < argc; i++ )
	{
	    if ( !str_cmp( argv[ i ], "-k" ) )
		TylkoKonwersja = TRUE;
	    else if ( !str_cmp( argv[ i ], "-o" ) )
		ODPLUSKWIACZ = TRUE;
	    else if ( !str_cmp( argv[ i ], "-p" ) )
		PRZERABIACZ = TRUE;
	    else if ( !str_cmp( argv[ i ], "-s" ) )
		pszukaj = TRUE;
#if !defined( MSDOS )
	    else if ( !str_cmp( argv[ i ], "-l" )
		   && i + 1 < argc )
	    {
		sprintf( HTTPD_log, "%s%s.log", HTTPDLOG_DIR, argv[ i + 1 ] );
		i++;
	    }
	    else if ( is_number( argv[ i ] ) )
		LAC_port = atoi( argv[ i ] );
	    else
	    {
		fprintf( stderr, "Uzycie: %s [-k] [-o] [-p] [-s] [-l nr_kroniki_httpd] [port]\n", argv[ 0 ] );
		exit( 1 );
	    }
#else
	    else
	    {
		fprintf( stderr, "Uzycie: %s [-k] [-o] [-p] [-s]\n", argv[ 0 ] );
		exit( 1 );
	    }
#endif
	}
    }

    /*
     * Run the game.
     */
#if defined( MSDOS )
    log_string( "Witaj w dosowym Lacu! Jak wiesz, nie wolno go rozpowszechniac." );
    log_string( "Jesli masz go od kogos, powiadom o tym bogow Laca." );
    log_string( "Listy bogow i ich adresow szukaj na: http://www.lac.pl/" );
    boot_db( edytor );
    if ( !TylkoKonwersja )
    {
	log_string( LAC_VERSION " gotowy do skopania tylkow!" );
	game_loop_mac_msdos( );
    }
    else
    {
	AREA_DATA *kraina;

	sprintf( log_buf, "Lac zapisuje wczytane krainy..." );
	for ( kraina = area_first; kraina; kraina = kraina->next )
	    save_area( kraina, TRUE );
    }
    gettimeofday( &now_time, NULL );
    current_time = (time_t) now_time.tv_sec;
    log_string( log_buf );
#endif

#if defined( unix ) || defined( AmigaTCP ) || defined( WIN32 )
    if ( !TylkoKonwersja )
	control = init_socket( LAC_port );

    if ( !*HTTPD_log || !( httpdlog = fopen( HTTPD_log, "w" ) ) )
	httpdlog = stderr;
    else
	setvbuf( httpdlog, ( char * ) NULL, _IOLBF, 0 );

    log_string( "Rozpoczecie czytania krain..." );
    boot_db( edytor );

    if ( pszukaj )
    {
	log_string( "Odpalono z parametrem -s, generowanie pszukaj.txt." );
	if ( !TylkoKonwersja )
	    log_string( "Uwaga - bez parametru -k, minimalny poziom zostanie zle wyliczony." );
	do_ofind( NULL, "-plik wsz" );
    }

    if ( PRZERABIACZ )
    {
	char argum[ MIL ];

	log_string( "Lac przeistacza sie w przerabiacz." );
	printf( "Podaj argumenty dla pszukaj: " );
	while ( scanf( "%[^\n]", argum ) )
	    ;
	printf( "Odpalam: pszukaj %s\n\r", argum );
	do_ofind( NULL, argum );
    }

    if ( !TylkoKonwersja )
	sprintf( log_buf,
	    "[PID %d] "
	    LAC_VERSION
#if defined( __GNUC__ )
	    " (kompilacja: " __TIME__ " " __DATE__ ", gcc " __VERSION__ ")"
#endif
	    " gotowy do skopania tylkow na porcie %u.", (int) getpid( ), LAC_port );
    else
    {
	if ( edytor )
	    sprintf( log_buf, "Lac przeistacza sie w edytor krain..." );
	else
	    sprintf( log_buf, "Lac zapisuje wczytane krainy..." );
    }
    gettimeofday( &now_time, NULL );
    current_time = (time_t) now_time.tv_sec;
    log_string( log_buf );

    if ( !TylkoKonwersja )
    {
	game_loop_unix( control );
	closesocket( control );
	closesocket( who_socket );
#if defined( WIN32 )
	WSACleanup( );
#endif
    }
    else
    {
	AREA_DATA *kraina;

	if ( edytor )
	    return 0;

	for ( kraina = area_first; kraina; kraina = kraina->next )
	    save_area( kraina, TRUE );
    }
#endif /* uniksy */

    /*
     * That's all, folks.
     */
    gettimeofday( &now_time, NULL );
    current_time = (time_t) now_time.tv_sec;
    log_string( "Lac zakonczyl dzialanie." );

    return 0;
}


/*
 * Init_socket - z lenistwa zrobione nagannie, ale dziala poprawnie.
 * Powinienem zamiast korzystac z globalnych, podawac mu parametry -Lam
 */
#if defined( unix ) || defined( AmigaTCP ) || defined( WIN32 )
int init_socket( unsigned int port )
{
#if defined( IPV6 )
    static struct sockaddr_in6 sa_zero;
	   struct sockaddr_in6 sa;
#else
    static struct sockaddr_in sa_zero;
	   struct sockaddr_in sa;
#endif
		  int         x        = 1;
		  int         fd, who_fd;


#if !defined( WIN32 )
    if ( system( "touch SHUTDOWN.TXT" ) )
	log_string( "Nie udalo sie dotknac SHUTDOWN.TXT" );

    fd = socket( SOCK_AF, SOCK_STREAM, 0 );
    if ( IS_INVALID_SOCKET( fd ) )
    {
	lac_perror( "Init_socket: socket" );
	exit( 1 );
    }

    who_fd = socket( SOCK_AF, SOCK_STREAM, 0 );
    if ( IS_INVALID_SOCKET( who_fd ) )
    {
	lac_perror( "Init_who_socket: socket" );
	exit( 1 );
    }

#if defined( IMUD )
    imud_socket = socket( SOCK_AF, SOCK_STREAM, 0 );
    if ( IS_INVALID_SOCKET( imud_socket ) )
    {
	lac_perror( "Init_imud_socket: socket" );
	exit( 1 );
    }
#endif
#else
    WORD wVersionRequested = MAKEWORD( 1, 1 );
    WSADATA wsaData;
    int err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 )
    {
	lac_perror( "No usable WINSOCK.DLL" );
	exit( 1 );
    }

    fd = socket( SOCK_AF, SOCK_STREAM, 0 );
    if ( IS_INVALID_SOCKET( fd ) )
    {
	lac_perror( "Init_socket: socket" );
	exit( 1 );
    }

    who_fd = socket( SOCK_AF, SOCK_STREAM, 0 );
    if ( IS_INVALID_SOCKET( who_fd ) )
    {
	lac_perror( "Init_who_socket: socket" );
	exit( 1 );
    }
#if defined( IMUD )

    imud_socket = socket( SOCK_AF, SOCK_STREAM, 0 );
    if ( IS_INVALID_SOCKET( imud_socket ) )
    {
	lac_perror( "Init_imud_socket: socket" );
	exit( 1 );
    }
#endif
#endif

    if ( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR,
    (char *) &x, sizeof( x ) ) < 0 )
    {
	lac_perror( "Init_socket: SO_REUSEADDR" );

	closesocket( fd );
	closesocket( who_fd );

	exit( 1 );
    }

    x = 1;

    /* Lam 26.6.2000: bez tego mud miewal problemy ze wstaniem po jebucie na
       Solarisie */
    if ( setsockopt( who_fd, SOL_SOCKET, SO_REUSEADDR,
    (char *) &x, sizeof( x ) ) < 0 )
    {
	lac_perror( "Init_socket: SO_REUSEADDR" );

	closesocket( fd );
	closesocket( who_fd );

	exit( 1 );
    }

/* Lam: tu byl kawalek od SO_DONLINGER. W Linuksie w ogole nie ma tego, jest za
   to SO_LINGER, ktory sluzy do wysylania resztek danych do gniazda przed
   zamknieciem, czyli do robienia opoznienia (10 sek). Niepotrzebne. */

    sa              = sa_zero;
#if defined( IPV6 )
    sa.sin6_family  = SOCK_AF;
    sa.sin6_port    = htons( port );
#else
    sa.sin_family   = SOCK_AF;
    sa.sin_port     = htons( port );
#endif

    if ( bind( fd, (struct sockaddr *) &sa, sizeof( sa ) ) < 0 )
    {
	lac_perror( "Init_socket: bind" );

	closesocket( fd );
	closesocket( who_fd );

	exit( 1 );
    }

/* Port KTO/HTML, standardowo 3999 ( port - 1 ) */

    who_sa            = sa_zero;
#if defined( IPV6 )
    who_sa.sin6_family = SOCK_AF;
    who_sa.sin6_port  = htons( port - 1 );
#else
    who_sa.sin_family = SOCK_AF;
    who_sa.sin_port   = htons( port - 1 );
#endif

    if ( bind( who_fd, (struct sockaddr *) &who_sa, sizeof( who_sa ) ) < 0 )
    {
	lac_perror( "Init_who_socket: bind" );

	closesocket( fd );
	closesocket( who_fd );

	exit( 1 );
    }

#if defined( IMUD )
    imud_sa.sin_family = SOCK_AF;
    imud_sa.sin_port   = htons( IMUD_PORT );
    /* ponizej powinienem uzywac inet_aton */
    imud_sa.sin_addr.s_addr = inet_addr( IMUD_HOST ); /* localhost */

    if ( connect( imud_socket, (struct sockaddr *) &imud_sa,
		sizeof( imud_sa ) ) < 0 )
    {
	lac_perror( "Init_imud_socket: connect" );
	fprintf( stderr, "Podczas gry nie bedzie IM-a.\n" );
	close( imud_socket ); /* nie exituje */
	imud_down = TRUE; /* ale o imudzie tez zapomina */
    }

    if ( !imud_down && fcntl( imud_socket, F_SETFL, O_NONBLOCK ) == -1 )
    {
	lac_perror( "Init_imud_socket: fcntl" );
	close( imud_socket );
    }
#endif

    if ( listen( fd, 3 ) < 0 )
    {
	lac_perror( "Init_socket: listen" );

	closesocket( fd );
	closesocket( who_fd );

	exit( 1 );
    }

    if ( listen( who_fd, 3 ) < 0 )
    {
	lac_perror( "Init_who_socket: listen" );

	closesocket( fd );
	closesocket( who_fd );

	exit( 1 );
    }

    who_socket = who_fd;

#if defined( WIN32 )
    {
	u_long iMode = 1;
	ioctlsocket( who_fd, FIONBIO, &iMode );
    }
#endif

#if !defined( AmigaTCP ) && !defined( WIN32 )
    fcntl( who_fd, F_SETFL, O_NONBLOCK );
#endif

#if defined( IMUD )
    if ( !imud_down )
    {
	write_to_descriptor( imud_socket, "Lac\n", 4 );
    }

    /* i tak bedzie go jeszcze mozna postawic w trakcie gry, wiec nie ma
       potrzeby ciaglego rezerwowania pamieci */
    imud_outsize = 2000;
    imud_outbuf  = alloc_mem( imud_outsize );
#endif

#if !defined( WIN32 )
    if ( system( "rm SHUTDOWN.TXT" ) )
	log_string( "Nie udalo sie dotknac SHUTDOWN.TXT" );
#endif
    return fd;
}
#endif



#if defined( MSDOS )
void game_loop_mac_msdos( void )
{
    static        DESCRIPTOR_DATA dcon;
	   struct timeval         last_time;
	   struct timeval         now_time;

    gettimeofday( &last_time, NULL );
    current_time = (time_t) last_time.tv_sec;

    /*
     * New_descriptor analogue.
     */
    dcon.descriptor     = 0;
    dcon.character      = NULL;
    dcon.connected      = CON_GET_NAME;
    strcpy( dcon.host, "localhost" );
    strcpy( dcon.login, "" );
    strcpy( dcon.user, "user" );
    strcpy( dcon.term_type, "ansi" );
    dcon.szer		= 80;
    dcon.wys		= 25;
    dcon.outsize        = 2000;
    dcon.outbuf         = alloc_mem( dcon.outsize );
    dcon.showstr_head   = str_dup( "" );
    dcon.showstr_point  = 0;
    dcon.next           = descriptor_list;
    descriptor_list     = &dcon;

    {
	extern char *help_greeting;
	if ( help_greeting[ 0 ] == '.' )
	    write_to_buffer( &dcon, help_greeting + 1, 0 );
	else
	    write_to_buffer( &dcon, help_greeting, 0 );
    }

    /* Main loop */
    while ( !merc_down )
    {
	DESCRIPTOR_DATA *d;


	/*
	 * Przed realizacja rzeczy od graczy wywolywane plynne_odradzanie
	 * ustawiajace tez d->wait
	 */
	update_handler_before( );


	/*
	 * Process input.
	 */
	for ( d = descriptor_list; d; d = d_next )
	{
	    d_next      = d->next;
	    d->fcommand = FALSE;

	    if ( kbhit( ) )
	    {
		if ( !read_from_descriptor( d ) )
		{
		    if ( d->character )
			save_char_obj( d->character );
		    d->outtop       = 0;
		    close_socket( d, TRUE );
		    continue;
		}
	    }

	    if ( d->character && d->wait )
	    {
		d->wait = FALSE;
		continue;
	    }

	    read_from_buffer( d, TRUE );
	    if ( d->incomm[ 0 ] != '\0' )
	    {
		d->fcommand     = TRUE;
		stop_idling( d->character );

		if ( d->connected == CON_PLAYING )
		{
		    if ( d->showstr_point )
			show_string( d, d->incomm );
		    else if ( d->character->odmiana )
			odmiana_postaci( d->character, d->incomm );
		    else
			interpret( d->character, d->incomm, INTERP_NORMAL );
		}
		else
		    nanny( d, d->incomm );

		d->incomm[ 0 ] = '\0';
	    }
	}



	/*
	 * Reszta aktualizacji, np. spadanie po ruchu, przyzwyczajanie
	 * do swiatla itd.
	 */
	update_handler( );


	/*
	 * Output.
	 */
	for ( d = descriptor_list; d; d = d_next )
	{
	    d_next = d->next;

	    if ( ( d->fcommand || d->outtop > 0 ) )
	    {
		if ( !process_output( d, TRUE ) )
		{
		    if ( d->character )
			save_char_obj( d->character );
		    d->outtop   = 0;
		    close_socket( d, TRUE );
		}
	    }
	}



	/*
	 * Synchronize to a clock.
	 * Busy wait (blargh).
	 */
	now_time = last_time;
	for ( ; ; )
	{
	    int delta;

	    if ( kbhit( ) )
	    {
		if ( !read_from_descriptor( &dcon ) )
		{
		    if ( dcon.character )
			save_char_obj( d->character );
		    dcon.outtop     = 0;
		    close_socket( &dcon, TRUE );
		}
		break;
	    }

	    gettimeofday( &now_time, NULL );
	    delta = ( now_time.tv_sec  - last_time.tv_sec  ) * 1000 * 1000
		  + ( now_time.tv_usec - last_time.tv_usec );
	    if ( delta >= 1000000 / PULSE_PER_SECOND )
		break;
	}
	last_time    = now_time;
	current_time = (time_t) last_time.tv_sec;
    }

    return;
}
#endif


/*
 * Lam 16.11.1999: nanosleep( ) zamiast select( )
 * Lam 29.12.2002: mala funkcja ladnie wylaczajaca muda po kill -TERM
 */
#if defined( unix )
static void ladne_wyjscie( int sig )
{
    log_string( "Przechwycilem SIGTERM, usiluje wylaczyc sie ladnie." );
    send_to_all_char( "Lac jest zmuszony wy`l`aczy`c si`e. Zapraszamy po przerwie.\n\r" );
    merc_down = TRUE;
    end_of_game( );

    return;
}
#endif

#if defined( unix ) || defined( AmigaTCP ) || defined( WIN32 )
void game_loop_unix( int control )
{
    static struct timeval null_time;
    struct timeval last_time;
    int ile_razy;

#if !defined( AmigaTCP ) && !defined( WIN32 )
    signal( SIGPIPE, SIG_IGN );
    signal( SIGTERM, ladne_wyjscie );
#endif

    gettimeofday( &last_time, NULL );
    current_time = (time_t) last_time.tv_sec;
    ile_razy = 0;

    /* Main loop */
    while ( !merc_down )
    {
	DESCRIPTOR_DATA *d;
	WHO_DESCRIPTOR_DATA *who_d, *who_d_next;
	fd_set           in_set;
	fd_set           out_set;
	fd_set           exc_set;
	unsigned int     maxdesc;

	/*
	 * Poll all active descriptors.
	 */
	FD_ZERO( &in_set  );
	FD_ZERO( &out_set );
	FD_ZERO( &exc_set );
	FD_SET( control, &in_set );
	maxdesc = control;
#if defined( IMUD )
	if ( !imud_down )
	{
	    FD_SET( imud_socket, &in_set );
	    FD_SET( imud_socket, &out_set );
	    FD_SET( imud_socket, &exc_set );
	    maxdesc = UMAX( maxdesc, imud_socket );
	}
#endif
	FD_SET( who_socket, &in_set );
	maxdesc = UMAX( maxdesc, who_socket );
	for ( who_d = who_descriptor_list; who_d; who_d = who_d->next )
	{
	    maxdesc = UMAX( maxdesc, who_d->descriptor );
	    FD_SET( who_d->descriptor, &in_set  );
	    FD_SET( who_d->descriptor, &out_set );
	    FD_SET( who_d->descriptor, &exc_set );
	}

	for ( d = descriptor_list; d; d = d->next )
	{
	    if ( d->welcome_state != WELCOME_DONE ) /* Lam */
		continue;
	    maxdesc = UMAX( maxdesc, d->descriptor );
	    FD_SET( d->descriptor, &in_set  );
	    FD_SET( d->descriptor, &out_set );
	    FD_SET( d->descriptor, &exc_set );
	}

	if ( select( maxdesc + 1, &in_set, &out_set, &exc_set, &null_time ) < 0 )
	{
	    /* malo prawdopodobne */
	    lac_perror( "Game_loop: select: poll" );
	    if ( ile_razy > 5 ) /* 6 razy rzad */
	    {
		send_to_all_char( "Co`s ubi`lo Laca. `Zegnajcie.\n\r" );
		log_string( "Zamykanie Laca po awarii selecta." );
		merc_down = TRUE;
		end_of_game( );
		FD_ZERO( &in_set  ); /* nie bedziemy przyjmowac polaczen */
		FD_ZERO( &exc_set ); /* malo istotne */
		ile_razy = -1;
	    }
	    else if ( ile_razy >= 0 )
	    {
		ile_razy++;
		continue;
	    }
	}
	if ( ile_razy > 0 ) /* mniej niz 6 razy pod rzad, nic sie nie stalo */
	    ile_razy = 0;

	/*
	 * New connection?
	 *
	 * Lam: jedno wywolanie new_descriptor powoduje, ze akceptowane jest
	 * tylko jedno polaczenie na puls, czyli 4 pol./s (PULSE_PER_SECOND).
	 * Moze to spowolnic zautomatyzowane ataki DOS, a wiec uwazam to za
	 * funkcje obronna, a nie blad
	 */
	if ( FD_ISSET( control, &in_set ) )
	    new_descriptor( control );

	/*
	 * KTO na porcie 3999
	 */
	if ( FD_ISSET( who_socket, &in_set ) )
	    new_who_descriptor( who_socket );

	/*
	 * Kick out the freaky folks.
	 */
#if defined( IMUD )
	if ( !imud_down && FD_ISSET( imud_socket, &exc_set ) )
	{
	    FD_CLR( imud_socket, &in_set );
	    FD_CLR( imud_socket, &out_set );
	    imud_down = TRUE;
	}
#endif

	/*
	 * exception || idle timeout || ban  --Lam
	 */
	for ( d = descriptor_list; d; d = d_next )
	{
	    d_next = d->next;

	    if ( d->welcome_state == WELCOME_BAN )
	    {
		d->outtop = 0;
		close_socket( d, TRUE );
		continue;
	    }

	    if ( FD_ISSET( d->descriptor, &exc_set ) )
	    {
		FD_CLR( d->descriptor, &in_set  );
		FD_CLR( d->descriptor, &out_set );
		if ( d->character && d->connected == CON_PLAYING )
		    save_char_obj( d->character );
		d->outtop = 0;
		close_socket( d, TRUE );
		continue;
	    }
	    d->idle++; /* Lam 29.6.98 */

	    /* 2 minuty na podanie imienia, 15 na przeczytanie regulaminu */
	    if ( ( !d->character && d->idle > 480 )
	      || ( d->connected != CON_PLAYING && d->idle > 3600 ) )
	   /* ||   d->idle > 28800 ) - 2 godziny (niesmiertelni) */
	    {
		if ( d->welcome_state != WELCOME_DONE )
		    write_to_descriptor( d->descriptor,
			"\n\rStrasznie dlugo zajmuje mi dowiadywanie sie czegos o tobie.\n\r"
			"Sprawdz, co moze byc przyczyna, a na razie nie moge cie wpuscic.\n\r", 0 );
		else if ( d->character )
		    send_to_char(
			"\n\rStrasznie d`lugo nie dotykasz klawiatury.\n\r"
			"Roz`l`aczam ci`e. Pa.\n\r", d->character );
		else
		    write_to_descriptor( d->descriptor,
			"\n\rStrasznie dlugo nie dotykasz klawiatury.\n\r"
			"Rozlaczam cie. Pa.\n\r", 0 );

		FD_CLR( d->descriptor, &in_set  );
		FD_CLR( d->descriptor, &out_set );
		/* d->outtop = 0; - reszta moze sie przydac, nie zaszkodzi */
		close_socket( d, TRUE );
		continue;
	    }
	}

	for ( who_d = who_descriptor_list; who_d; who_d = who_d_next )
	{
	    who_d_next = who_d->next;

	    who_d->idle++;
	    if ( FD_ISSET( who_d->descriptor, &exc_set )
	      || who_d->idle > 120 ) /* pol minuty polaczenia */
	    {
		FD_CLR( who_d->descriptor, &in_set  );
		FD_CLR( who_d->descriptor, &out_set );
		close_who_socket( who_d );
		continue;
	    }
	}


	/*
	 * Przed realizacja rzeczy od graczy wywolywane plynne_odradzanie
	 * ustawiajace tez d->wait
	 */
	update_handler_before( );


	/*
	 * Process input.
	 */
#if defined( IMUD )
	if ( !imud_down && FD_ISSET( imud_socket, &in_set ) )
	{
	    static char buf[ 16385 ];
	    int przeczytane;

	    przeczytane = read( imud_socket, buf, ( sizeof buf ) - 1 );
	    if ( przeczytane < 1 )
	    {
		if ( przeczytane )
		    lac_perror( "IMud: read" );
		send_to_all_char( "Po`l`aczenie z IM-em przerwane!\n\r" );
		close( imud_socket );
		imud_down = TRUE;
	    }
	    else
	    {
		buf[ przeczytane ] = 0;
		imud_interp( buf );
	    }
	}
#endif

	for ( d = descriptor_list; d; d = d_next )
	{
	    d_next      = d->next;
	    d->fcommand = FALSE;

	    if ( FD_ISSET( d->descriptor, &in_set ) )
	    {
		/* d->idle = 0;
		if ( d->character )
		    d->character->timer = 0; */
		if ( !read_from_descriptor( d ) )
		{
		    FD_CLR( d->descriptor, &out_set );
		    /* Lam 12.2.1998: sprawdzanie CON_PLAYING, w przeciwnym
		       przypadku zapisuja sie postacie ludzi, ktorzy nie
		       wchodzac do gry rozlaczaja sie. Tak niektorzy tracili
		       poziomy, bo mieli dwie sesje: na jednej grali, a druga
		       ich pytala, czy sie polaczyc mimo to. Najpierw wychodzili
		       na jednej, a potem rozlaczali sie na drugiej, dlatego
		       zapisywala sie postac wgrana na samym poczatku. */
		    if ( d->character && d->connected == CON_PLAYING )
			save_char_obj( d->character );
		    d->outtop   = 0;
		    close_socket( d, TRUE );
		    continue;
		}
	    }

	    if ( d->character && d->wait )
	    {
		d->wait = FALSE; /* ustawi to plynne_odradzanie */
		continue;
	    }
	    /* if ( d->character && d->character->wait > 0 )
	    {
		--d->character->wait;
		continue;
	    } */

	    read_from_buffer( d, TRUE );
	    if ( d->incomm[ 0 ] != '\0' )
	    {
		d->fcommand     = TRUE;
		stop_idling( d->character );

		if ( d->connected == CON_PLAYING )
		{
		    if ( d->showstr_point )
			show_string( d, d->incomm );
		    else if ( d->character->odmiana )
			odmiana_postaci( d->character, d->incomm );
		    else
			interpret( d->character, d->incomm, INTERP_NORMAL );
		}
		else
		    nanny( d, d->incomm );

		d->incomm[ 0 ]    = '\0';
	    }
	}

	for ( who_d = who_descriptor_list; who_d; who_d = who_d_next )
	{
	    who_d_next = who_d->next;

	    if ( FD_ISSET( who_d->descriptor, &in_set ) )
	    {
		char whobuf[ MSL ];
		int  koniec;

		/* tutaj powinien byc odczyt zadania HTTP */
		koniec = read( who_d->descriptor, &whobuf, ( sizeof whobuf ) - 1 );
		if ( koniec < 1 )
		{
		    FD_CLR( who_d->descriptor, &in_set  );
		    FD_CLR( who_d->descriptor, &out_set );
		    close_who_socket( who_d );
		    continue;
		}

		whobuf[ koniec ] = '\0';
		if ( !get_who_data( whobuf, who_d ) )
		{
		    http_log( who_d );
		    close_who_socket( who_d );
		    continue;
		}

		http_log( who_d );
		( *szukaj_strony( who_d->odnosnik ) ) ( who_d );
		who_d->cts = TRUE;
	    }
	}


	/*
	 * Reszta aktualizacji, np. spadanie po ruchu, przyzwyczajanie
	 * do swiatla itd.
	 */
	update_handler( );


	/*
	 * Output.
	 */
#if defined( IMUD )
	if ( !imud_down && imud_outtop )
	    if ( write_to_descriptor( imud_socket, imud_outbuf, imud_outtop ) < 0 )
	    {
		close( imud_socket );
		imud_down = TRUE;
	    }
	imud_outtop = 0;
#endif

	for ( d = descriptor_list; d; d = d_next )
	{
	    d_next = d->next;

	    if ( ( d->fcommand || d->outtop > 0 )
		&& FD_ISSET( d->descriptor, &out_set ) )
	    {
		if ( !process_output( d, TRUE ) )
		{
		    if ( d->character && d->connected == CON_PLAYING )
			save_char_obj( d->character );
		    d->outtop   = 0;
		    close_socket( d, TRUE );
		}
	    }
	}

	for ( who_d = who_descriptor_list; who_d; who_d = who_d_next )
	{
	    who_d_next = who_d->next;

	    if ( who_d->cts && FD_ISSET( who_d->descriptor, &out_set ) )
	    {
		if ( !process_who_output( who_d ) )
		{
		    who_d->outtop = 0;
		    close_who_socket( who_d );
		}
	    }
	}


	/*
	 * Synchronize to a clock.
	 * Sleep( last_time + 1/PULSE_PER_SECOND - now ).
	 * Careful here of signed versus unsigned arithmetic.
	 */
#if !defined( WIN32 )
	{
	    struct timeval now_time;
	    long secDelta;
	    long usecDelta;

	    gettimeofday( &now_time, NULL );
	    usecDelta   = ( (int) last_time.tv_usec )
			- ( (int)  now_time.tv_usec )
			+ 1000000 / PULSE_PER_SECOND;
	    secDelta    = ( (int) last_time.tv_sec )
			- ( (int)  now_time.tv_sec );
	    while ( usecDelta < 0 )
	    {
		usecDelta += 1000000;
		secDelta  -= 1;
	    }

	    while ( usecDelta >= 1000000 )
	    {
		usecDelta -= 1000000;
		secDelta  += 1;
	    }

	    /* Lam 29.12.2002: czy merc_down? */
	    if ( !merc_down
	      && ( secDelta > 0 || ( secDelta == 0 && usecDelta > 0 ) ) )
	    {
#if defined( NANOSLEEP )
		struct timespec stall_time;
		struct timespec remain_time;

		/* o dziwo, to sie zdarza dosc czesto, kiedy serwer poprawia
		   sobie czas synchronizujac sie z innym, zamiast miec
		   sprawny zegar albo uzywac ntpd */
		if ( secDelta )
		    cbug( "nanosleep: secDelta = %d, olewam!", secDelta );
		stall_time.tv_sec = 0;
		stall_time.tv_nsec = usecDelta * 1000;

		while ( stall_time.tv_nsec && !merc_down )
		{
		    if ( nanosleep( &stall_time, &remain_time ) )
		    {
			lac_perror( "game_loop_unix: nanosleep" );
			stall_time.tv_nsec = remain_time.tv_nsec;
		    }
		    else
			stall_time.tv_nsec = 0;
		}
#else /* nie nanosleep, wiec stary select( ) */
		struct timeval stall_time;

		stall_time.tv_usec = usecDelta;
		stall_time.tv_sec  = secDelta;
		if ( select( 0, NULL, NULL, NULL, &stall_time ) < 0 )
		{
		    gettimeofday( &now_time, NULL );
		    usecDelta   = ( (int) last_time.tv_usec )
				- ( (int)  now_time.tv_usec )
				+ 1000000 / PULSE_PER_SECOND;
		    secDelta    = ( (int) last_time.tv_sec )
				- ( (int)  now_time.tv_sec );
		    while ( usecDelta < 0 )
		    {
			usecDelta += 1000000;
			secDelta  -= 1;
		    }

		    while ( usecDelta >= 1000000 )
		    {
			usecDelta -= 1000000;
			secDelta  += 1;
		    }

		    if ( secDelta > 0 || ( secDelta == 0 && usecDelta > 0 ) )
		    {
			struct timeval stall_time;

			stall_time.tv_usec = usecDelta;
			stall_time.tv_sec  = secDelta;
			if ( select( 0, NULL, NULL, NULL, &stall_time ) < 0 )
			{
			    lac_perror( "Game_loop: select: stall" );
			    send_to_all_char( "Co`s ubi`lo Laca. `Zegnajcie.\n\r" );
			    merc_down = TRUE;
			    end_of_game( );
			}
		    }
		}
#endif /* nanosleep */
	    }
	}
#else /* WIN32 */
	{
	    int times_up;
	    int nappy_time;
	    struct timeb start_time;
	    struct timeb end_time;
	    ftime( &start_time );
	    times_up = 0;

	    while ( times_up == 0 )
	    {
		ftime( &end_time );
		if ( ( nappy_time =
		      (int) ( 1000 *
			     (double) ( ( end_time.time - start_time.time ) +
				       ( (double) ( end_time.millitm -
						   start_time.millitm ) /
					1000.0 ) ) ) ) >=
		    (double)( 1000 / PULSE_PER_SECOND ) )
		  times_up = 1;
		else
		{
		    Sleep( (int) ( (double) ( 1000 / PULSE_PER_SECOND ) -
				  (double) nappy_time ) );
		    times_up = 1;
		}
	    }
	}
#endif

	gettimeofday( &last_time, NULL );
	current_time = (time_t) last_time.tv_sec;
    }

    return;
}


bool check_ban( char *host, char *ip, char *user, int type )
{
    BAN_DATA *pban;

    for ( pban = ban_list; pban; pban = pban->next )
	/* ip - str_prefix( ), mozna podawac cale klasy */
	if ( ( ( is_ipaddr( pban->host )
	      && !str_prefix( pban->host, ip ) )
	/* serwer - str_suffix( ), zeby lapalo cale domeny */
	    || ( !is_ipaddr( pban->host )
	      && !str_suffix( pban->host, host ) ) )
	/* wszyscy na serwerze, albo akurat ten */
	  && ( !*pban->user || !str_cmp( pban->user, user ) )
	/* to raczej zrozumiale */
	  && pban->type <= type )
	    return TRUE;

    return FALSE;
}


/*
 * Napisane przez Lama 27.6.98.
 * Jest to przepisany pomysl Sektora, dlatego prawa do tej technologii naleza
 * do niego, a ja sobie tylko napisalem te funkcje tak jak w Lacu trzeba. Ten
 * wlasnie watek powinien obslugiwac wchodzenie gracza wlaczajac load_char_obj
 * (lagujaca funkcja), ale napisze to kiedy indziej, bo tam sie juz trzeba bawic
 * select()em i buforami, ze wzgledu na blamaze niektorych telnetow (zwlaszcza
 * tego z Windows).
 * dopisek 1.4.2000: a gdyby wylaczyc tryb non-blocking?
 *
 * Lam 7.3.99: jesli mamy dwa lacza, laczymy sie (RFC1413) tym, ktore powinno
 * sie laczyc. Podpatrzone w kodzie tcp_wrappers.
 */
#if !defined( NO_PTHREADS )
#define CZY_BLAD_WELCOME( warunek )	if ( warunek ) \
{ \
    d->outtop = 0; \
    d->welcome_state = WELCOME_BAN; /* calkiem elegancko */ \
    memset( &d->thread, 0, sizeof( d->thread ) ); \
    return NULL; \
}

void *welcome( void *vo )
{
    DESCRIPTOR_DATA *d = (DESCRIPTOR_DATA *) vo;
#if defined( IPV6 )
    struct sockaddr_in6 sock;
    struct sockaddr_in6 sock2;
    struct sockaddr_in *sock4;
    char bufhosta6[ MSL ];
    int blad;
#else
    struct sockaddr_in sock;
    struct sockaddr_in sock2;
    struct hostent *from;
#endif
/*  struct servent *serv; */
    int plik;
    int len;
    socklen_t len2;
    char buf[ MSL ];
    char bufhosta[ MSL ];
    char name[ MIL ];
    char lsc_user[ MIL ];
    char lsc_host[ MIL ];
    char fBuf[ MSL ];
    fd_set pokij;
    int zwrot;
    struct timeval tv;

    len = write_to_descriptor( d->descriptor, "Polaczyles sie z Lacem.\n\r", 25 );
    CZY_BLAD_WELCOME( len < 0 );
    len = write_to_descriptor( d->descriptor, "Czekaj, az wszystkiego sie o tobie dowiem.\n\r", 44 );
    CZY_BLAD_WELCOME( len < 0 );

    lsc_user[ 0 ] = '\0';
    lsc_host[ 0 ] = '\0';
    strcpy( bufhosta, d->host );
    len2 = sizeof( sock );
    if ( getpeername( d->descriptor, (struct sockaddr *) &sock, &len2 )
      || getsockname( d->descriptor, (struct sockaddr *) &sock2, &len2 ) )
    {
	sprintf( bufhosta, "Welcome (desc %d): getpeername/getsockname", d->descriptor );
	lac_perror( bufhosta );
	CZY_BLAD_WELCOME( TRUE );
    }
    else
    {
#if defined( IPV6 )
	if ( !( blad = getnameinfo( (struct sockaddr *) &sock, len,
			bufhosta6, sizeof( bufhosta6 ), NULL, 0, NI_NAMEREQD ) ) )
	    strcpy( d->host, bufhosta6 );
	else
	if ( blad != EAI_FAIL
	  && blad != EAI_NONAME
	  && blad != EAI_AGAIN )
	    fprintf( stderr, "welcome: getnameinfo: (%d) %s\n", blad, gai_strerror( blad ) );
#else
	from = gethostbyaddr( (char *) &sock.sin_addr,
			sizeof( sock.sin_addr ), SOCK_AF );

	if ( from )
	    strcpy( d->host, from->h_name );
#endif

	/* sztuczka - na to telnet musi wyslac odpowiedz, wiec nie zablokuje
	   selecta ponizej czekajacego na LSC. Sama odpowiedz na typ terminala
	   przyjdzie pozniej, kiedy sie po nia zglosimy */
	len = write_to_descriptor( d->descriptor, term_type_str, 3 );
	CZY_BLAD_WELCOME( len < 0 );

	/* tu szukam LSC. Trzeba to robic przed wyswietleniem winietki, zeby moc
	   wyrzucic kazdego zabanowanego gracza */
	/* poczekamy przy uzyciu selecta na odpowiedz na powyzsze, liczac na to,
	   ze wczesniej pojawi sie LSC */
	FD_ZERO( &pokij );
	FD_SET( d->descriptor, &pokij );
	tv.tv_sec = 10;
	tv.tv_usec = 0;
	zwrot = select( d->descriptor + 1, &pokij, NULL, NULL, &tv );
	if ( zwrot )
	{
	    len = read( d->descriptor, fBuf, ( sizeof fBuf ) - 1 );
	    CZY_BLAD_WELCOME( len < 0 );
	    fBuf[ len ] = '\0';
	    /*
	     * Moze rlacd lub WhiteLantern wyslal nam LSC? (jesli tak, na
	     * pewno jest na samym poczatku, bo one najpierw sla LSC, a potem
	     * dopiero pozwalaja telnetowi mowic).
	     *
	     * LSC czyli Lac Secret Code. Format jest taki:
	     * <INIT>user<DELIM>host<LF>
	     * gdzie INIT to specjalny sekretny kod, DELIM to rownie tajny
	     * znaczek, a LF to \n.
	     *
	     * Tym sposobem dowiadujemy sie, kto probuje laczyc sie z mudem,
	     * a takze, ze rozmawiamy z tunelem.
	     */
	    if ( lsc_init_str_len
	      && strlen( fBuf ) > lsc_init_str_len + 1
	      && !strncmp( fBuf, lsc_init_str, lsc_init_str_len )
	      && strchr( fBuf + lsc_init_str_len, lsc_delimiter )
	      && strchr( fBuf, '\n' ) )
	    {
		int sz, sz2;
		for ( sz = lsc_init_str_len; fBuf[ sz ] && fBuf[ sz ] != lsc_delimiter; sz++ )
		    ;
		if ( fBuf[ sz ] == lsc_delimiter )
		{
		    fBuf[ sz ] = '\0';
		    strcpy( lsc_user, &fBuf[ lsc_init_str_len ] );
		    sz++;
		    for ( sz2 = sz; fBuf[ sz2 ] && fBuf[ sz2 ] != '\n'; sz2++ )
			;
		    if ( fBuf[ sz2 ] == '\n' )
		    {
			fBuf[ sz2 ] = '\0';
			strcpy( lsc_host, &fBuf[ sz ] );
		    }
		    sz--;
		    fBuf[ sz ] = lsc_delimiter;
		    fBuf[ sz2 ] = '\n';
		}
	    } /* koniec obslugi LSC */
	}

	/* zgodnie z RFC 856 klient powinien zaczynac w trybie ansi, co nie
	   pozwala na polskie literki na zgodnych ze standardem telnetach */
	len = write_to_descriptor( d->descriptor, binary_in, 3 );
	CZY_BLAD_WELCOME( len < 0 );
	/* To dziwne - takie ustawienie oglupia echo lokalne telneta
	   (nie daje CR oprocz LF przy wciskaniu entera), ale jego brak wcale
	   nie powoduje znikniecia polskich literek
	len = write_to_descriptor( d->descriptor, binary_out, 3 );
	CZY_BLAD_WELCOME( len < 0 ); */

	/* zadanie podania nazwy terminala */
	len = write_to_descriptor( d->descriptor, get_ttype_str, 6 );
	CZY_BLAD_WELCOME( len < 0 );
	/* ... i zeby informowal o zmianach okna */
	len = write_to_descriptor( d->descriptor, naws_str, 3 );
	CZY_BLAD_WELCOME( len < 0 );

	/*
	 * IDENT. Nie ignoruje identa pomimo LSC, bo moze sie zdarzyc, ze ktos
	 * bedzie nas chcial oszukac. Wciaz nie ma mozliwosci banowania tuneli
	 * rlacd.
	 */
	sprintf( buf, "%u,%u\r\n", (unsigned) d->port, LAC_port );
#if defined( IPV6 )
	sock.sin6_port = htons( 113 );
	sock.sin6_family = SOCK_AF;
	sock2.sin6_port = htons( 0 ); /* dowolny port */
#else
	sock.sin_port = htons( 113 );
	sock.sin_family = SOCK_AF;
	sock2.sin_port = htons( 0 ); /* dowolny port */
#endif

	plik = socket( SOCK_AF, SOCK_STREAM, 0 );
	if ( IS_INVALID_SOCKET( plik ) )
	{
	    lac_perror( "welcome: ident: socket" );
	}
	else if ( fcntl( plik, F_SETFL, O_NONBLOCK ) == -1 )
	{
	    lac_perror( "welcome: ident: fcntl" );
	    closesocket( plik );
	    plik = -1;
	}

	if ( plik >= 0 )
	{
	    if ( bind( plik, (struct sockaddr *) &sock2, sizeof( sock ) ) == -1 )
	    {
		lac_perror( "welcome: ident: bind" );
		closesocket( plik );
		plik = -1;
	    }

	    if ( plik >= 0
	      && connect( plik, (struct sockaddr *) &sock, sizeof( sock ) ) == -1
	      && errno != EINPROGRESS )
	    {
		if ( errno == ECONNREFUSED )
		    strcpy( d->user, "(brak identd)" );
		else
		    lac_perror( "welcome: ident: connect" );
		closesocket( plik );
		plik = -1;
	    }

	    if ( plik >= 0 )
	    {
		fd_set ident_pokij;
		int ident_zwrot;
		struct timeval ident_tv;
		int so_error;
		socklen_t so_err_dlug = sizeof( so_error );

		FD_ZERO( &ident_pokij );
		FD_SET( plik, &ident_pokij );
		/* Lam 21.2.2006: 5 z 15, malo ludzi uzywa, duzo ma glupie zapory */
		ident_tv.tv_sec = 5;
		ident_tv.tv_usec = 0;
		ident_zwrot = select( plik + 1, NULL, &ident_pokij, NULL, &ident_tv );
		if ( getsockopt( plik, SOL_SOCKET, SO_ERROR, &so_error, &so_err_dlug ) == -1 )
		{
		    lac_perror( "welcome: ident: getsockopt" );
		    ident_zwrot = 0;
		}
		if ( so_error )
		{
		    closesocket( plik );
		    plik = -1;
		    ident_zwrot = 0;
		    if ( so_error == ECONNREFUSED )
			strcpy( d->user, "(brak identd)" );
		    else
			strcpy( d->user, "(b`l`ad identd)" );
		}
		if ( ident_zwrot )
		{
		    ident_zwrot = write_to_descriptor( plik, buf, strlen( buf ) );
		    /* FIXME: mozliwosc niepelnego zapisu */

		    FD_ZERO( &ident_pokij );
		    FD_SET( plik, &ident_pokij );
		    ident_tv.tv_sec = 10;
		    ident_tv.tv_usec = 0;
		    ident_zwrot = select( plik + 1, &ident_pokij, NULL, NULL, &ident_tv );
		    if ( ident_zwrot ) /* w ciagu 10 sekund nadeszly jakies dane */
		    {
			/* len uzywam drugi raz w funkcji */
			len = read( plik, buf, ( sizeof buf ) - 1 );
			buf[ len ] = '\0';
			/* Lam 24.3.2000: (blad) */
			strcpy( name, "(b`l`ad odpowiedzi identd)" );
			if ( strstr( buf, "ERROR" ) )
			    ident_zwrot = sscanf( buf, "%*d , %*d : ERROR : %15s", name );
			else
			    ident_zwrot = sscanf( buf, "%*d , %*d : USERID : %*[^:]: %15s", name );
			name[ MIL - 1 ] = '\0'; /* a nuz przepelnienie? */
			if ( name[ 0 ]
			  && strcmp( name, "(b`l`ad odpowiedzi identd)" ) )
			{
			    char *bzdet = name;
			    while ( *bzdet != '\0' )
			    {
				if ( !isalnum( (unsigned char) *bzdet ) )
				    *bzdet = '_';
				bzdet++;
			    }
			    /* powyzsze zamiast smash_tilde( name ); */
			    strcpy( d->user, name );
			}
			else if ( name[ 0 ] )
			    strcpy( d->user, name );
			/* chcialem sprawdzic co to, okazalo sie, ze jakis
			   niskiej jakosci demon identd podaje
				" : USERID : UNIX : wlzzoousk"
			   (bez portow, "uzytkownik" losowany)
			if ( !ident_zwrot )
			    wiznet( "[BZDET] B`l`edna odpowied`x identd: \"$t\"{x",
				NULL, (OBJ_DATA *) buf, NULL, WIZ_DEBUG, 0, 106 ); */
		    }
		    else
			strcpy( d->user, "(identd laguje?)" );
		}
		else if ( plik != -1 )
		    strcpy( d->user, "(identd za zapor`a?)" );
	    }

	    if ( plik != -1 )
		closesocket( plik );
	}
    }

    sprintf( log_buf, "Polaczenie %d: %s@%s (%s)", d->descriptor,
		d->user, d->host, bufhosta );

    if ( lsc_user[ 0 ] || lsc_host[ 0 ] )
    {
	strcpy( d->lsc_user, d->user );
	strcpy( d->user, lsc_user );

	strcpy( d->lsc_host, d->host );
	strcpy( d->host, lsc_host );

	sprintf( log_buf, "Polaczenie %d: %s@%s [%s@%s] (%s)", d->descriptor,
		 d->user, d->host, d->lsc_user, d->lsc_host, bufhosta );

	/* W chamski sposob uzywam istniejacych zmiennych. --Lam */
#if defined( IPV6 )
	sock.sin6_family = strchr( lsc_host, ':' ) ? AF_INET6 : AF_INET;
	sock4 = (struct sockaddr_in *) &sock;
	if ( inet_pton( sock.sin6_family, lsc_host,
		sock.sin6_family == AF_INET6
			? (void *) &sock.sin6_addr
			: (void *) &sock4->sin_addr ) > 0 )
	{
	    if ( !( blad = getnameinfo( (struct sockaddr *) &sock, len,
			    bufhosta6, sizeof( bufhosta6 ), NULL, 0, NI_NAMEREQD ) ) )
	    {
		strcpy( d->host, bufhosta6 );
		sprintf( log_buf, "Polaczenie %d: %s@%s [%s@%s] (%s)",
			 d->descriptor, d->user, d->host, d->lsc_user,
			 d->lsc_host, lsc_host );
	    }
	    else
	    if ( blad != EAI_FAIL
	      && blad != EAI_NONAME
	      && blad != EAI_AGAIN )
		fprintf( stderr, "welcome: getnameinfo (lsc_host): (%d) %s\n", blad, gai_strerror( blad ) );
	}
#else
	if ( inet_aton( lsc_host, &sock.sin_addr ) )
	{
	    from = gethostbyaddr( (char *) &sock.sin_addr,
			sizeof( sock.sin_addr ), SOCK_AF );

	    if ( from )
	    {
		strcpy( d->host, from->h_name );
		sprintf( log_buf, "Polaczenie %d: %s@%s [%s@%s] (%s)",
			 d->descriptor, d->user, d->host, d->lsc_user,
			 d->lsc_host, lsc_host );
	    }
	}
#endif
    }
    log_string( log_buf );
    smash_tilde( d->host ); /* do pliku gracza nie moze trafic zadna dodatkowa tylda */
    sprintf( d->login, "%s\rZ: %s@%s", (char *) asctime_pl( localtime( &current_time ) ), d->user, d->host );

    if ( check_ban( d->host, d->ipaddr, d->user, 0 ) )
    {
	write_to_descriptor( d->descriptor,
		"\n\rNiech zemsta, niech przeklenstwo spadnie na twoja glowe!"
		"\n\rNie masz wstepu do Laca.\n\r\n\r", 0 );
	d->outtop = 0; /* nie pokaze powitania */
	d->welcome_state = WELCOME_BAN;
	d->thread = 0;
	return NULL;
    }

    d->welcome_state = WELCOME_DONE;
    d->thread = 0;

    return NULL;
}
#endif		/* NO_PTHREADS */


void new_descriptor( int control )
{
    static DESCRIPTOR_DATA  d_zero;
	   DESCRIPTOR_DATA *dnew;
#if !defined( NO_PTHREADS )
    pthread_attr_t	    attr;
#endif
#if defined( IPV6 )
    struct sockaddr_in6     sock;
    struct sockaddr_in6     sock2;
    unsigned int            len;
#else
    struct sockaddr_in      sock;
    int addr;
#endif
    char                    buf[ MAX_STRING_LENGTH ];
    int                     desc;
#if defined( WIN32 )
    signed int              size;
    u_long                  iMode = 1;
#elif defined( _POSIX_C_SOURCE )
    socklen_t               size;
#else
    unsigned int            size;
#endif
    int                     wiersz;

    size = sizeof( sock );

    desc = accept( control, (struct sockaddr *) &sock, &size );
    if ( IS_INVALID_SOCKET( desc ) )
    {
	lac_perror( "New_descriptor: accept" );
	return;
    }

#if defined( WIN32 )
    if ( ioctlsocket( desc, FIONBIO, &iMode ) != 0 )
    {
	lac_perror( "New_descriptor: ioctlsocket: FIONBIO=1" );
	return;
    }
#elif !defined( AmigaTCP ) && !defined( WIN32 )
    if ( fcntl( desc, F_SETFL, O_NONBLOCK ) == -1 )
    {
	lac_perror( "New_descriptor: fcntl: O_NONBLOCK" );
	return;
    }
#endif

    if ( !descriptor_free )
    {
	dnew            = alloc_perm( sizeof( *dnew ) );
    }
    else
    {
	dnew            = descriptor_free;
	descriptor_free = descriptor_free->next;
    }

    *dnew               = d_zero;
    dnew->descriptor    = desc;
    dnew->character     = NULL;
    dnew->connected     = CON_GET_NAME;
    dnew->showstr_head  = str_dup( "" );
    dnew->showstr_point = 0;
    dnew->outsize       = 2000;
    dnew->outbuf        = alloc_mem( dnew->outsize );
    dnew->wej_znaki	= 0;
    dnew->wej_linie	= 0;
    dnew->wyj_znaki	= 0;
    dnew->wyj_linie	= 0;
    dnew->telnet_state	= TELNET_NORM;
    dnew->nowe_dane	= FALSE;
    strcpy( dnew->user, "(brak)" );
    strcpy( dnew->term_type, "(brak)" );
    strcpy( dnew->login, "" );
    strcpy( dnew->lsc_host, "" );
    strcpy( dnew->lsc_user, "" );
    strcpy( dnew->host, "(nieznany)" );

    /* Vigud 15.9.2010: kolor uzywany w kanalach komunikacji, przy wlaczonej
       opcji kolorowania imion u odbiorcy, ustalany na podstawie reszty
       z dzielenia deskryptora przez ilosc akceptowalnych kolorow */

    /* do kolorowania imienia nie nadaja sie 4 kolory z tabeli: pierwsze dwa,
       "b" (gdzies w srodku) i ostatni (o indeksie 16). Dlatego pule kolorow
       zmniejszam o 4, wylosowac sie moga indeksy od 0 do 12. +2 daje 2 do 14,
       czyli nigdy nie wylosuje sie nr 15, ostatni dobry. I to na niego
       zamieniam "b", czyli to tak, jakby sie losowal nr 15, a nie "b". */
    wiersz = desc % ( MAX_COLOUR - 4 ) + 2;
    if ( *tablica_kolorow[ 0 ][ wiersz ] == 'b' )
	dnew->kolor = *tablica_kolorow[ 0 ][ MAX_COLOUR - 2 ];
    else
	dnew->kolor = *tablica_kolorow[ 0 ][ wiersz ];

    size = sizeof( sock );

#if defined( IPV6 )
    len = sizeof( sock2 );
    if ( getpeername( desc, (struct sockaddr *) &sock2, &len ) )
	lac_perror( "new_descriptor: getpeername" );
    else
    {
/*
	if ( getnameinfo( (struct sockaddr *) &sock2, len,
			buf, sizeof( buf ), NULL, 0, NI_NUMERICHOST ) )
		lac_perror( "new_descriptor: getnameinfo" );
*/
	if ( !inet_ntop( sock2.sin6_family, &sock2.sin6_addr, buf, sizeof( buf ) ) )
	    lac_perror( "new_descriptor: inet_ntop" );
	else
	    strcpy( dnew->host, buf );
	/* z adresami IPv4 trzeba sobie radzic inaczej */
	if ( !strncmp( dnew->host, "::ffff:", 7 ) )
	    strcpy( dnew->host, buf + 7 );
    }

    dnew->port = ntohs( sock.sin6_port );
#else
    addr = ntohl( sock.sin_addr.s_addr );
    sprintf( buf, "%d.%d.%d.%d",
	( addr >> 24 ) & 0xFF, ( addr >> 16 ) & 0xFF,
	( addr >>  8 ) & 0xFF, ( addr       ) & 0xFF );
    strcpy( dnew->host, buf );

    dnew->port = ntohs( sock.sin_port );
#endif

    strcpy( dnew->ipaddr, dnew->host );

    dnew->next                  = descriptor_list;
    descriptor_list             = dnew;

    /*
     * Powitanie. Nie musze tego robic w welcome( ), albowiem to i tak
     * bedzie czekac w buforze dopoki welcome( ) sie nie skonczy.
     */
    if ( help_greeting[ 0 ] == '.' )
	write_to_buffer( dnew, help_greeting + 1, 0 );
    else
	write_to_buffer( dnew, help_greeting, 0 );

#if defined( NO_PTHREADS ) /* Nie wlaczac jesli nie jest to konieczne! */
    dnew->welcome_state = WELCOME_DONE;
    sprintf( log_buf, "Polaczenie %d: %s@%s", dnew->descriptor, dnew->user, dnew->host );
    log_string( log_buf );
#else
    dnew->welcome_state = WELCOME_PENDING;
    pthread_attr_init( &attr );
    pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
    /* glibc 2.4 ma 10 MiB domyslnie - nie potrzebujemy tyle
       welcome( ) potrzebuje z 5 * MSL, plus funkcje, ktore wywoluje */
    pthread_attr_setstacksize( &attr, 131072 );
    pthread_create( &dnew->thread, &attr, welcome, (void *) dnew );
    pthread_attr_destroy( &attr );
#endif

    return;
}


void new_who_descriptor( int control )
{
    static WHO_DESCRIPTOR_DATA  d_zero;
	   WHO_DESCRIPTOR_DATA *dnew;
#if defined( IPV6 )
    struct sockaddr_in6     sock;
    struct sockaddr_in6     sock2;
    unsigned int            len;
#else
    struct sockaddr_in      sock;
    int addr;
#endif
    char                    buf [ MAX_STRING_LENGTH ];
    int                     desc;
#if defined( WIN32 )
    signed int              size;
    u_long                  iMode = 1;
#elif defined( _POSIX_C_SOURCE )
    socklen_t               size;
#else
    unsigned int            size;
#endif

    size = sizeof( sock );

    desc = accept( control, (struct sockaddr *) &sock, &size );
    if ( IS_INVALID_SOCKET( desc ) )
    {
	lac_perror( "New_who_descriptor: accept" );
	return;
    }

    if ( desc > 200 )
    {
	write_to_descriptor( desc, "Niestety, nie moge cie teraz wpuscic.\n\r"
	 "Sprobuj za moment, kiedy wszystko wroci do normy...\n\r", 0 );
	close( desc );
	return;
    }

#if defined( WIN32 )
    if ( ioctlsocket( desc, FIONBIO, &iMode ) != 0 )
    {
	lac_perror( "New_who_descriptor: ioctlsocket: FIONBIO=1" );
	return;
    }
#elif !defined( AmigaTCP ) && !defined( WIN32 )
    if ( fcntl( desc, F_SETFL, O_NONBLOCK ) == -1 )
    {
	lac_perror( "New_who_descriptor: fcntl: O_NONBLOCK" );
	return;
    }
#endif

    if ( !who_descriptor_free )
    {
	dnew		    = alloc_perm( sizeof( *dnew ) );
    }
    else
    {
	dnew		    = who_descriptor_free;
	who_descriptor_free = who_descriptor_free->next;
    }

    *dnew               = d_zero;
    dnew->descriptor    = desc;
    dnew->outsize	= 2000;
    dnew->outbuf	= alloc_mem( dnew->outsize );

    size = sizeof( sock );

#if defined( IPV6 )
    len = sizeof( sock2 );
    if ( getpeername( desc, (struct sockaddr *) &sock2, &len ) )
	lac_perror( "new_who_descriptor: getpeername" );
    else
    {
	if ( !inet_ntop( sock2.sin6_family, &sock2.sin6_addr, buf, sizeof( buf ) ) )
	    lac_perror( "new_who_descriptor: inet_ntop" );
	else
	    strcpy( dnew->host, buf );
	/* z adresami IPv4 trzeba sobie radzic inaczej */
	if ( !strncmp( dnew->host, "::ffff:", 7 ) )
	    strcpy( dnew->host, buf + 7 );
    }
#else
    addr = ntohl( sock.sin_addr.s_addr );
    sprintf( buf, "%d.%d.%d.%d",
	( addr >> 24 ) & 0xFF, ( addr >> 16 ) & 0xFF,
	( addr >>  8 ) & 0xFF, ( addr       ) & 0xFF
	);
    strcpy( dnew->host, buf );
#endif

    dnew->next		= who_descriptor_list;
    who_descriptor_list = dnew;

    return;
}


void close_who_socket( WHO_DESCRIPTOR_DATA *dclose )
{
    if ( dclose->outtop > 0 )
	process_who_output( dclose );

    if ( dclose == who_descriptor_list )
	who_descriptor_list = who_descriptor_list->next;
    else
    {
	WHO_DESCRIPTOR_DATA *d;

	for ( d = who_descriptor_list; d && d->next != dclose; d = d->next )
	    ;
	if ( d )
	    d->next = dclose->next;
	else
	    bug( "Close_who_socket: dclose not found.", 0 );
    }

    closesocket( dclose->descriptor );

    dclose->host[ 0 ] = '\0';
    free_mem( dclose->outbuf, dclose->outsize );

    dclose->next        = who_descriptor_free;
    who_descriptor_free = dclose;

    return;
}

#endif
/* ten endif jest od ifa przed game_loop_unix() (unix|AmiTCP|WIN32) */


void close_socket( DESCRIPTOR_DATA *dclose, bool lost )
{
    CHAR_DATA *ch;

#if !defined( MSDOS ) && !defined( NO_PTHREADS )
    if ( dclose->thread )
	pthread_cancel( dclose->thread );
#endif

    if ( dclose->outtop > 0 )
	process_output( dclose, FALSE );

    if ( dclose->snoop_by )
    {
	write_to_buffer( dclose->snoop_by,
	    "Twoja ofiara pluskwienia opuscila gre.\n\r", 0 );
    }

    {
	DESCRIPTOR_DATA *d;

	for ( d = descriptor_list; d; d = d->next )
	{
	    if ( d->snoop_by == dclose )
		d->snoop_by = NULL;
	}
    }

    if ( ( ch = dclose->character ) )
    {
	if ( dclose->connected == CON_PLAYING )
	{
	    sprintf( log_buf, "Closing link %d to %s@%s. (link lost while playing)",
		dclose->descriptor, ch->name, dclose->host );
	    log_string( log_buf );
	    if ( WP( ch ) && !IS_SET( ch->act, PLR_WIZINVIS ) ) act( ZM_WZROK | ZM_WID_CHAR, "{M### $n straci`l$a po`l`aczenie.{x", ch, NULL, NULL, TO_ROOM );
	    wiznet( "{M$N{x straci`l$A po`l`aczenie.", ch, NULL, NULL, WIZ_LINKS, 0,
		( get_trust( ch ) > 108 ) ? 108 : get_trust( ch ) );
	    ch->desc = NULL;
	    if ( ch->pcdata->clan )
	    {
		ch->pcdata->clan_member->seen = current_time;
		ch->pcdata->clan_member->ch = NULL;
	    }
	}
	else
	{
	    sprintf( log_buf, "Closing link %d to %s@%s. (dclose->connected %d)",
		dclose->descriptor, ch->name, dclose->host, dclose->connected );
	    log_string( log_buf );
	    /* pytanie o haslo, postac w nicosci */
	    if ( ( dclose->connected >= CON_GET_OLD_PWD_DELETE
		&& dclose->connected <= CON_CONFIRM_NEW_PWD_PWD )
	    /* podczas restartu */
	      || ( ch->in_room
		&& !ch->level ) )
	    {
		extract_char( dclose->character, TRUE );
	    }
	    else
		free_char( dclose->character, TRUE );
	}
    }
    else if ( lost ) /* nie zapisuje tego przy wychodzeniu z gry */
    {
	sprintf( log_buf, "Closing link %d to %s. (no character)",
		dclose->descriptor, dclose->host );
	log_string( log_buf );
    }

    if ( d_next == dclose )
	d_next = d_next->next;

    if ( dclose == descriptor_list )
    {
	descriptor_list = descriptor_list->next;
    }
    else
    {
	DESCRIPTOR_DATA *d;

	for ( d = descriptor_list; d && d->next != dclose; d = d->next )
	    ;
	if ( d )
	    d->next = dclose->next;
	else
	    bug( "Close_socket: dclose not found.", 0 );
    }

    closesocket( dclose->descriptor );

    /* Lam: tu cos tego jest malo */
    dclose->host[ 0 ] = '\0';
    dclose->user[ 0 ] = '\0';
    dclose->term_type[ 0 ] = '\0';
    dclose->login[ 0 ] = '\0';

    /* RT socket leak fix */
    free_mem( dclose->outbuf, dclose->outsize );

    dclose->next        = descriptor_free;
    descriptor_free     = dclose;
#if defined( MSDOS )
    exit( 1 );
#endif

    return;
}


/*
 * Lam 17.6.2004 wraz ze zmianami w read_from_descriptor( ) i welcome( )
 */
void interp_telnet_seq( DESCRIPTOR_DATA *d, int gdzie )
{
	unsigned int kod = (unsigned char) d->inbuf[ gdzie ];

	if ( d->telnet_state == TELNET_IAC )
	{
	    /* na razie interesuja nas tylko odpowiedzi nt. dwoch opcji,
	       przysylane z IAC SB */
	    if ( kod == SB )
	    {
#if defined( DEBUG_TELNET )
		log_string( "IAC SB" );
#endif
		d->telnet_state = TELNET_SB;
	    }
	    else if ( kod == WILL || kod == WONT || kod == DO || kod == DONT )
	    {
#if defined( DEBUG_TELNET )
		sprintf( log_buf, "IAC %s...", TELCMD_OK( kod ) ? TELCMD( kod ) : "?" );
		log_string( log_buf );
#endif
		d->telnet_state = TELNET_DWDW;
	    }
	    else
	    {
#if defined( DEBUG_TELNET )
		sprintf( log_buf, "%u (%s) po IAC, NIE SB - KONIEC",
			kod, TELCMD_OK( kod ) ? TELCMD( kod ) : "?" );
		log_string( log_buf );
#endif
		d->telnet_state = TELNET_NORM;
	    }
	}
	else if ( d->telnet_state == TELNET_DWDW )
	{
#if defined( DEBUG_TELNET )
	    sprintf( log_buf, "...%s (%u)",
		TELOPT_OK( kod ) ? TELOPT( kod ) : "?", kod );
	    log_string( log_buf );
#endif
	    d->telnet_state = TELNET_NORM;
	}
	else if ( d->telnet_state == TELNET_SB )
	{
	    if ( kod == TELOPT_TTYPE )
	    {
#if defined( DEBUG_TELNET )
		log_string( "IAC SB TTYPE" );
#endif
		d->term_type[ 0 ] = '\0';
		d->telnet_state = TELNET_TTYPE0;
	    }
	    else if ( kod == TELOPT_NAWS )
	    {
#if defined( DEBUG_TELNET )
		log_string( "IAC SB NAWS" );
#endif
		d->telnet_state = TELNET_NAWS1;
	    }
	    else
	    {
#if defined( DEBUG_TELNET )
		sprintf( log_buf, "%u (%s) IAC, NIE SB - IGNORUJ DO SE",
			kod, TELCMD( d->inbuf[ gdzie ] ) );
		log_string( log_buf );
#endif
		d->telnet_state = TELNET_IGNTOSE;
	    }
	}
	else if ( d->telnet_state == TELNET_TTYPE0 )
	{
	    if ( kod != TELQUAL_IS )
	    {
#if defined( DEBUG_TELNET )
		bug( "IAC SB TTYPE _NIE_TELQUAL_IS!_ (bo %d)", kod );
#endif
		strcpy( d->term_type, "(klient ignoruje standardy)" );
		d->telnet_state = TELNET_NORM; /* niezbyt wiem, co tu zrobic */
		/* przyklad takiego klienta: Tinkeri View czyli Savitar. Nie uzywac, smiec! */
	    }
	    else
		d->telnet_state = TELNET_TTYPE;
	}
	else if ( d->telnet_state == TELNET_TTYPE )
	{
	    int pomi; /* pomocniczy iterator */

	    if ( kod == IAC )
	    {
		d->telnet_state = TELNET_IGNTOSE2;
		/* str_prefix, wiec xterm-color i inne tez dzialaja. Testowalem
		   nastepujace programy: rxvt, xterm, nxterm, MGT, pod Windows
		   Tera Term Pro (nalezy ustawic zeby zwracal 'xterm') i PuTTY */
		if ( !str_prefix( "xterm", d->term_type ) )
		    write_to_descriptor( d->descriptor, "\033]0;" LAC_VERSION "\7", strlen( LAC_VERSION ) + 5 );

		/* usuniecie znakow kontrolnych, przez ktore jakis gmud czy zmud
		   potrafil psuc tabelke z polecenia "polaczenia" */
		for ( pomi = 0; d->term_type[ pomi ]; pomi++ )
		   /* to wylapuje takze znaki > 127, bo sa traktowane jako
		      liczby ze znakiem */
		   if ( d->term_type[ pomi ] < 32 )
			d->term_type[ pomi ] = '_';
	    }
	    else if ( strlen( d->term_type ) < CONN_TERM_LEN - 1 )
	    {
		char duzy_buf[ 2 ];
		duzy_buf[ 0 ] = kod;
		duzy_buf[ 1 ] = '\0';
		strcat( d->term_type, duzy_buf );
	    }
	}
	else if ( d->telnet_state == TELNET_NAWS1 )
	{
	    d->szer = kod << 8;
	    d->telnet_state = TELNET_NAWS2;
	}
	else if ( d->telnet_state == TELNET_NAWS2 )
	{
	    d->szer += kod;
	    if ( d->szer == 65535 ) d->szer = 255;
	    d->telnet_state = TELNET_NAWS3;
	}
	else if ( d->telnet_state == TELNET_NAWS3 )
	{
	    d->wys = kod << 8;
	    d->telnet_state = TELNET_NAWS4;
	}
	else if ( d->telnet_state == TELNET_NAWS4 )
	{
	    d->wys += kod;
	    if ( d->wys == 65535 ) d->wys = 255;
	    d->telnet_state = TELNET_IGNTOSE;
	}
	else if ( d->telnet_state == TELNET_IGNTOSE
	       || d->telnet_state == TELNET_IGNTOSE2 )
	{
	    if ( kod == SE )
	    {
#if defined( DEBUG_TELNET )
		log_string( "SE - KONIEC" );
#endif
		d->telnet_state = TELNET_NORM;
	    }
	}
	else
	    bug( "d->telnet_state %d, nie znam", d->telnet_state );

	/* usuwamy znak z bufora. Nie mozna uzyc strcpy, poniewaz kody telnetu
	   uzywaja '\0' */
	memmove( d->inbuf + gdzie, d->inbuf + gdzie + 1, sizeof( d->inbuf ) - gdzie - 1 );

    return;
}


/*
 * Lam 6.11.2000: liczenie wchodzacych znakow i linii
 * Lam 17.6.2004: poprawna, normalna obsluga tego, co nam potrzeba z protokolu
 *		TELNET (zamiast w welcome( )):
 * TERMINAL TYPE - RFC 1091 (w welcome( ) od 3.9.1998)
 * NAWS (Negotiate About Window Size) - RFC 1073 (w welcome( ) od 6.12.1998)
 */
bool read_from_descriptor( DESCRIPTOR_DATA *d )
{
    unsigned int iStart;
    int nRead;
    int pzl;
    int ipp;

    /* Hold horses if pending command already. */
    if ( d->incomm[ 0 ] != '\0' )
    {
	bug( "d->incomm nie jest puste!", 0 );
	return FALSE;
    }

    /* Check for overflow. */
    iStart = strlen( d->inbuf );
    if ( iStart >= sizeof( d->inbuf ) - 10 )
    {
	sprintf( log_buf, "%s input overflow!", d->host );
	log_string( log_buf );
	write_to_descriptor( d->descriptor,
	    "\n\r*** DAJ SOBIE NA WSTRZYMANIE! ***\n\r", 0 );
	return FALSE;
    }

    /* Snarf input. */
#if defined( MSDOS ) || defined( unix ) || defined( AmigaTCP ) || defined( WIN32 )
    for ( ; ; )
    {
#if !defined( WIN32 )
	nRead = read( d->descriptor, d->inbuf + iStart,
			sizeof( d->inbuf ) - 10 - iStart );
#else
	nRead = recv( d->descriptor, d->inbuf + iStart,
			sizeof( d->inbuf ) - 10 - iStart, 0 );
#endif
	if ( nRead > 0 )
	{
	    d->wej_znaki += nRead;
	    d->inbuf[ iStart + nRead ] = '\0';
	    for ( pzl = 0; pzl < nRead; pzl++ )
	    {
		ipp = iStart + pzl;
#if !defined( MSDOS )
		if ( ( (unsigned char) d->inbuf[ ipp ] ) == IAC
		  && d->telnet_state != TELNET_TTYPE )
		{
		    if ( d->telnet_state == TELNET_NORM )
		    {
#if defined( DEBUG_TELNET )
			log_string( "IAC" );
#endif
			d->telnet_state = TELNET_IAC;
		    }
		    else if ( d->telnet_state == TELNET_IGNTOSE )
		    {
#if defined( DEBUG_TELNET )
			log_string( "IAC przed SE" );
#endif
			d->telnet_state = TELNET_IGNTOSE2;
		    }
#if defined( DEBUG_TELNET )
		    else
			log_string( "IAC w sekwencji?" );
#endif
		    /* komentarz w interp_telnet_seq */
		    memmove( d->inbuf + ipp, d->inbuf + ipp + 1, sizeof( d->inbuf ) - ipp - 1 );
		    pzl--; /* poniewaz wycieto znak, patrz kom. powyzej */
		    nRead--;
		}
		else if ( d->telnet_state != TELNET_NORM )
		{
		    interp_telnet_seq( d, ipp );
		    pzl--; /* jw. */
		    nRead--;
		}
		else
#endif /* !defined( MSDOS ) && !defined( WIN32 ) */
		if ( d->inbuf[ ipp ] == '\n' || d->inbuf[ ipp ] == '\r' )
		{
		    d->idle = 0;
		    if ( d->character )
			d->character->timer = 0;
		    d->wej_linie++;
		    /* Lam 3.11.2005: && w pustej linii przerywa dlugie aliasy */
		    if ( ( ( ipp >= 3 )
			&& ( d->inbuf[ ipp - 3 ] == '\n'
			  || d->inbuf[ ipp - 3 ] == '\r' )
			&& d->inbuf[ ipp - 2 ] == '&'
			&& d->inbuf[ ipp - 1 ] == '&' )
		      || ( ( ipp == 2 )
			&& d->inbuf[ ipp - 2 ] == '&'
			&& d->inbuf[ ipp - 1 ] == '&' ) )
		    {
			if ( d->character )
			{
			    bool fcom = d->fcommand;
			    d->fcommand = TRUE;
			    if ( ipp == 2 )
				STC( "Bufor polece`n jest pusty, nie ma czego przerywa`c.\n\r", d->character );
			    else
				STC( "Przerywasz wykonywanie polece`n z bufora.\n\r", d->character );
			    d->fcommand = fcom;
			}
			memmove( d->inbuf, d->inbuf + ipp + 1, sizeof( d->inbuf ) - ipp - 1 );
			nRead -= pzl;
			iStart = 0;
			pzl = 0;
		    }
		}
	    }

	    iStart += nRead;
	    if ( iStart > 0
	      && ( d->inbuf[ iStart - 1 ] == '\n'
		|| d->inbuf[ iStart - 1 ] == '\r' ) )
		break;
	}
	else if ( nRead == 0 )
	{
	    sprintf( log_buf, "EOF encountered on read (link %d).", d->descriptor );
	    log_string( log_buf );
	    return FALSE;
	}
#if !defined( AmigaTCP ) && !defined( WIN32 ) && !defined( MSDOS )
	else if ( errno == EWOULDBLOCK || errno == EAGAIN )
	    break;
#endif
#if defined( WIN32 )
	else if ( WSAGetLastError( ) == WSAEWOULDBLOCK || errno == EAGAIN )
	    break;
#endif
	else
	{
	    sprintf( bug_buf, "Read_from_descriptor (%d)", d->descriptor );
	    lac_perror( bug_buf );
	    return FALSE;
	}
    }
#endif

    d->inbuf[ iStart ] = '\0';

    return TRUE;
}


/*
 * Transfer one line from input buffer to input line.
 */
void read_from_buffer( DESCRIPTOR_DATA *d, bool msg )
{
    int i;
    int j;
    int k;
    bool bezkoloru;

    /*
     * Hold horses if pending command already.
     */
    if ( d->incomm[ 0 ] != '\0' )
    {
	bug( "d->incomm nie jest puste!", 0 );
	return;
    }

    /*
     * Look for at least one new line.
     */
    for ( i = 0; d->inbuf[ i ] != '\n' && d->inbuf[ i ] != '\r'; i++ )
    {
	/* na wypadek bledow w read_from_descriptor albo rozwijaniu aliasow */
	if ( i == BUF_LIMIT_INBUF )
	{
	    /* dbug? */
	    bug( "Read_from_buffer: pelny bufor bez \\0.", 0 );
	    d->inbuf[ BUF_LIMIT_INBUF - 2 ] = '\n';
	    d->inbuf[ BUF_LIMIT_INBUF - 1 ] = '\0';
	    break;
	}
	/* jesli read_from_descriptor wczytala tylko fragment linii */
	if ( d->inbuf[ i ] == '\0' )
	    return;
    }

    if ( d->character
      && !IS_NPC( d->character )
      && IS_SET( d->character->act, PLR_NO_COLOUR ) )
	bezkoloru = TRUE;
    else
	bezkoloru = FALSE;

    /*
     * Canonical input processing.
     */
    for ( i = 0, k = 0; d->inbuf[ i ] != '\n' && d->inbuf[ i ] != '\r'; i++ )
    {
	if ( k >= MAX_INPUT_LENGTH - 2 )
	{
	    if ( msg )
	    {
		/* Lam 13.1.2003: wprowadzam polskie litery za pomoca STC,
		   a prawie na pewno w tym miejscu d->fcommand bedzie falszem
		   (czego nie rozumiem, ale tak jest) i to dodaje niepotrzebne
		   entery. Nie widze powodu, by tak robilo, poniewaz ta funkcja
		   musi wczytywac cos z klawiatury...  */
		bool tymczas = d->fcommand;
		d->fcommand = TRUE;
		if ( d->character )
		    STC( "Za d`luga linia. Wypisuj si`e kr`ocej.\n\r", d->character );
		else
		    write_to_descriptor( d->descriptor, "Za dluga linia. Wypisuj sie krocej.\n\r", 0 );
		d->fcommand = tymczas;
	    }

	    /* skip the rest of the line */
	    for ( ; d->inbuf[ i ] != '\0'; i++ )
	    {
		if ( d->inbuf[ i ] == '\n' || d->inbuf[ i ] == '\r' )
		    break;
	    }
	    if ( d->inbuf[ i ] == '\0' )
		d->inbuf[ i + 1 ] = '\0';
	    d->inbuf[ i ] = '\n';
	    /* Lam 1.5.2007: jedna za dluga linia psula caly bufor
	    d->inbuf[ i + 1 ] = '\0';
	    zamiast tego pomijam jak trzeba: */
	    do
	    {
		i++;
	    } while ( d->inbuf[ i ] != '\n'
		   && d->inbuf[ i ] != '\r'
		   && d->inbuf[ i ] != '\0' );
	    break;
	}

	if ( d->inbuf[ i ] == '\b' && k > 0 )
	    --k;
	else if ( ( d->inbuf[ i ] & b07 ) /* 7 bit (polska litera?) */
/*			&& d->connected == CON_PLAYING && d->character */
	       && d->polskie /* d->character->polskie */
	       && k < MAX_INPUT_LENGTH - 3 ) /* MIL-4, bo '`' 'X' '\n' '\0' */
	{
		int pzl;

		for ( pzl = 0; pzl < 18; pzl++ )
		    if ( d->inbuf[ i ] == polska_tablica[ d->polskie ][ pzl ][ 0 ]
		      && ( d->polskie != 4 /* specjalna obsluga 1-2bajtowego unikodu */
			|| d->inbuf[ i + 1 ] == polska_tablica[ d->polskie ][ pzl ][ 1 ] ) )
		    {
			d->incomm[ k++ ] = '`';
			/* Lac zawsze jednobajtowo */
			d->incomm[ k++ ] = *polska_tablica[ 0 ][ pzl ];
			/* jesli doszedl do tego momentu w trybie unikodu,
			   na pewno wczytal dwa bajty */
			if ( d->polskie == 4 )
			    i++;
		    }
	}
	else if ( isprint( (unsigned char) d->inbuf[ i ] ) )
	{
	    /* Lam 12.10.2004: bezkoloru */
	    if ( bezkoloru && d->inbuf[ i ] == '{' )
	    {
		i++;

		if ( d->inbuf[ i ] == '{' )
		{
		    d->incomm[ k++ ] = '{';
		    /* tak samo ucina postaciom z kolorami */
		    if ( k < MAX_INPUT_LENGTH - 2 )
			d->incomm[ k++ ] = '{';
		}

		if ( d->inbuf[ i ] == '\n' || d->inbuf[ i ] == '\r' )
		    i--;
	    }
	    else
		d->incomm[ k++ ] = d->inbuf[ i ];
	}

    }

    /*
     * Finish off the line.
     */
    if ( k == 0 )
	d->incomm[ k++ ] = ' ';
    d->incomm[ k ] = '\0';

    /*
     * Deal with bozos with #repeat 1000 ...
     *
    if ( k > 1 || d->incomm[ 0 ] == '!' )
    {
	if ( d->incomm[ 0 ] != '!' && strcmp( d->incomm, d->inlast ) )
	    d->repeat = 0;
	else if ( ++d->repeat >= 40 )
	{
	    sprintf( log_buf, "%s input spamming!", d->host );
	    log_string( log_buf );
	    if ( d->character )
		STC( "We`x co`s na wstrzymanie i przesta`n si`e wreszcie powtarza`c.\n\r", d->character );
	    else
		write_to_descriptor( d->descriptor, "\n\rWez cos na wstrzymanie i przestan sie wreszcie powtarzac.\n\r", 0 );
	    strcpy( d->incomm, "&wyjdz" );
	}
    } */

    /*
     * Do '!' substitution. Lam: !!
     */
    if ( ( d->incomm[ 0 ] == '!' ) && ( d->incomm[ 1 ] == '!' ) )
	strcpy( d->incomm, d->predlast );
    else if ( d->incomm[ 0 ] == '!' )
	strcpy( d->incomm, d->inlast );
    else
    {
	strcpy( d->predlast, d->inlast );
	strcpy( d->inlast, d->incomm );
    }

    /*
     * Shift the input buffer.
     */
    while ( d->inbuf[ i ] == '\n' || d->inbuf[ i ] == '\r' )
	i++;

    for ( j = 0; ( d->inbuf[ j ] = d->inbuf[ i + j ] ) != '\0'; j++ )
	;

    return;
}


/*
 * Low level output function.
 */
bool process_output( DESCRIPTOR_DATA *d, bool fPrompt )
{
    int a, b;
    int zwrot;

    /*
     * Bust a prompt.
     */
    if ( fPrompt
      && !merc_down
      && d->connected == CON_PLAYING
      && ( d->nowe_dane || d->fcommand ) )
    {
	if ( d->showstr_point )
	{
	/* Lam 11.10.1999: licznik linii */
		a = licz_linie( d->showstr_point );
		b = licz_linie( d->showstr_head );
		ch_printf( d->character,
  "[Jeszcze {W%d{x lini%s z%s %d]{W[(K)ontynuuj, (O)d`swie`z, (P)owr`o`c, (W)yjd`x, ENTER]:{x ",
	a, ( a == 1 ) ? "a" : ( liczba( a ) ? "e" : "i" ), /* linia/e/i */
	( b / 100 == 1 ) ? "e" : "", b ); /* z pieciu, ze stu */
/*
		write_to_buffer( d,
  "[Wpisz: (k)ontynuuj, (o)dswiez, (p)owroc, (w)yjdz, albo ENTER]:  ", 0 );
*/
/*
  "[Please type (c)ontinue, (r)efresh, (b)ack, (q)uit, or RETURN]:  ", 0 );
*/
	}
	else if ( d->character && d->character->odmiana == 0 )
	{
	    CHAR_DATA *ch;

	    ch = d->original ? d->original : d->character;
	    if ( CZY_OPCJA( ch, OPC_BLANK ) )
		write_to_buffer( d, "\n\r", 2 );

	    if ( CZY_OPCJA( ch, OPC_PROMPT ) )
		bust_a_prompt( d );

	    /* if ( CZY_OPCJA( ch, OPC_TELNET_GA ) )
		write_to_buffer( d, (char *) go_ahead_str, 0 ); */
	}
	else if ( d->character )
	    odmiana_prompt( d->character );
    }

    /*
     * Short-circuit if nothing to write.
     */
    if ( d->outtop == 0 )
	return TRUE;

    /*
     * Snoop-o-rama.
     */
    if ( d->snoop_by && d->nowe_dane )
    {
	write_to_buffer( d->snoop_by, "% ", 2 );
	write_to_buffer( d->snoop_by, d->outbuf, d->outtop );
    }

    d->nowe_dane = FALSE;

    /*
     * OS-dependent output.
     */

    zwrot = write_to_descriptor( d->descriptor, d->outbuf, d->outtop );
    if ( zwrot < 0 )
    {
	d->outtop = 0;
	return FALSE;
    }
    else if ( zwrot < BUF_MIN_PREDKOSC && d->outtop >= BUF_PROG_PRZEPELNIENIA )
    {
	d->outtop = 0;
	log_string( "process_output: zamykam zapchane polaczenie" );
	return FALSE;
    }
    else if ( zwrot == d->outtop )
    {
	d->outtop = 0;
	return TRUE;
    }
    else
    {
	memmove( d->outbuf, d->outbuf + zwrot, d->outtop - zwrot );
	d->outtop -= zwrot;
	return TRUE;
    }

    d->outtop = 0;

    return FALSE;
}


bool process_who_output( WHO_DESCRIPTOR_DATA *d )
{
    int		zwrot;

    /*
     * Jesli bufor jest pusty to albo wyslalismy wszystko, albo jeszcze
     * nie zostal zapelniony. Jesli wszystko wyslane, to zwracam FALSE,
     * by Lac zamknal gniazdo. W praktyce cts zawsze powinno byc TRUE,
     * ale na wszelki wypadek bede je sprawdzal. - Ulryk
     */
    if ( d->cts && d->outtop == 0 )
	return FALSE;
    else if ( d->outtop == 0 )
	return TRUE;

    /*
     * OS-dependent output.
     */

    zwrot = write_to_descriptor( d->descriptor, d->outbuf, d->outtop );
    if ( zwrot < 0 || ( zwrot < BUF_MIN_PREDKOSC
      && d->outtop >= BUF_PROG_PRZEPELNIENIA ) )
    {
	d->outtop = 0;
	return FALSE;
    }
    else if ( zwrot == d->outtop )
    {
	d->outtop = 0;
	return TRUE;
    }
    else
    {
	memmove( d->outbuf, d->outbuf + zwrot, d->outtop - zwrot );
	d->outtop -= zwrot;
	return TRUE;
    }

    return FALSE;
}


/*
 * Ta funkcja ma mozliwosc podswietlania wyrazow, a czasami nawet linii, jak
 * wbudowane w klienty "hilighty". W komentarzu primaaprilisowe odwracanie
 * wyrazow, ktore zostalo uzyte, ale kod szukania poczatku i konca moze sie
 * przydac do podstwietlania. Glownym problemem jest zapamietywanie biezacego
 * koloru, do ktorego nalezy wrocic po podswietleniu wyrazu.
 */
const char *odwroc_wyrazy( const char *zrodlo )
{
    return zrodlo;
/*
    static char wyjscie[ MSL * 9 ];
    char *dokad = wyjscie;
    const char *pocz, *kon;
    extern bool odwracamy;

    if ( !odwracamy )
	return zrodlo;

    pocz = zrodlo;
    while ( *pocz )
    {
	while ( *pocz && *pocz != '`' && !isalnum( *pocz ) )
	{
	    if ( *pocz == '{' )
		*dokad++ = *pocz++;
	    *dokad++ = *pocz++;
	}
	if ( !*pocz )
	    break;
	kon = pocz;
	while ( *kon && ( *kon == '`' || isalnum( *kon ) ) )
	    kon++;
	kon--;
	if ( *( kon - 1 ) == '`' )
	    kon--;
	while ( kon >= pocz )
	{
	    if ( *kon == '`' )
	    {
		*dokad++ = *kon;
		*dokad++ = *( kon + 1 );
	    }
	    else
		*dokad++ = *kon;
	    kon--;
	    if ( *( kon - 1 ) == '`' )
		kon--;
	}
	while ( *pocz && ( *pocz == '`' || isalnum( *pocz ) ) )
	    pocz++;
    }
    *dokad = '\0';

    return wyjscie;
*/
}


char *prompt_wyjscia( ROOM_INDEX_DATA *room )
{
    static char zwrot[ MIL ];
    int pzl, dir;

    zwrot[ 0 ] = '\0';
    for ( pzl = 0; pzl < MAX_DIR; pzl++ )
    {
	dir = dir_order[ pzl ];
	if ( room->exit[ dir ]
	  && room->exit[ dir ]->to_room
	  && !IS_SET( room->exit[ dir ]->exit_info, EX_BURIED ) )
	{
	    if ( zwrot[ 0 ] )
		strcat( zwrot, "-" );
	    if ( IS_SET( room->exit[ dir ]->exit_info, EX_CLOSED ) )
		strcat( zwrot, kierunki[ dir ].skrot );
	    else
		strcat( zwrot, kierunki[ dir ].skrotd );
	}
    }

    if ( zwrot[ 0 ] )
	return zwrot;
    else
	return "-";
}


/*
 * Bust a prompt (player settable prompt)
 * coded by Morgenes for Aldara Mud
 *
 * Lam: sporo zmian pochodzi ode mnie
 *
 * Lam 14.7.98: zabezpieczenie przed dzieleniem przez 0. Myslalem, ze wystarczy
 * zabezpieczyc mseta, ale sa przedmioty, ktore moga zrobic 0 mimo to.
 * (Odkryte przez Grzecha i mnie).
 */
void bust_a_prompt( DESCRIPTOR_DATA *d )
{
	 CHAR_DATA *ch;
	 CHAR_DATA *och;
   const char      *str;
   const char      *i;
	 char      *point;
	 char      *pbuff;
	 char       buffer[ MAX_STRING_LENGTH ];
	 char       buf  [ MAX_STRING_LENGTH  ];
	 char       buf2 [ MAX_STRING_LENGTH  ];
	 int        hp_proc = 0;
	 int        mana_proc = 0;
	 int        move_proc = 0;
	 int        hp_m_proc = 100;
	 int        mana_m_proc = 100;

    /* Lam: ch to mob, a och to opetujacy ch bohater, zacheta jest z och, a dane
       z ch */
    och = ( d->original ? d->original : d->character );
    ch  = d->character;

    if ( !och->pcdata->prompt || och->pcdata->prompt[ 0 ] == '\0' )
    {
	send_to_char_bw( "\n\r\n\r", ch );
	return;
    }

    /* Lam: to ma 2 zastosowania: procenty (wczesniej liczone normalnie)
       a przede wszystkim kolory zalezne od stosunku */
    if ( ch->max_hit )
	hp_proc     = 100 * ch->hit  / ch->max_hit;
    if ( ch->max_mana )
	mana_proc   = 100 * ch->mana / ch->max_mana;
    if ( ch->max_move )
	move_proc   = 100 * ch->move / ch->max_move;
    if ( ch->monitor )
    {
	if ( ch->monitor->max_hit )
	    hp_m_proc   = 100 * ch->monitor->hit  / ch->monitor->max_hit;
	if ( ch->monitor->max_mana )
	    mana_m_proc = 100 * ch->monitor->mana / ch->monitor->max_mana;
    }

    point = buf;
    str = och->pcdata->prompt;
    if ( ch->fighting && och->pcdata->fprompt && *och->pcdata->fprompt )
	str = och->pcdata->fprompt;

    /* Lam */
    if ( PP( ch ) )
    {
	if ( CZY_OPCJA( och, OPC_CZASPP ) )
	    str = "%A%I%t> ";
	else
	    str = "%A%IPoza `swiatem> ";
    }

    if ( ch->fighting )
    {
	sprintf( buf2, "{G[{Y%d%%{G]{C",
		ch->fighting->vch->max_hit == 0 ? 0
		: ( 100 * ch->fighting->vch->hit / ch->fighting->vch->max_hit ) );
	i = buf2;
	while ( ( *point = *i ) != '\0' )
	    ++point, ++i;
    }
    else
    {
	sprintf( buf2, "{C" );
	i = buf2;
	while ( ( *point = *i ) != '\0' )
	    ++point, ++i;
    }

    while ( *str != '\0' )
    {
	if ( *str != '%' )
	{
	   *point++ = *str++;
	   continue;
	}
	++str;
	switch ( *str )
	{
	    default:
		i = " ";
		break;
	    case 'f': /* 03.11.05 -Ulryk */
		if ( ch->fighting )
		    strcpy( buf2, PERS( FIGHTING( ch ), ch, TRUE ) );
		else
		    strcpy( buf2, "" );
		i = buf2;
		break;
	    case 'F': /* 03.11.05 -Ulryk */
		if ( ch->fighting )
		    strcpy( buf2, can_see( ch, FIGHTING( ch ) )
		       ? name_expand( ch, FIGHTING( ch ) ) : "kto`s" );
		else
		    strcpy( buf2, "" );
		i = buf2;
		break;
	    case 'W': /* 03.11.05 -Ulryk */
		sprintf( buf2, "%d", ch->desc
		     ? licz_linie( ch->desc->inbuf ) : 0 );
		i = buf2;
		break;
   /*Lam*/  case 't': /* 22.4.98 */
		{
		    struct tm * czas;

		    czas = localtime( &current_time );
		    sprintf( buf2, "%02d:%02d:%02d",
			      czas->tm_hour, czas->tm_min, czas->tm_sec );
		    i = buf2;
		    break;
		}
   /*Lam*/  case 'T': /* 22.4.98 */
		{
		    int min, hour;

		    min = ( pulse_point_max - pulse_point ) * 60 / pulse_point_max;
		    hour = ch->in_room ? ch->in_room->area->time_info.hour : time_info.hour;

		    sprintf( buf2, "%02d:%02d", hour, min );
		    i = buf2;
		    break;
		}
	    case 'h':
		if ( hp_proc <= 25 )
		    sprintf( buf2, "{R%d{C", ch->hit );
		else if ( hp_proc <= 50 )
		    sprintf( buf2, "{M%d{C", ch->hit );
		else
		    sprintf( buf2, "%d", ch->hit );
		i = buf2;
		break;
	    case 'H':
		sprintf( buf2, "%d", ch->max_hit );
		i = buf2;
		break;
   /*Lam*/  case 'p':
		if ( hp_proc <= 25 )
		    sprintf( buf2, "{R%d{C", hp_proc );
		else if ( hp_proc <= 50 )
		    sprintf( buf2, "{M%d{C", hp_proc );
		else
		    sprintf( buf2, "%d", hp_proc );
		i = buf2;
		break;
   /*Lam*/  case 'n':
		if ( ch->monitor )
		{
		    if ( hp_m_proc <= 25 )
			sprintf( buf2, "{R%d{C", ch->monitor->hit );
		    else if ( hp_m_proc <= 50 )
			sprintf( buf2, "{M%d{C", ch->monitor->hit );
		    else
			sprintf( buf2, "%d", ch->monitor->hit );
		}
		else
		    sprintf( buf2, "-" );
		i = buf2;
		break;
   /*Lam*/  case 'N':
		if ( ch->monitor )
		    sprintf( buf2, "%d", ch->monitor->max_hit );
		else
		    sprintf( buf2, "-" );
		i = buf2;
		break;
   /*Lam*/  case 'o':
		if ( ch->monitor )
		{
		    if ( hp_m_proc <= 25 )
			sprintf( buf2, "{R%d{C", hp_m_proc );
		    else if ( hp_m_proc <= 50 )
			sprintf( buf2, "{M%d{C", hp_m_proc );
		    else
			sprintf( buf2, "%d", hp_m_proc );
		}
		else
		    sprintf( buf2, "-" );
		i = buf2;
		break;
   /*Lam*/  case 'c':
		if ( ch->monitor )
		{
		    if ( mana_m_proc <= 25 )
			sprintf( buf2, "{R%d{C", ch->monitor->mana );
		    else if ( mana_m_proc <= 50 )
			sprintf( buf2, "{M%d{C", ch->monitor->mana );
		    else
			sprintf( buf2, "%d", ch->monitor->mana );
		}
		else
		    sprintf( buf2, "-" );
		i = buf2;
		break;
   /*Lam*/  case 'C':
		if ( ch->monitor )
		    sprintf( buf2, "%d", ch->monitor->max_mana );
		else
		    sprintf( buf2, "-" );
		i = buf2;
		break;
   /*Lam*/  case 'O':
		if ( ch->monitor )
		{
		    if ( mana_m_proc <= 25 )
			sprintf( buf2, "{R%d{C", mana_m_proc );
		    else if ( mana_m_proc <= 50 )
			sprintf( buf2, "{M%d{C", mana_m_proc );
		    else
			sprintf( buf2, "%d", mana_m_proc );
		}
		else
		    sprintf( buf2, "-" );
		i = buf2;
		break;
   /*Lam*/  case 'X':
		if ( ch->monitor )
		    sprintf( buf2, "%d", ch->monitor->exp );
		else
		    sprintf( buf2, "-" );
		i = buf2;
		break;
	    case 'm':
		if ( mana_proc <= 25 )
		    sprintf( buf2, "{R%d{C", ch->mana );
		else if ( mana_proc <= 50 )
		    sprintf( buf2, "{M%d{C", ch->mana );
		else
		    sprintf( buf2, "%d", ch->mana );
		i = buf2;
		break;
	    case 'M':
		sprintf( buf2, "%d", ch->max_mana );
		i = buf2;
		break;
   /*Lam*/  case 'P':
		if ( mana_proc <= 25 )
		    sprintf( buf2, "{R%d{C", mana_proc );
		else if ( mana_proc <= 50 )
		    sprintf( buf2, "{M%d{C", mana_proc );
		else
		    sprintf( buf2, "%d", mana_proc );
		i = buf2;
		break;
	    case 'v':
		if ( move_proc <= 25 )
		    sprintf( buf2, "{R%d{C", ch->move );
		else if ( move_proc <= 50 )
		    sprintf( buf2, "{M%d{C", ch->move );
		else
		    sprintf( buf2, "%d", ch->move );
		i = buf2;
		break;
   /*Lam*/  case 'b':
		if ( move_proc <= 25 )
		    sprintf( buf2, "{R%d{C", move_proc );
		else if ( move_proc <= 50 )
		    sprintf( buf2, "{M%d{C", move_proc );
		else
		    sprintf( buf2, "%d", move_proc );
		i = buf2;
		break;
	    case 'V':
		sprintf( buf2, "%d", ch->max_move );
		i = buf2;
		break;
	    case 'x':
		sprintf( buf2, "%d", ch->exp );
		i = buf2;
		break;
	    case 'g':
		sprintf( buf2, "%d", ch->gold );
		i = buf2;
		break;
	    case 'w':
		sprintf( buf2, "%d", ch->wait );
		i = buf2;
		break;
	    case 'a':
		if ( ch->level >= 10 )
		     sprintf( buf2, "%d", ch->alignment );
		else if ( ch->alignment >   900 ) strcpy( buf2, "{Ganielska{C" );
		else if ( ch->alignment >   700 ) strcpy( buf2, "{G`swi`eta{C" );
		else if ( ch->alignment >   350 ) strcpy( buf2, "{gdobra{C" );
		else if ( ch->alignment >   100 ) strcpy( buf2, "{gmi`la{C" );
		else if ( ch->alignment >= -100 ) strcpy( buf2, "{xneutralna{C" );
		else if ( ch->alignment >= -350 ) strcpy( buf2, "{ygro`xna{C" );
		else if ( ch->alignment >= -700 ) strcpy( buf2, "{rz`la{C" );
		else if ( ch->alignment >= -900 ) strcpy( buf2, "{Rdemoniczna{C" );
		else                              strcpy( buf2, "{Rszata`nska{C" );
   /*		if ( ch->sex == 1 )
		    sprintf( buf2, "%s", IS_GOOD( ch ) ? "dobry"
				     : IS_EVIL( ch ) ? "z`ly" : "neutralny" );
		else if ( ch->sex == 2 )
		    sprintf( buf2, "%s", IS_GOOD( ch ) ? "dobra"
				     : IS_EVIL( ch ) ? "z`la" : "neutralna" );
		else
		    sprintf( buf2, "%s", IS_GOOD( ch ) ? "dobre"
				     : IS_EVIL( ch ) ? "z`le" : "neutralne" ); */
		i = buf2;
		break;
   /*Lam*/  case 'A':
		sprintf( buf2, "%s", IS_SET( ch->act, PLR_AFK ) ? "(OOK) " : "" );
		i = buf2;
		break;
	    case 's':
		if ( !IS_NPC( ch ) && IS_AFFECTED( ch, AFF_ANIOL_STROZ ) )
		    sprintf( buf2, "%d", ch->pcdata->hp_angel );
		else
		    sprintf( buf2, "0" );
		i = buf2;
		break;
	    case 'r':
		if ( ch->in_room )
		{
		    if ( IS_AWAKE( ch ) && can_see_room( ch, ch->in_room ) )
			sprintf( buf2, "%s{C", ch->in_room->name );
		    else
			strcpy( buf2, "(nie wida`c)" );
		}
		else
		    sprintf( buf2, " " );
		i = buf2;
		break;
	    case 'R':
		if ( IS_IMMORTAL( ch ) && ch->in_room && IS_AWAKE( ch ) && can_see_room( ch, ch->in_room ) )
		    sprintf( buf2, "%d", ch->in_room->vnum );
		else
		    sprintf( buf2, " " );
		i = buf2;
		break;
	    case 'z':
		if ( ( IS_IMMORTAL( ch ) || IS_SET( ch->info, INF_KARTOGRAF ) )
		  && ch->in_room )
		{
		    if ( IS_AWAKE( ch ) && can_see_room( ch, ch->in_room ) )
			sprintf( buf2, "%s{C", ch->in_room->area->full_name );
		    else
			strcpy( buf2, "(nie wida`c)" );
		}
		else
		    sprintf( buf2, " " );
		i = buf2;
		break;
	    case 'i':  /* Invisible notification on prompt sent by Kaneda */
		sprintf( buf2, "%s", IS_AFFECTED( ch, AFF_INVISIBLE ) ? "niewidka" :
		   ( ch->sex == 1 ) ? "widzialny" : ( ch->sex == 2 ) ? "widzialna" : "widzialne");
		i = buf2;
		break;
	    case 'I':
		if ( !IS_NPC( ch ) && IS_IMMORTAL( ch )
		  && IS_SET( ch->act, PLR_WIZINVIS ) )
		    sprintf( buf2, "(B%c: %d) ",
			IS_SET( ch->act, PLR_BOSKA_KUKLA ) ? 'K' : 'N',
			ch->pcdata->wizinvis );
		else
		    strcpy( buf2, "" );
		i = buf2;
		break;
   /*Lam*/  case 'e':
		if ( ch->in_room
		  && IS_AWAKE( ch )
		  && can_see_room( ch, ch->in_room ) )
		    i = prompt_wyjscia( ch->in_room );
		else
		{
		    strcpy( buf2, "(nie wida`c)" );
		    i = buf2;
		}
		break;
	    case 'S':
		if ( !IS_NPC( ch ) && ch->pcdata->odwleczenie )
		    sprintf( buf2, "%d", ( ch->pcdata->odwleczenie + PULSE_PER_SECOND - 1 ) / PULSE_PER_SECOND );
		else
		    strcpy( buf2, "0" );
		i = buf2;
		break;
	    case '%':
	    case '\0':  /* tak lepiej wyglada -Lam */
		sprintf( buf2, "%%" );
		i = buf2;
		break;
	}

	if ( *str != '\0' )
	    ++str;

	while ( ( *point = *i ) != '\0' )
	    ++point, ++i;
    }

    sprintf( buf2, "{x" );
    i = buf2;
    while ( ( *point = *i ) != '\0' )
	++point, ++i;

/* write_to_buffer( d, buf, point - buf ); */
    *point = '\0';
    pbuff = buffer;
    /* moze zamiast ch dac och? */
    colourconv( pbuff, odwroc_wyrazy( buf ), ch, sizeof( buffer ) );
    write_to_buffer( d, buffer, 0 );

    return;
}


/*
 * Append onto an output buffer.
 */
void write_to_buffer( DESCRIPTOR_DATA *d, const char *txt, int length )
{
    int pzl;

    if ( !d->outbuf )
	return;

    /*
     * Find length in case caller didn't.
     */
    if ( length <= 0 )
	length = strlen( txt );

    /*
     * Initial \n\r if needed.
     */
    if ( !d->nowe_dane && !d->fcommand )
    {
	d->outbuf[ 0 ]  = '\n';
	d->outbuf[ 1 ]  = '\r';
	d->outtop       = 2;
    }

    /*
     * Expand the buffer as needed.
     */
    while ( d->outtop + length >= d->outsize )
    {
	char *outbuf;

	/*
	 * Wszystkie pady od jakichs 2 miesiecy byly spowodowane wysylaniem
	 * zbyt duzych ilosci tekstu (np. donate all i 900 przedmiotow)
	 * Lam 7.5.98
	 */
	if ( d->outsize >= BUF_LIMIT_ROZMIARU )
	{
	    d->outtop = 0;
	    d->outbuf[ 0 ] = '\0';
	    if ( d->character )
		send_to_char( "{R!!! Za du`zo tekstu !!!{x\n\r", d->character );
	    /* Mala rekurencja, ale sie na pewno nie zapetli */
	    return;
	}

	outbuf      = alloc_mem( 2 * d->outsize );
	strncpy( outbuf, d->outbuf, d->outtop );
	free_mem( d->outbuf, d->outsize );
	d->outbuf   = outbuf;
	d->outsize *= 2;
    }

    d->wyj_znaki += length;
    for ( pzl = 0; pzl < length; pzl++ )
	if ( txt[ pzl ] == '\n' )
	    d->wyj_linie++;

    /*
     * Copy.  Modifications sent in by Zavod.
     */
    strncpy( d->outbuf + d->outtop, txt, length );
    d->outtop += length;
    d->nowe_dane = TRUE;

    return;
}


void write_to_who_buffer( WHO_DESCRIPTOR_DATA *d, const char *txt, int length )
{
    if ( !d->outbuf )
	return;

    /*
     * Find length in case caller didn't.
     */
    if ( length <= 0 )
	length = strlen( txt );

    while ( d->outtop + length >= d->outsize )
    {
	char *outbuf;

	outbuf = alloc_mem( 2 * d->outsize );
	strncpy( outbuf, d->outbuf, d->outtop );
	free_mem( d->outbuf, d->outsize );
	d->outbuf = outbuf;
	d->outsize *= 2;
    }

    strncpy( d->outbuf + d->outtop, txt, length );
    d->outtop += length;

    return;
}


#if defined( IMUD )
void write_to_imud_buffer( const char *txt )
{
    int length = strlen( txt );

    if ( imud_down )
    {
	imud_outtop = 0;
	return;
    }

    while ( imud_outtop + length >= imud_outsize )
    {
	char *outbuf;

	outbuf      = alloc_mem( 2 * imud_outsize );
	strncpy( outbuf, imud_outbuf, imud_outtop );
	free_mem( imud_outbuf, imud_outsize );
	imud_outbuf   = outbuf;
	imud_outsize *= 2;
    }

    strncpy( imud_outbuf + imud_outtop, txt, length );
    imud_outtop += length;

    return;
}
#endif


/*
 * Lowest level output function.
 * Write a block of text to the file descriptor.
 * If this gives errors on very long blocks (like 'ofind all'),
 *   try lowering the max block size.
 */
int write_to_descriptor( int desc, const void *txt, int length )
{
    int iStart;
    int nWrite = 0;
    int nBlock;

#if defined( MSDOS )
    if ( desc == 0 )
	desc = 1;
#endif

    if ( length <= 0 )
	length = strlen( txt );

    for ( iStart = 0; iStart < length; iStart += nWrite )
    {
#if defined( MSDOS )
	nBlock = length - iStart;
#else
	nBlock = UMIN( length - iStart, 2048 );
#endif
/* rzutowanie w char *, zeby kompilator nie mial problemu z ustaleniem
   "kroku" dodawania do wskaznika */
#if !defined( WIN32 )
	nWrite = write( desc, ( (char *) txt ) + iStart, nBlock );
#else
	nWrite = send( desc, ( (char *) txt ) + iStart, nBlock, 0 );
#endif
	if ( nWrite < 0 )
	{
#if !defined( MSDOS )
# if defined( WIN32 )
	    if ( errno != EAGAIN && WSAGetLastError( ) != WSAEWOULDBLOCK )
		break;
# else
	    if ( errno != EAGAIN && errno != EWOULDBLOCK )
	    {
		sprintf( bug_buf, "Write_to_descriptor (%d)", desc );
		lac_perror( bug_buf );
	    }
# endif
#endif
	    break;
	}
	else if ( nWrite < nBlock )
	{
	    iStart += nWrite;
	    break;
	}
    }

#if !defined( MSDOS )
# if defined( WIN32 )
    return nWrite >= 0 ? iStart
	: ( errno == EAGAIN || WSAGetLastError( ) == WSAEWOULDBLOCK ) ? iStart : -1;
#else
    return nWrite >= 0 ? iStart
	: ( errno == EAGAIN || errno == EWOULDBLOCK ) ? iStart : -1;
#endif
#else
    return nWrite >= 0 ? iStart : -1;
#endif
}


/*
 * Look for link-dead player to reconnect.
 */
bool check_reconnect( DESCRIPTOR_DATA *d, bool fConn )
{
    CHAR_DATA *ch;

    for ( ch = char_list; ch; ch = ch->next )
    {
	if ( ch->deleted )
	    continue;

	if ( !IS_NPC( ch )
	    && ( !fConn || !ch->desc )
	    && !str_cmp( d->character->name, ch->name ) )
	{
	    if ( fConn == FALSE )
	    {
		free_string( d->character->pcdata->pwd );
		d->character->pcdata->pwd = str_dup( ch->pcdata->pwd );
	    }
	    else
	    {
		free_char( d->character, TRUE );
		d->character = ch;
		ch->desc     = d;
		ch->timer    = 0;
		d->polskie   = ch->polskie;
		send_to_char( "`L`acz`e ponownie.\n\r\n\r", ch );
		if ( WP( ch ) && !IS_SET( ch->act, PLR_WIZINVIS ) ) act( ZM_WZROK | ZM_WID_CHAR, "{B### $n po`l`aczy`l$9 si`e ponownie.{x", ch, NULL, NULL, TO_ROOM );
		sprintf( log_buf, "%s@%s reconnected, link %d.", ch->name, d->host, d->descriptor );
		log_string( log_buf );
		wiznet( "{M$N{x po`l`aczy`l$7 si`e ponownie.", ch, NULL, NULL, WIZ_LINKS, 0,
		    ( get_trust( ch ) > 108 ) ? 108 : get_trust( ch ) );
		if ( ch->pcdata->clan )
		    ch->pcdata->clan_member->ch = ch;
		d->connected = CON_PLAYING;
		stop_idling( ch );
		do_look( ch, "auto" ); /* Lam 26.10.2008 */
		czy_rekord_graczy( ); /* Lam 11.3.98 */
	    }
	    return TRUE;
	}
    }

    return FALSE;
}


/*
 * Check if already playing.
 */
bool check_playing( DESCRIPTOR_DATA *d, char *name, bool nowa )
{
    DESCRIPTOR_DATA *dold;

    for ( dold = descriptor_list; dold; dold = dold->next )
    {
	if ( dold != d
	    && dold->character
	    && dold->connected != CON_GET_NAME
	    && !str_cmp( name, dold->original
			? dold->original->name : dold->character->name )
	    && !dold->character->deleted )
	{
	    if ( !nowa )
	    {

		if ( d->character->sex == 2 )
		    STC( "Oka`z, `ze jeste`s t`a, za kt`or`a si`e podajesz.\n\rHas`lo: ", d->character );
		else
		    STC( "Oka`z, `ze jeste`s tym, za kt`orego si`e podajesz.\n\rHas`lo: ", d->character );
		write_to_buffer( d, (char *) echo_off_str, 0 );
		d->connected = CON_GET_OLD_PWD_GRAL;  /* 07.12.2001 Cadaver */
	    }
	    return TRUE;
	}
    }

    return FALSE;
}


void stop_idling( CHAR_DATA *ch )
{
    if (   !ch
	|| !ch->desc
	||  ch->desc->connected != CON_PLAYING
	||  ch->in_room != get_room_index( ROOM_VNUM_LIMBO )
	|| !ch->pcdata
	|| !ch->pcdata->was_in_room )
	return;

    ch->timer = 0;
    char_from_room( ch );
    char_to_room( ch, ch->pcdata->was_in_room );
    ch->pcdata->was_in_room     = NULL;
    STC( "Powracasz z nico`sci.\n\r", ch );
    act( ZM_WZROK | ZM_WID_CHAR, "$n powr`oci`l$9 z nico`sci.", ch, NULL, NULL, TO_ROOM );

    return;
}


/*
 * ch_printf - przyjazna funkcja nie wymagajaca stosowania buforow.
 * Sciagniete ze Smauga, oni nie wiem skad (EOD).
 */
void ch_printf( CHAR_DATA *ch, const char *fmt, ... )
{
    char buf[ MAX_STRING_LENGTH * 2 ];
    va_list args;

    va_start( args, fmt );
    vsprintf( buf, fmt, args );
    va_end( args );

    send_to_char( buf, ch );

    return;
}


/*
 * Write to all characters.
 */
void send_to_all_char( const char *text )
{
    DESCRIPTOR_DATA *d;

    if ( !text )
	return;

    for ( d = descriptor_list; d; d = d->next )
	if ( d->connected == CON_PLAYING )
	    send_to_char( text, d->character );

    return;
}


/*
 * Liczy ile linii ma podany tekst. Tak sprawdzam, czy powinienem przerwac
 * graczowi pagera przy send_to_char(). Wczesniej ciagle przerywal i nie
 * dalo sie przeczytac notki np. Teraz 3 linijki przerywaja pagera.
 * Lam 22.3.98
 *
 * Lam 27.5.2006: Ulryk uzyl tego do zliczania linii wejsciowych, a nie
 * wysylanych przez Laca, a PuTTY wysyla samo \r (zmud \r\n, telnet samo \n),
 * wiec zliczanie wylacznie \n nie dzialalo w przypadku PuTTY psujac %W dla
 * wiekszosci graczy.
 */
int licz_linie( const char *tekst )
{
    int licznik = 0;

    do
    {
	if ( *tekst == '\n' )
	{
	    licznik++;
	    if ( *( tekst + 1 ) == '\r' )
		tekst++;
	}
	else if ( *tekst == '\r' )
	{
	    licznik++;
	    if ( *( tekst + 1 ) == '\n' )
		tekst++;
	}
    }
    while ( *tekst++ );

    return licznik;
}


/*
 * Lam 13.1.2004: send_to_char_bw zamienia teraz polskie litery, ale nadal
 * nie zamienia kolorow
 */
void send_to_char_bw( const char *txt, CHAR_DATA *ch )
{
    const	char	*point;
		char	*point2;
		char	buf  [ MAX_STRING_LENGTH * 8 ]; /* duzo */
		char	bufek[ MAX_STRING_LENGTH * 9 ]; /* do zawijania */
		int	skip = 0;

    txt = odwroc_wyrazy( txt );

    buf[ 0 ] = '\0';
    point2 = buf;
    if ( txt && ch->desc )
    {
	if ( ch->polskie )
	{
	    for ( point = txt; *point; point++ )
	    {
		if ( *point == '`' )
		{
		    point++;
		    skip = spolszcz( *point, ch->polskie, point2 );
		    if ( skip == 0 )
			--point;
		    else
			point2 += skip;
		    continue;
		}

		*point2 = *point;
		*++point2 = '\0';
	    }
	    *point2 = '\0';
	    if ( licz_linie( buf ) > 2 && ch->desc->connected == CON_PLAYING )
	    {
		free_string( ch->desc->showstr_head );
		ch->desc->showstr_head  = str_dup( buf );
		ch->desc->showstr_point = ch->desc->showstr_head;
		show_string( ch->desc, "" );
	    }
	    else if ( CZY_OPCJA( ch, OPC_WRAP ) )
	    {
		zawin( buf, bufek, ch->desc->szer, CZY_UTF_8( ch ) );
		write_to_buffer( ch->desc, bufek, strlen( bufek ) );
	    }
	    else
		write_to_buffer( ch->desc, buf, strlen( buf ) );
	}
	else
	{
	    for ( point = txt; *point; point++ )
	    {
		if ( *point == '`' )
		{
		    if ( *++point == 'x' )
		    {
			*point2 = 'z';
			*++point2 = '\0';
			continue;
		    }
		    if ( *point == 'X' )
		    {
			*point2 = 'Z';
			*++point2 = '\0';
			continue;
		    }
		    if ( *point == '`' )
		    {
			*point2 = '`';
			*++point2 = '\0';
			continue;
		    }
		    --point;
		    continue;
		}

		*point2 = *point;
		*++point2 = '\0';
	    }
	    *point2 = '\0';
	    if ( licz_linie( buf ) > 2 && ch->desc->connected == CON_PLAYING )
	    {
		free_string( ch->desc->showstr_head );
		ch->desc->showstr_head  = str_dup( buf );
		ch->desc->showstr_point = ch->desc->showstr_head;
		show_string( ch->desc, "" );
	    }
	    else if ( CZY_OPCJA( ch, OPC_WRAP ) )
	    {
		zawin( buf, bufek, ch->desc->szer, CZY_UTF_8( ch ) );
		write_to_buffer( ch->desc, bufek, strlen( bufek ) );
	    }
	    else
		write_to_buffer( ch->desc, buf, strlen( buf ) );
	}
    }

    return;
}


/*
 * Lam: send to char, wersja z kolorami i polskimi literkami
 */
void send_to_char( const char *txt, CHAR_DATA *ch )
{
    const	char	*point;
		char	*point2;
		char	buf  [ MAX_STRING_LENGTH * 34 ]; /* duzo */
		char	bufek[ MAX_STRING_LENGTH * 36 ]; /* do zawijania */
		int	skip = 0;
		char	ostatni = '\0';

    txt = odwroc_wyrazy( txt );

    buf[ 0 ] = '\0';
    point2 = buf;
    if ( txt && ch->desc && ch->desc->outbuf )
    {
	if ( ch->kolor && ch->polskie )
	{
	    for ( point = txt; *point; point++ )
	    {
		if ( *point == '{' )
		{
		    point++;
		    if ( *point != ostatni )
		    {
			ostatni = *point;
			point2 += colour( *point, ch, point2 );
		    }
		    continue;
		}
		if ( *point == '`' )
		{
		    point++;
		    skip = spolszcz( *point, ch->polskie, point2 );
		    if ( skip == 0 )
			--point;
		    else
			point2 += skip;
		    continue;
		}

		*point2++ = *point;
		/* kolory ansi maja najwyzej 7, nie ma htmlowych u graczy */
		if ( (unsigned int) ( point2 - buf ) > sizeof( buf ) - 8 )
		    break;
	    }
	    *point2 = '\0';
	    if ( licz_linie( buf ) > 2 && ch->desc->connected == CON_PLAYING )
	    {
		free_string( ch->desc->showstr_head );
		ch->desc->showstr_head  = str_dup( buf );
		ch->desc->showstr_point = ch->desc->showstr_head;
		show_string( ch->desc, "" );
	    }
	    else if ( CZY_OPCJA( ch, OPC_WRAP ) )
	    {
		zawin( buf, bufek, ch->desc->szer, CZY_UTF_8( ch ) );
		write_to_buffer( ch->desc, bufek, strlen( bufek ) );
	    }
	    else
		write_to_buffer( ch->desc, buf, strlen( buf ) );
	}
	else if ( ch->kolor && !ch->polskie )
	{
	    for ( point = txt; *point; point++ )
	    {
		if ( *point == '{' )
		{
		    point++;
		    if ( *point != ostatni )
		    {
			ostatni = *point;
			point2 += colour( *point, ch, point2 );
		    }
		    continue;
		}
		if ( *point == '`' )
		{
		    if ( *++point == 'x' )
		    {
			*point2 = 'z';
			*++point2 = '\0';
			continue;
		    }
		    if ( *point == 'X' )
		    {
			*point2 = 'Z';
			*++point2 = '\0';
			continue;
		    }
		    if ( *point == '`' )
		    {
			*point2 = '`';
			*++point2 = '\0';
			continue;
		    }
		    --point;
		    continue;
		}

		*point2++ = *point;
		/* kolory ansi maja najwyzej 7, nie ma htmlowych u graczy */
		if ( (unsigned int) ( point2 - buf ) > sizeof( buf ) - 8 )
		    break;
	    }
	    *point2 = '\0';
	    if ( licz_linie( buf ) > 2 && ch->desc->connected == CON_PLAYING )
	    {
		free_string( ch->desc->showstr_head );
		ch->desc->showstr_head  = str_dup( buf );
		ch->desc->showstr_point = ch->desc->showstr_head;
		show_string( ch->desc, "" );
	    }
	    else if ( CZY_OPCJA( ch, OPC_WRAP ) )
	    {
		zawin( buf, bufek, ch->desc->szer, CZY_UTF_8( ch ) );
		write_to_buffer( ch->desc, bufek, strlen( bufek ) );
	    }
	    else
		write_to_buffer( ch->desc, buf, strlen( buf ) );
	}
	else if ( !ch->kolor && ch->polskie )
	{
	    for ( point = txt; *point; point++ )
	    {
		if ( *point == '{' )
		{
		    if ( point[ 1 ] == '{' )
		    {
			*point2 = '{';
			*++point2 = '\0';
		    }
		    point++;
		    continue;
		}
		if ( *point == '`' )
		{
		    point++;
		    skip = spolszcz( *point, ch->polskie, point2 );
		    if ( skip == 0 )
			--point;
		    else
			point2 += skip;
		    continue;
		}

		*point2++ = *point;
		/* brak kolorow, polskie znaki maks. 2 bajty */
		if ( (unsigned int) ( point2 - buf ) > sizeof( buf ) - 3 )
		    break;
	    }
	    *point2 = '\0';
	    if ( licz_linie( buf ) > 2 && ch->desc->connected == CON_PLAYING )
	    {
		free_string( ch->desc->showstr_head );
		ch->desc->showstr_head  = str_dup( buf );
		ch->desc->showstr_point = ch->desc->showstr_head;
		show_string( ch->desc, "" );
	    }
	    else if ( CZY_OPCJA( ch, OPC_WRAP ) )
	    {
		zawin( buf, bufek, ch->desc->szer, CZY_UTF_8( ch ) );
		write_to_buffer( ch->desc, bufek, strlen( bufek ) );
	    }
	    else
		write_to_buffer( ch->desc, buf, strlen( buf ) );
	}
	else if ( !ch->kolor && !ch->polskie )
	{
	    for ( point = txt; *point; point++ )
	    {
		if ( *point == '{' )
		{
		    if ( point[ 1 ] == '{' )
		    {
			*point2 = '{';
			*++point2 = '\0';
		    }
		    point++;
		    continue;
		}
		if ( *point == '`' )
		{
		    if ( *++point == 'x' )
		    {
			*point2 = 'z';
			*++point2 = '\0';
			continue;
		    }
		    if ( *point == 'X' )
		    {
			*point2 = 'Z';
			*++point2 = '\0';
			continue;
		    }
		    if ( *point == '`' )
		    {
			*point2 = '`';
			*++point2 = '\0';
			continue;
		    }
		    --point;
		    continue;
		}

		*point2++ = *point;
		/* brak kolorow, polskie skracane do 1 bajtu */
		if ( (unsigned int) ( point2 - buf ) > sizeof( buf ) - 2 )
		    break;
	    }
	    *point2 = '\0';
	    if ( licz_linie( buf ) > 2 && ch->desc->connected == CON_PLAYING )
	    {
		free_string( ch->desc->showstr_head );
		ch->desc->showstr_head  = str_dup( buf );
		ch->desc->showstr_point = ch->desc->showstr_head;
		show_string( ch->desc, "" );
	    }
	    else if ( CZY_OPCJA( ch, OPC_WRAP ) )
	    {
		zawin( buf, bufek, ch->desc->szer, CZY_UTF_8( ch ) );
		write_to_buffer( ch->desc, bufek, strlen( bufek ) );
	    }
	    else
		write_to_buffer( ch->desc, buf, strlen( buf ) );
	}
    }

    return;
}


/* The heart of the pager.  Thanks to N'Atas-Ha, ThePrincedom
   for porting this SillyMud code for MERC 2.0 and laying down the groundwork.
   Thanks to Blackstar, hopper.cs.uiowa.edu 4000 for which
   the improvements to the pager was modeled from.  - Kahn */
/* 12/1/94 Fixed bounds and overflow bugs in pager thanks to BoneCrusher
   of EnvyMud Staff - Kahn */

void show_string( struct descriptor_data *d, char *input )
{
    register char *scan;
    static   char  buffer[ MAX_STRING_LENGTH * 6 ];
    static   char  bufek [ MAX_STRING_LENGTH * 7 ]; /* po zawin( ) */
	     char  buf   [ MAX_INPUT_LENGTH ];
	     int   line      = 0;
	     int   toggle    = 0;
	     int   pagelines = 20;

    one_argument( input, buf );

    switch ( UPPER( buf[ 0 ] ) )
    {
    case '\0':
    case 'K': /* kontynuuj - show next page of text */
	break;

    case 'O': /* odswiez - refresh current page of text */
	toggle = 1;
	break;

    case 'P': /* powroc - scroll back a page of text */
	toggle = 2;
	break;

    default: /* otherwise, stop the text viewing */
	if ( d->showstr_head )
	{
	    free_string( d->showstr_head );
	    d->showstr_head = str_dup( "" );
	}
	d->showstr_point = 0;
	return;

    }

    if ( d->original )
	pagelines = d->original->pcdata->pagelen;
    else
	pagelines = d->character->pcdata->pagelen;

    if ( toggle )
    {
	if ( d->showstr_point == d->showstr_head )
	    return;
	if ( toggle == 1 )
	    line = -1;
	do
	{
	    if ( *d->showstr_point == '\n' )
	      if ( ( line++ ) == ( pagelines * toggle ) )
		break;
	    d->showstr_point--;
	} while ( d->showstr_point != d->showstr_head );
    }

    line    = 0;
    *buffer = 0;
    scan    = buffer;

    /* Lam 9.10.2000: wylaczone ze wzgledu na utrate fragmentow tekstu podczas
       laga. Probowalem tez ESC [ s i ESC [ u, ale zapamietana pozycja nie
       przesuwa sie wraz z przesuwaniem linii (wciskaniem enterow). */
/*
    if ( d->showstr_point != d->showstr_head )
    {
	*scan++ = ESC;
	*scan++ = '[';
	*scan++ = 'A';
	*scan++ = '\r';
	*scan++ = ESC;
	*scan++ = '[';
	*scan++ = '2';
	*scan++ = 'K';
    }
*/
    if ( *d->showstr_point )
    {
	do
	{
	    *scan = *d->showstr_point;
	    if ( *scan == '\n' )
		if ( ( ++line ) == pagelines ) /* bylo line++ */
		{
		    scan++;
		    d->showstr_point++;
		    if ( *d->showstr_point == '\r' )
		    {
			*scan = '\r';
			scan++; /* Lam 27.6.98: to bardzo wazne! */
			d->showstr_point++;
		    }
		    break;
		}
	    scan++;
	    d->showstr_point++;
	    if ( !*d->showstr_point )
		break;
	} while ( 1 );
    }

    *scan = 0;

    /* Lam: zawin */
    if ( d->original )
    {
	if ( CZY_OPCJA( d->original, OPC_WRAP ) )
	{
	    zawin( buffer, bufek, d->szer, CZY_UTF_8( d->original ) );
	    write_to_buffer( d, bufek, 0 );
	}
	else
	    write_to_buffer( d, buffer, 0 );
    }
    else if ( d->character )
    {
	if ( CZY_OPCJA( d->character, OPC_WRAP ) )
	{
	    zawin( buffer, bufek, d->szer, CZY_UTF_8( d->character ) );
	    write_to_buffer( d, bufek, 0 );
	}
	else
	    write_to_buffer( d, buffer, 0 );
    }
    else
	write_to_buffer( d, buffer, strlen( buffer ) );
    if ( *d->showstr_point == 0 )
    {
	free_string( d->showstr_head );
	d->showstr_head  = str_dup( "" );
	d->showstr_point = 0;
    }

    return;
}


/*
 * Zawija tekst. Uwzglednia znaki ANSI, zeby ich nie zawijac (nie zwieksza
 * licznika kiedy trafi na kolor). Zawija na 80 kolumnie, czyli niektore
 * emulatory terminala (np. BitCom) moga zle wyswietlac tak zawiniety tekst, ale
 * tylko jesli same zawijaja, a wtedy przeciez niepotrzebne im zawijanie.
 * Lam 6.4.98 i 15.4.98
 * Lam 23.3.99: szerokosc brana z terminala, zamiast na sztywno 80
 */
void zawin( const char *in, char *out, int szer, bool uni )
{
    bool ansi = FALSE;
    int count = 0;

    if ( szer < 4 )
	szer = 80;

    for ( ; ; )
    {
	if ( !*in ) { *out = '\0'; break; } /* koniec lancucha */

	if ( ansi ) /* jesli ANSI, to cyfry i srednik sa dopuszczalne */
	{
	    if ( ( *in >= '0' && *in <= '9' ) || *in == ';' )
		*out++ = *in++;
	    else /* litera oznacza koniec ANSI, ale ona sama tez musi byc ignorowana */
	    {
		*out++ = *in++;
		ansi = FALSE;
	    }
	    continue;
	}

	if ( *in == ESC ) /* escape - poczatek sekwencji ANSI */
	{
	    *out++ = *in++;
	    if ( *in == '[' ) /* bedzie wiecej niz jeden znak po ESC */
		ansi = TRUE;
	    if ( *in )
		*out++ = *in++;
	    continue;
	}

	/* Poczatek sekwencji UTF-8. Obsluguje wiecej niz dwuznakowe (chociaz
	   aktualnie Lac nie moze takich wygenerowac), ale nie obsluguje
	   niepoprawnych (mysle, ze nigdy nie bedzie generowal/przepuszczal) */
	if ( uni
	  && ( *in & 0xc0 ) == 0xc0 )
	{
	    *out++ = *in++;
	    while ( ( *in & 0xc0 ) == 0x80
		 && ( *( in + 1 ) & 0xc0 ) == 0x80 )
	    {
		*out++ = *in++;
	    }
	    continue;
	}

	if ( *in == '\n' || *in == '\r' )
	    count = -1; /* ponizej jest count++ i zrobi 0 */

	*out++ = *in++;

	if ( count++ == szer - 1 )
	{
	    count = 0;
	    if ( *in == '\n' )
		continue; /* dzieki temu nie zawija dwa razy */
	    *out++ = '\n';
	    *out++ = '\r';
	}
    }

    return;
}


/*
 * The primary output interface for formatted output.
 * Przyspieszona przez Lama 20.5.98. Bieganie po pomieszczeniu bylo
 * niepotrzebne, zwlaszcza jesli slane bylo tylko do jednej osoby.
 * 26.5.1998: zamiana wielu URANGE na UMIN
 * 29.4.2000: super_act, act() i colour_act() -> makra (merc.h)
 *  7.5.2000: opcja, wiadomosc wedruje tylko do postaci BEZ tej opcji.
 * 8.12.2000: usuniecie colour_act() i barwy z super_act()
 * 20.3.2002: plec "ktosia" zawsze meska, dwa ponizsze makra
 * 21.6.2003: poza swiatem bogowie widza bledy, reszta niczego nie widzi
 *    3.2006: zmysly
 */
#define CH_SEX ( ( ch && ( telepatia ? can_see_who( to, ch ) \
			: can_see( to, ch ) ) ) ? UMIN( ch->sex, 2 ) : 1 )
#define VCH_SEX ( ( vch && ( telepatia ? can_see_who( to, vch ) \
			: can_see( to, vch ) ) ) ? UMIN( vch->sex, 2 ) : 1 )

void super_act( unsigned int opcja, int zmysly, const char *format,
	CHAR_DATA *ch, const void *arg1, const void *arg2, int type, bool ooc )
{
	   OBJ_DATA        *obj1        = (OBJ_DATA  *) arg1;
	   OBJ_DATA        *obj2        = (OBJ_DATA  *) arg2;
	   CHAR_DATA       *to;
	   CHAR_DATA       *vch         = (CHAR_DATA *) arg2;
    static char *    const  on_ona  [ ] = { "ono", "on", "ona" };
    static char *    const  jemu_jej[ ] = { "temu", "jemu", "jej" };
    static char *    const  mu_jej  [ ] = { "mu", "mu", "jej" };
    static char *    const  jego_jej[ ] = { "jego", "jego", "jej" };
    static char *    const  niego_niej[]= { "niego", "niego", "niej" };
    static char *    const  nim_nia [ ] = { "nim", "nim", "ni`a" };
    static char *    const  go_ja   [ ] = { "je", "go", "j`a" };
    static char *    const  go_jej  [ ] = { "go", "go", "jej" };
    static char *    const  y_a_e   [ ] = { "e", "y", "a" }; /* zabit[y/a/e] */
    static char *    const  Y_A_E   [ ] = { "E", "Y", "A" };
    static char *    const  a_o     [ ] = { "o", "", "a" };/* uciek`l/uciek`la/uciek`lo */
    static char *    const  A_O     [ ] = { "O", "", "A" };
    static char *    const  e_a_o   [ ] = { "e", "e", "a" }; /* uciek`l[e/a/e]`s */
    const  char            *str;
    const  char            *i;
	   char            *point;
	   char            *pbuff;
	   char             buf     [ MAX_STRING_LENGTH ];
	   char             buf1    [ MAX_STRING_LENGTH ];
	   char             buffer  [ MAX_STRING_LENGTH * 2 ];
	   char             bufek   [ MAX_STRING_LENGTH * 2 ];
	   bool             telepatia = FALSE;
	   bool	            odwrocopc = FALSE;
	   char             kolor[] = "{x";

    /*
     * Discard null and zero-length messages.
     */
    if ( !format || !*format )
    {
	bug( "brak formatu", 0 );
	return;
    }

    if ( IS_SET( type, ACT_TELEP ) )
    {
	telepatia = TRUE;
	REMOVE_BIT( type, ACT_TELEP );
    }

    if ( opcja && IS_SET( opcja, OPC_ODWROC ) )
    {
	REMOVE_BIT( opcja, OPC_ODWROC );
	odwrocopc = TRUE;
    }

    if ( type == TO_CHAR )
	to = ch;
    else if ( type == TO_VICT )
    {
	if ( !vch )
	{
	    bug( "Act: null vch with TO_VICT.", 0 );
	    sprintf( buf1, "Bad act string:  %s", format );
	    bug( buf1, 0 );
	    return;
	}
	to = vch;
    }
    else
	to = ch->in_room->people;


    /* tu jest nowosc - jesli do jednej postaci, nie robi tej petli 100 razy */
    for ( ; to; to = ( type == TO_CHAR || type == TO_VICT ) ? NULL
							: to->next_in_room )
    {
	/* Vigud 23.4.2010, 2.6.2010: przemeblowanie */
	if ( ( to->deleted )
	  || ( !to->desc && IS_NPC( to ) )
	  || ( !ooc && PP( to ) && to->level < 106 )
	  || ( opcja && odwrocopc != ( CZY_OPCJA( to, opcja ) ? TRUE : FALSE ) ) /* Lam 7.5.2000 */
	  || ( type == TO_ROOM && to == ch )
	  || ( type == TO_NOTVICT && ( to == ch || to == vch ) ) )
	{
	    continue;
	}

/*************************** ZMYSLY ***************************/

	/* sprawdzenie, czy sa ustawione ktorekolwiek ze zmyslow na "plus" */
	if ( !IS_SET( zmysly, ZM_PRZYT | ZM_ZAWSZE ) )
	{
	    bug( "brak wybranych zmyslow", 0 );
	    return;
	}

	/*
	 * 1. ZM_ZAWSZE widoczne zawsze; dowolny inny zmysl wystarczy do
	 *    odebrania wiadomosci.
	 */
	if ( !IS_SET( zmysly, ZM_ZAWSZE ) )
	{
	    bool widzi = FALSE;

	    if ( IS_SET( zmysly, ZM_WZROK )
	      && IS_AWAKE( to )
	      && can_see_room( to, to->in_room ) )
		widzi = TRUE;

	    if ( !widzi
	      && IS_SET( zmysly, ZM_SLUCH )
	      && CAN_HEAR( to ) )
		widzi = TRUE;

	    if ( !widzi
	      && IS_SET( zmysly, ZM_WECH )
	      && IS_AWAKE( to )
	      && !CZY_BRAK_TLENU( to ) )
		widzi = TRUE;

	    if ( !widzi
	      && IS_SET( zmysly, ZM_DOTYK )
	      && IS_AWAKE( to ) )
		widzi = TRUE;

	    if ( !widzi )
		continue;
	}

	/*
	 * 2. ZM_WID_* musza byc widoczne wszystkie, zeby odebrac wiadomosc.
	 */
	if ( IS_SET( zmysly, ZM_WID_CHAR )
	  && !can_see( to, ch ) )
	    continue;

	if ( IS_SET( zmysly, ZM_WID_VICT )
	  && !can_see( to, vch ) )
	    continue;

	if ( IS_SET( zmysly, ZM_WID_OBJ1 )
	  && !can_see_obj( to, obj1 ) )
	    continue;

	if ( IS_SET( zmysly, ZM_WID_OBJ2 )
	  && !can_see_obj( to, obj2 ) )
	    continue;

	/*
	 * 3. Odbieranie dowolnego ze zmyslow ZM_W_* blokuje wiadomosc.
	 */
	if ( IS_SET( zmysly, ZM_W_WZROK )
	  && IS_AWAKE( to )
	  && can_see_room( to, to->in_room ) )
	    continue;

	if ( IS_SET( zmysly, ZM_W_SLUCH )
	  && CAN_HEAR( to ) )
	    continue;

	if ( IS_SET( zmysly, ZM_W_WECH )
	  && IS_AWAKE( to )
	  && !CZY_BRAK_TLENU( to ) )
	    continue;

	if ( IS_SET( zmysly, ZM_W_DOTYK )
	  && IS_AWAKE( to ) )
	    continue;

	/*
	 * 4. Bez ZM_W_WID_* widac wszystkich. Z jakimis ZM_W_WID_* wystarczy
	 *    zalapanie sie na dowolny, zeby zobaczyc komunikat.
	 */
	if ( IS_SET( zmysly, ZM_W_WID_CHAR )
	  || IS_SET( zmysly, ZM_W_WID_VICT )
	  || IS_SET( zmysly, ZM_W_WID_OBJ1 )
	  || IS_SET( zmysly, ZM_W_WID_OBJ2 ) )
	{
	    bool widzi = FALSE;

	    if ( IS_SET( zmysly, ZM_W_WID_CHAR )
	      && !can_see( to, ch ) )
		widzi = TRUE;

	    if ( !widzi
	      && IS_SET( zmysly, ZM_W_WID_VICT )
	      && !can_see( to, vch ) )
		widzi = TRUE;

	    if ( !widzi
	      && IS_SET( zmysly, ZM_W_WID_OBJ1 )
	      && !can_see_obj( to, obj1 ) )
		widzi = TRUE;

	    if ( !widzi
	      && IS_SET( zmysly, ZM_W_WID_OBJ2 )
	      && !can_see_obj( to, obj2 ) )
		widzi = TRUE;

	    if ( !widzi )
		continue;
	}

/*************************** zmysly ***************************/

	point   = buf;
	str     = format;

	if ( !ooc && PP( to ) )
	{
	    i = "[PP-KWAS?]";
	    while ( ( *point = *i ) != '\0' )
		++point, ++i;
	}

	while ( *str != '\0' )
	{
	    if ( *str != '$' )
	    {
		*point++ = *str++;
		continue;
	    }
	    ++str;

	    if ( !arg2 && *str >= 'A' && *str <= 'Z' )
	    {
		bug( "Act: missing arg2 for code %d.", *str );
		sprintf( buf1, "Bad act string:  %s", format );
		bug( buf1, 0 );
		i = " <@@@> ";
	    }
	    else
	    {
		switch ( *str )
		{
		default:  bug( "Act: bad code %d.", *str );
			  sprintf( buf1, "Bad act string:  %s", format );
			  bug( buf1, 0 );
			  i = " <@@@> ";                                break;
		/* Lam 31.12.2003: jak wstawic '$' w act? */
		case '\\': i = "$";					break;

		/* Thx alex for 't' idea */
		case 't': i = (char *) arg1;				break;
		case 'T': i = (char *) arg2;				break;
		case 'r': i = can_see( to, ch ) || can_see_obj( to, obj2 ) ?
			  (char *) arg1 : "co`s";			break;
		case 'R': i = can_see( to, ch ) || can_see_obj( to, obj1 ) ?
			  (char *) arg2 : "co`s";			break;

		/* Vigud 15.9.2010 */
		case '{':
		    {
			str++;

			if ( to->podswietl == 0
			  || !ch || !ch->desc
			  || !( telepatia ? can_see_who( to, ch ) : can_see( to, ch ) ) )
			{
			    kolor[ 1 ] = *str;
			}
			else
			    kolor[ 1 ] = ch->desc->kolor;

			i = kolor;

			break;
		    }
		case '}':
		    {
			str++;

			if ( to->podswietl == 0
			  || to->podswietl == 1
			  || !ch || !ch->desc
			  || !( telepatia ? can_see_who( to, ch ) : can_see( to, ch ) ) )
			{
			    kolor[ 1 ] = *str;
			}
			else
			    kolor[ 1 ] = ch->desc->kolor;

			i = kolor;

			break;
		    }
		case 'n': i = PERS(  ch,  to, telepatia );		break;
		case 'N': i = PERS(  vch, to, telepatia );		break;
		case '1': i = PERSD( ch,  to, telepatia );		break;
		case '!': i = PERSD( vch, to, telepatia );		break;
		case '2': i = PERSC( ch,  to, telepatia );		break;
		case '@': i = PERSC( vch, to, telepatia );		break;
		case '3': i = PERSB( ch,  to, telepatia );		break;
		case '#': i = PERSB( vch, to, telepatia );		break;
		case '4': i = PERSN( ch,  to, telepatia );		break;
		case '$': i = PERSN( vch, to, telepatia );		break;
		case '5': i = PERSM( ch,  to, telepatia );		break;
		case '%': i = PERSM( vch, to, telepatia );		break;

		/* do stosowania jako z$e/z$E przed $4/$$ */
		case 'e': i = can_see( to, ch ) && czy_ze( PERSN( ch, to, telepatia ) ) ?
			  "e" : "";					break;
		case 'E': i = can_see( to, vch ) && czy_ze( PERSN( vch, to, telepatia ) ) ?
			  "e" : "";					break;

		/* $y i $a - literki ze Zlego */
		case 'y':
		case '0': i = y_a_e	[ CH_SEX ];	break;
		case ')': i = Y_A_E	[ CH_SEX ];	break;
		case 'a':
		case '9': i = a_o	[ CH_SEX ];	break;
		case '(': i = A_O	[ CH_SEX ];	break;
		case 'Y':
		case '8': i = y_a_e	[ VCH_SEX ];	break;
		case '*': i = Y_A_E	[ VCH_SEX ];	break;
		case 'A':
		case '7': i = a_o	[ VCH_SEX ];	break;
		case '&': i = A_O	[ VCH_SEX ];	break;
		case 'o': i = e_a_o	[ CH_SEX ];	break;
		case 'O': i = e_a_o	[ VCH_SEX ];	break;

		case 'z': i = on_ona	[ CH_SEX ];	break;
		case 'Z': i = on_ona	[ VCH_SEX ];	break;
		case 'x': i = jemu_jej	[ CH_SEX ];	break;
		case 'X': i = jemu_jej	[ VCH_SEX ];	break;
		case 'u': i = mu_jej	[ CH_SEX ];	break;
		case 'U': i = mu_jej	[ VCH_SEX ];	break;
		case 'c': i = jego_jej	[ CH_SEX ];	break;
		case 'C': i = jego_jej	[ VCH_SEX ];	break;
		case 'v': i = go_ja	[ CH_SEX ];	break;
		case 'V': i = go_ja	[ VCH_SEX ];	break;
		case 'b': i = niego_niej[ CH_SEX ];	break;
		case 'B': i = niego_niej[ VCH_SEX ];	break;
		case 'm': i = go_jej	[ CH_SEX ];	break;
		case 'M': i = go_jej	[ VCH_SEX ];	break;
		case 'w': i = nim_nia	[ CH_SEX ];	break;
		case 'W': i = nim_nia	[ VCH_SEX ];	break;


		case 'p': i = can_see_obj( to, obj1 )
				? obj1->short_descr : "co`s";		break;
		case 'P': i = can_see_obj( to, obj2 )
				? obj2->short_descr : "co`s";		break;
		case 'g': i = can_see_obj( to, obj1 )
				? obj1->dopelniacz  : "czego`s";	break;
		case 'G': i = can_see_obj( to, obj2 )
				? obj2->dopelniacz  : "czego`s";	break;
		case 'h': i = can_see_obj( to, obj1 )
				? obj1->celownik    : "czemu`s";	break;
		case 'H': i = can_see_obj( to, obj2 )
				? obj2->celownik    : "czemu`s";	break;
		case 'j': i = can_see_obj( to, obj1 )
				? obj1->biernik     : "co`s";		break;
		case 'J': i = can_see_obj( to, obj2 )
				? obj2->biernik     : "co`s";		break;
		case 'k': i = can_see_obj( to, obj1 )
				? obj1->narzednik   : "czym`s";		break;
		case 'K': i = can_see_obj( to, obj2 )
				? obj2->narzednik   : "czym`s";		break;
		case 'l': i = can_see_obj( to, obj1 )
				? obj1->miejscownik : "czym`s";		break;
		case 'L': i = can_see_obj( to, obj2 )
				? obj2->miejscownik : "czym`s";		break;

		/* w oryginale bylo male 'd', ale bralo tylko pierwszy wyraz
		   ze slow kluczowych drzwi (my mamy odmiane) */
		case 'D':
		    if ( !arg2 || ( (char *) arg2 )[ 0 ] == '\0' )
			i = "drzwi";
		    else
			i = (char *) arg2;
		    break;
		}
	    }

	    ++str;
	    while ( ( *point = *i ) != '\0' )
		++point, ++i;
	}

	*point++        = '\n';
	*point++        = '\r';
	*point          = '\0';

	/* Vigud 15.9.2010: wczesniej tu bylo sprawdzanie po kolei pierwszych
	   trzech znakow buf, teraz po prostu szukam pierwszego drukowanego
	   znaku */
	{
	    int j = 0;
	    int p = point - buf;

	    do
	    {
		if ( buf[ j ] == '{' )
		    j += 2;
		else if ( buf[ j ] == '`' )
		    j++;
		else
		{
		    buf[ j ] = UPPER( buf[ j ] );
		    break;
		}
	    }
	    while ( j < p );
	}

	pbuff           = buffer;
	colourconv( pbuff, odwroc_wyrazy( buf ), to, sizeof( buffer ) );
	/* Zawija, jesli gracz ma OPC_WRAP. */
	if ( to->desc )
	{
	    if ( CZY_OPCJA( to, OPC_WRAP ) )
	    {
		zawin( buffer, bufek, to->desc->szer, CZY_UTF_8( to ) );
		write_to_buffer( to->desc, bufek, 0 );
	    }
	    else
		write_to_buffer( to->desc, buffer, 0 );
	}
    }

    return;
}


/*
 * przepisane od nowa przez Lama, 22.1.2004
 */
int colour( char type, CHAR_DATA *ch, char *string )
{
    char code[ 40 ];
    char *p = NULL;
    char kod;
    int i, kolor;

    /* zmiana zachowania wzgledem Laca 1.0 - 1.4.-1-pre6, wczesniej nieznany
       kod koloru wysylal lancuch ustawiajacy domyslne atrybuty terminala,
       teraz bedzie cichutko pomijany */
    code[ 0 ] = '\0';

    if ( !ch )
	kolor = 3;
    else
    {
	kolor = ch->kolor;

	kod = kolor_gracza_c( ch, type );
	if ( kod != '-' )
	    type = kod;

	if ( CZY_OPCJA( ch, OPC_CUTCP )
	  && ( type == 'x'
	    || type == '0' ) )
	    type = 'w';
    }

    for ( i = 0; i < MAX_COLOUR; i++ )
	if ( type == *tablica_kolorow[ 0 ][ i ] )
	{
	    strcpy( code, tablica_kolorow[ kolor ][ i ] );
	    p = code;
	    while ( *p != '\0' )
	    {
		*string = *p++;
		*++string = '\0';
	    }
	    return strlen( code );
	}

    p = code;
    while ( *p != '\0' )
    {
	*string = *p++;
	*++string = '\0';
    }

    return strlen( code );
}


/* Lam 22.9.2002: napisane od nowa */
int spolszcz( char type, int polskie, char *string )
{
    char code[ 20 ];
    char *p = code;
    int i;

    if ( polskie > 4 )
	polskie = 0;

    if ( type == '{' )
	return 0;

    if ( type == '`' )
    {
	strcpy( code, "`" );
	while ( *p != '\0' )
	{
	    *string = *p++;
	    *++string = '\0';
	}
	return strlen( code );
    }

    for ( i = 0; i < 18; i++ )
	if ( type == *polska_tablica[ 0 ][ i ] )
	{
	    strcpy( code, polska_tablica[ polskie ][ i ] );
	    while ( *p != '\0' )
	    {
		*string = *p++;
		*++string = '\0';
	    }
	    return strlen( code );
	}

    sprintf( code, "%c", type );
    while ( *p != '\0' )
    {
	*string = *p++;
	*++string = '\0';
    }

    return strlen( code );
}


/*
 * 22.3.99 - jesli ch == NULL, ignorujemy kolory i polskie znaki
 */
#define NAJDLUZSZY_KOLOR 36
void colourconv( char *buffer, const char *txt, CHAR_DATA *ch, unsigned int size )
{
    const char *point;
    char *bufstart = buffer;
    int skip = 0;
    int polskie, kolor;
    char ostatni = '\0';
    size_t roznica;

    /* polskie litery i brak kolorow skracaja bufor, ale przewaznie
       wyjscie bedzie wychodzilo dluzsze niz wejscie */
    if ( strlen( txt ) > size )
	bug( "Bufor wyjsciowy (%d) mniejszy od wejsciowego!", size );

    if ( ch )
    {
	polskie = ch->polskie;
	kolor = ch->kolor;
    }
    else
    {
	polskie = 2;
	kolor = 3;
    }

    if ( /* ch->desc && */ txt )
    {
	/* 4 rozne wersje dla maksymalnej predkosci */
	if ( !kolor && !polskie )
	{
	    for ( point = txt; *point; point++ )
	    {
		if ( *point == '{' )
		{
		    if ( point[ 1 ] == '{' )
		    {
			*buffer = '{';
			*++buffer = '\0';
		    }
		    point++;
		    continue;
		}
		if ( *point == '`' )
		{
		    if ( *++point == 'x' )
		    {
			*buffer = 'z';
			*++buffer = '\0';
			continue;
		    }
		    if ( *point == 'X' )
		    {
			*buffer = 'Z';
			*++buffer = '\0';
			continue;
		    }
		    if ( *point == '`' )
		    {
			*buffer = '`';
			*++buffer = '\0';
			continue;
		    }
		    --point;
		    continue;
		}
		*buffer++ = *point;
	    }
	    *buffer = '\0';
	}
	else if ( kolor && polskie )
	{
	    for ( point = txt; *point; point++ )
	    {
		if ( *point == '{' )
		{
		    point++;
		    if ( *point != ostatni )
		    {
			ostatni = *point;
			buffer += colour( *point, ch, buffer );
		    }
		    continue;
		}
		if ( *point == '`' )
		{
		    point++;
		    skip = spolszcz( *point, polskie, buffer );
		    if ( skip == 0 )
			--point;
		    else
			buffer += skip;
		    continue;
		}
		*buffer++ = *point;
		roznica = buffer - bufstart;
		if ( roznica > size + ( NAJDLUZSZY_KOLOR + 1 ) )
		    break;
	    }
	    *buffer = '\0';
	}
	else if ( kolor && !polskie )
	{
	    for ( point = txt; *point; point++ )
	    {
		if ( *point == '{' )
		{
		    point++;
		    if ( *point != ostatni )
		    {
			ostatni = *point;
			buffer += colour( *point, ch, buffer );
		    }
		    continue;
		}
		if ( *point == '`' )
		{
		    if ( *++point == 'x' )
		    {
			*buffer = 'z';
			*++buffer = '\0';
			continue;
		    }
		    if ( *point == 'X' )
		    {
			*buffer = 'Z';
			*++buffer = '\0';
			continue;
		    }
		    if ( *point == '`' )
		    {
			*buffer = '`';
			*++buffer = '\0';
			continue;
		    }
		    --point;
		    continue;
		}
		*buffer++ = *point;
		roznica = buffer - bufstart;
		if ( roznica > size + ( NAJDLUZSZY_KOLOR + 1 ) )
		    break;
	    }
	    *buffer = '\0';
	}
	else if ( !kolor && polskie )
	{
	    for ( point = txt; *point; point++ )
	    {
		if ( *point == '{' )
		{
		    if ( point[ 1 ] == '{' )
		    {
			*buffer = '{';
			*++buffer = '\0';
		    }
		    point++;
		    continue;
		}
		if ( *point == '`' )
		{
		    point++;
		    skip = spolszcz( *point, polskie, buffer );
		    if ( skip == 0 )
			--point;
		    else
			buffer += skip;
		    continue;
		}
		*buffer++ = *point;
	    }
	    *buffer = '\0';
	}
    }

    return;
}


#if !defined( MAM_GETTIMEOFDAY )
# if (_MSC_VER >= 1400) || (__BORLANDC__ >= 0x0630)
/* VC 8.0+ 2005 | Embarcadero C++ Builder XE implement ULL. */
int gettimeofday( struct timeval *tv, void *tz )
{
    FILETIME ft;
    unsigned __int64 tmpres = 0;

    if ( !tv )
	return 0;

    GetSystemTimeAsFileTime( &ft );

    /* The GetSystemTimeAsFileTime returns the number of 100 nanosecond
       intervals since Jan 1, 1601 in a structure. Copy the high bits to the
       64 bit tmpres, shift it left by 32 then or in the low 32 bits. */
    /* tmpres = (ft.dwHighDateTime << 32 ) | ft.dwLowDateTime; */
    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;

    /* Convert to microseconds by dividing by 10 */
    tmpres /= 10;

    /* The Unix epoch starts on Jan 1 1970.  Need to subtract the difference
       in seconds from Jan 1 1601. */
    tmpres -= 11644473600000000ULL;

    /* Finally change microseconds to seconds and place in the seconds
       value. The modulus picks up the microseconds. */
    tv->tv_sec = ( long int ) ( tmpres / 1000000UL );
    tv->tv_usec = ( long int ) ( tmpres % 1000000UL );

    return 0;
}
# else
/* VC 6.0 | Borland C++ 5.5 */
int gettimeofday( struct timeval *tp, void *tzp )
{
    tp->tv_sec = time( NULL );
#  if defined( WIN32 )
    /* Use tickcount to populate microseconds. */
    tp->tv_usec = ( GetTickCount( ) % 1000 ) * 1000;
#  else
    tp->tv_usec = 0;
#  endif
    return 0;
}
# endif
#endif


/*
 * Lam 24.11.2005: natrafilem na wzmianke o tym, ze dosowa wersja ls nie
 * potrzebuje ansi.sys do kolorowania, poszukalem i faktycznie - zawiera
 * te funkcjonalnosc sama w sobie, szkoda by bylo nie skorzystac.
 *
 * naglowek oryginalny, ale wycialem zbedne funkcje i zostawilem tylko ten
 * emulator ansi.sys, poprawiajac styl
 */
/* MSDOS-specific functions for `ls'.

   Written by Eli Zaretskii <eliz@is.elta.co.il>  */

#ifdef MSDOS
#ifdef __DJGPP__

/*  Screen write redirector.  We need this to support colorization
    without requiring ANSI.SYS driver (or its work-alike) to be loaded.

    This function uses the DJGPP filesystem extensions mechanism.  It is
    installed as a handler for handle-based functions (read/write/close)
    for the standard output (but actually only handles writes, only if
    the standard output is connected to the terminal, and only if user
    asked for colorization).  When a buffer is written to the screen by
    low-level functions of the DJGPP C library, our handler will be
    called.  For any request that doesn't require colored screen writes
    we return a zero to the caller, in which case the caller will handle
    the output in the usual way (by eventually calling DOS).

    When colorization *is* required, the buffer is written directly to
    the screen while converting the ANSI escape sequences into calls to
    DJGPP conio functions which change text attributes.  A non-zero value is
    then returned to the caller to signal that the output has been handled.

    Warning: this function relies on the fact that `ls' disables the use
    of TAB characters when colorization is required, and therefore it
    does NOT expand TABs!  It also doesn't check for NULL characters in
    the buffer, and might end the output prematurely if there are NULLs.  */

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <conio.h>
#include <sys/fsext.h>
#include <go32.h>	/* for `_dos_ds' */
#include <sys/farptr.h>

static int norm_blink = -1, cur_blink = -1;
static unsigned char norm_attr = 0, cur_attr = 0;
static int isatty_stdout = -1;

/* Restore the BIOS blinking bit to its original value.  Called at exit.  */
static void restore_blink_bit( void )
{
    if ( cur_blink != norm_blink )
    {
	if ( norm_blink > 0 )
	    blinkvideo( );
	else
	    intensevideo( );
    }

    return;
}

static int msdos_screen_write( __FSEXT_Fnumber func, int *retval,
				va_list rest_args )
{
    static char *cbuf = 0;
    static size_t cbuf_len = 0;
    /* Only dark colors mentioned here, so that bold has visible effect.  */
    static enum COLORS screen_color[ ] = { BLACK, RED, GREEN, BROWN,
					   BLUE, MAGENTA, CYAN, LIGHTGRAY };
    char *anchor, *p_next;
    unsigned char fg, bg;

    int handle;
    char *buf;
    size_t buflen;

    /* Avoid direct screen writes unless colorization was actually requested.
       Otherwise, we will break programs that catch I/O from their children.  */
    handle = va_arg( rest_args, int );
    if ( func != __FSEXT_write
      || !( handle == STDOUT_FILENO ? isatty_stdout : isatty( handle ) ) )
	return 0;

    buf = va_arg( rest_args, char * );
    if ( !buf )
    {
	errno = EINVAL;
	*retval = -1;
	return 1;
    }

    /* Allocate a sufficiently large buffer to hold the output.  */
    buflen = va_arg( rest_args, size_t );
    if ( !cbuf )
    {
	struct text_info txtinfo;

	cbuf_len = buflen + 1;
	cbuf = (char *) malloc( cbuf_len );
	gettextinfo( &txtinfo );
	norm_attr = txtinfo.attribute; /* save the original text attribute */
	cur_attr = norm_attr;
	/* Does it normally blink when bg has its 3rd bit set?  */
	norm_blink = ( _farpeekb( _dos_ds, 0x465 ) & 0x20 ) ? 1 : 0;
	cur_blink = norm_blink;
    }
    else if ( buflen >= cbuf_len )
    {
	cbuf_len = buflen + 1;
	cbuf = ( char * ) realloc( cbuf, cbuf_len );
    }
    memcpy( cbuf, buf, buflen );
    cbuf[ buflen ] = '\0';

    /* Current text attributes are used as baseline.  */
    fg = cur_attr & 15;
    bg = ( cur_attr >> 4 ) & 15;

    /* Walk the buffer, writing text directly to video RAM,
       changing color attributes when an escape sequence is seen.  */
    for ( anchor = p_next = cbuf;
	 ( p_next = memchr( p_next, ESC, buflen - ( p_next - cbuf ) ) ) != 0; )
    {
	char *p = p_next;

	if ( p[ 1 ] == '[' )	/* "Esc-[" sequence */
	{
	    /* If some chars seen since the last escape sequence,
	       write it out to the screen using current text attributes.  */
	    if ( p > anchor )
	    {
		*p = '\0';	/* `cputs' needs ASCIIZ string */
		cputs( anchor );
		*p = ESC;		/* restore the ESC character */
		anchor = p;
	    }
	    p += 2;		/* get past "Esc-[" sequence */
	    p_next = p;
	    while ( *p != 'm' )	/* `m' ends the escape sequence */
	    {
		char *q;
		long code = strtol( p, &q, 10 );

		/* Sanity checks:

		     q > p unless p doesn't point to a number;
		     ANSI codes are between 0 and 47;
		     Each ANSI code ends with a `;' or an `m'.

		   If any of the above is violated, we just ignore the bogon. */
		if ( q == p || code > 47 || code < 0 || ( *q != 'm' && *q != ';' ) )
		{
		    p_next = q;
		    break;
		}
		if ( *q == ';' )	/* more codes to follow */
		    q++;

		/* Convert ANSI codes to color fore- and background.  */
		switch ( code )
		{
		    case 0:	/* all attributes off */
			fg = norm_attr & 15;
			bg = ( norm_attr >> 4 ) & 15;
			break;
		    case 1:	/* intensity on */
			fg |= 8;
			break;
		    case 4:	/* underline on */
			fg |= 8;	/* we can't, so make it bold instead */
			break;
		    case 5:	/* blink */
			if ( cur_blink != 1 )
			{
			    blinkvideo( ); /* ensure we are'nt in bright bg mode */
			    cur_blink = 1;
			}
			bg |= 8;
			break;
		    case 7:	/* reverse video */
			{
			    unsigned char t = fg;
			    fg = bg;
			    bg = t;

			    /* If it was blinking before, let it blink after.  */
			    if ( fg & 8 )
				bg |= 8;

			    /* If the fg was bold, let the background be bold.  */
			    if ( ( t & 8 ) && cur_blink != 0 )
			    {
				intensevideo( );
				cur_blink = 0;
			    }
			}
			break;
		    case 8:	/* concealed on */
			fg = ( bg & 7 ) | 8;	/* make fg be like bg, only bright */
			break;
		    case 30: case 31: case 32: case 33: /* foreground color */
		    case 34: case 35: case 36: case 37:
			fg = ( fg & 8 ) | ( screen_color[ code - 30 ] & 15 );
			break;
		    case 40: case 41: case 42: case 43: /* background color */
		    case 44: case 45: case 46: case 47:
			bg = ( bg & 8 ) | ( screen_color[ code - 40 ] & 15 );
			break;
		    default:
			p_next = q;	/* ignore unknown codes */
			break;
		}
		p = q;
	    }
	    if ( *p == 'm' && p > p_next )
	    {
		/* They don't *really* want it invisible, do they?  */
		if ( fg == ( bg & 7 ) )
		    fg |= 8;	/* make it concealed instead */

		/* Construct the text attribute and set it.  */
		cur_attr = ( bg << 4 ) | fg;
		textattr( cur_attr );
		p_next = anchor = p + 1;
	    }
	    else
		break;
	}
	else
	    p_next++;
    }

    /* Output what's left in the buffer.  */
    cputs( anchor );
    *retval = buflen;

    return 1;
}


/* This is called before `main' to install our STDOUT redirector.  */
static void __attribute__( ( constructor ) ) djgpp_ls_startup( void )
{
    __FSEXT_set_function( STDOUT_FILENO, msdos_screen_write );
    isatty_stdout = isatty( STDOUT_FILENO );
    atexit( restore_blink_bit );

    return;
}

#endif  /* __DJGPP__ */
#endif  /* MSDOS */



/*
 * InterMud ConTroL, obecnie imster
 * Lam 13.5.98
 */
KOMENDA( do_imctl )
{
#if !defined( MSDOS ) && defined( IMUD )
    extern int imud_socket; /* comm.c */
    extern char *imud_outbuf;
    extern int imud_outtop;
    extern struct sockaddr_in imud_sa;

    if ( !authorized( get_char( ch ), "imster" ) )
	return;

    if ( !str_cmp( argument, "po`l`o`z" ) )
    {
	if ( imud_down )
	{
	    send_to_char( "IM i tak ju`z le`zy.\n\r", ch );
	    return;
	}
	imud_down = TRUE;
	close( imud_socket );
	*imud_outbuf = '\0';
	imud_outtop = 0;
	send_to_all_char( "Po`l`aczenie do IM-a zamkni`ete przez nie`smiertelnego.\n\r" );
	return;
    }
    else if ( !str_cmp( argument, "postaw" ) )
    {
	if ( !imud_down )
	{
	    send_to_char( "IM i tak ju`z stoi.\n\r", ch );
	    return;
	}

	imud_socket = socket( SOCK_AF, SOCK_STREAM, 0 );
	if ( IS_INVALID_SOCKET( imud_socket ) )
	{
	    send_to_char( "Socket :(\n\r", ch );
	    return;
	}

	imud_sa.sin_family = SOCK_AF;
	imud_sa.sin_port   = htons( 1979 );
	imud_sa.sin_addr.s_addr = 0; /* localhost pod Linuksem */
	if ( connect( imud_socket, (struct sockaddr *) &imud_sa,
		sizeof( imud_sa ) ) )
	{
	    send_to_char( "Connect :(\n\r", ch );
	    return;
	}

	if ( fcntl( imud_socket, F_SETFL, O_NONBLOCK ) == -1 )
	{
	    send_to_char( "Fcntl :(\n\r", ch );
	    close( imud_socket );
	    return;
	}

	write_to_descriptor( imud_socket, "LAC\n", 4 );
	imud_down = FALSE;
	*imud_outbuf = '\0';
	imud_outtop = 0;
	send_to_all_char( "Po`l`aczenie z IM-em wsta`lo!\n\r" );
	return;
    }
    else if ( !str_prefix( argument, "status" ) )
    {
	ch_printf( ch, "IM %s.\n\r", imud_down ? "le`zy" : "stoi" );
    }
    STC( "Sk`ladnia: imster po`l`o`z|postaw|status\n\r", ch );
#else
    STC( "Ten Lac zosta`l skompilowany bez obs`lugi IM-a.\n\r", ch );
#endif

    return;
}


#if defined( IMUD )

/*
 * Dzielenie tego, co przysyla IM na linie (pojedyncze komendy)
 */
char *imud_one_command( const char *txt, char *cmd )
{
    char *tmp;
    char *pok;

    tmp = (char *) txt;
    while ( TRUE )
    {
	if ( *tmp == '\0' )
	{
	    *cmd = '\0';
	    return (char *) txt;
	}
	if ( *tmp != '\n' && *tmp != '\r' )
	    break;
	tmp++;
    }
    pok = cmd;
    do
    {
	*pok = *tmp;
	pok++;
	tmp++;
    }
    while ( *tmp != '\n' && *tmp != '\r' && *tmp != '\0' );
    /* ewentualne \n i \r od razu zignorujemy */
    while ( *tmp == '\n' || *tmp == '\r' )
    {
	tmp++;
    }
    *pok = '\0'; /* zakonczyc lancuch */

    return tmp;
}


/*
 * Interpreter tego, co przychodzi z IM-a.
 * 25.4.98 - zaczatki kodu (imud)
 * 8.5.98  - mud_on, mud_off, player_on, player_off
 *
 * Miedzy mudami komunikacja dziala b. dobrze, problem jest z ircem - irc nie
 * ma takich restrykcji jak my, wiec ludzie moga bipac w srodku tekstu i ludzi
 * w mudzie bipa. Tak jest tez z innymi znakami kontrolnymi. Nie przewidziana
 * jest obsluga polskich liter. Jesli ktos na ircu powie zbyt dlugi tekst (np.
 * pol strony tekstu za jednym privmsg), mud pada, bo nie jest przed tym
 * zabezpieczony. Jesli w mudzie, ktory sie do nas podlaczy, MAX_INPUT_LENGTH
 * bedzie inne niz u nas, tez pewnie beda pady. Jesli kiedys dojdzie
 * do zastosowania tego mechanizmu (pisze to 9.2.99), trzeba bedzie o tym
 * pomyslec.
 */
void upper_window( CHAR_DATA *ch );
void lower_window( CHAR_DATA *ch );
void imud_interp( const char *txt )
{
    char buf1 [ MAX_STRING_LENGTH ];
    char mud  [ MAX_INPUT_LENGTH ];
    char plr  [ MAX_INPUT_LENGTH ];
    char *lst;
    char *cmd;
    static char komenda[ MAX_STRING_LENGTH * 4 ]; /* na wypadek kto */
    DESCRIPTOR_DATA *d;

    if ( !*txt ) return;

    lst = (char *) txt;
while ( 1 )
{
    cmd = &komenda[ 0 ];
    lst = imud_one_command( lst, cmd );
    if ( !*cmd )
	return;
    cmd = one_argument2( cmd, buf1 );
    if ( !str_cmp( buf1, "imud" ) )
    {
	if ( !*cmd ) return;
	cmd = one_argument2( cmd, mud ); /* zawiera teraz muda */
	cmd = one_argument2( cmd, plr ); /* zawiera teraz gracza */
	/* cmd zawiera tekst */
	if ( !str_cmp( mud, "lac" ) )
	    strcpy( mud, "Laca" );
	if ( !str_cmp( mud, "agat" ) )
	    strcpy( mud, "agata" );
	sprintf( buf1, "{W%s z %s imudzi: \"%s{W\"{x\n\r", plr, mud, cmd );
	buf1[ 2 ] = UPPER( buf1[ 2 ] );
	for ( d = descriptor_list; d; d = d->next )
	{
	    CHAR_DATA *vch;
	    CHAR_DATA *och;

	    och = d->original ? d->original : d->character;
	    vch = d->character;

	    if ( d->connected == CON_PLAYING
		&& !IS_SET( och->deaf, CHANNEL_IM )
		&& !IS_SET( och->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
	    {
		/* ignoruje sie imieniem, wiec dziala takze na imud */
		if ( !IS_NPC( och ) && is_name2( plr, och->pcdata->ignore ) )
		    continue;
		upper_window( vch );
		send_to_char( buf1, vch );
		lower_window( vch );
	    }
	}
    }
    else if ( !str_cmp( buf1, "ircsay" ) )
    {
	if ( !*cmd ) return;
	cmd = one_argument2( cmd, plr );
	sprintf( buf1, "{G%s na #mudpl ircuje: \"%s{G\"{x\n\r", plr, cmd );
	buf1[ 2 ] = UPPER( buf1[ 2 ] ); /* irc pozwala rozpoczynac mala litera */
	for ( d = descriptor_list; d; d = d->next )
	{
	    CHAR_DATA *vch;
	    CHAR_DATA *och;

	    och = d->original ? d->original : d->character;
	    vch = d->character;

	    if ( d->connected == CON_PLAYING
		&& !IS_SET( och->deaf, CHANNEL_IM )
		&& !IS_SET( och->in_room->room_flags, ROOM_CONE_OF_SILENCE ) )
	    {
		/* ignoruje sie imieniem, wiec dziala takze na irca */
		if ( !IS_NPC( och ) && is_name2( plr, och->pcdata->ignore ) )
		    continue;
		upper_window( vch );
		send_to_char( buf1, vch );
		lower_window( vch );
	    }
	}
    }
    else if ( !str_cmp( buf1, "mud_on" ) )
    {
	cmd = one_argument2( cmd, mud );
	sprintf( buf1, "{GDo IM-a pod`l`aczy`l si`e mud: %s{x\n\r", mud );
	send_to_all_char( buf1 );
    }
    else if ( !str_cmp( buf1, "mud_off" ) )
    {
	cmd = one_argument2( cmd, mud );
	sprintf( buf1, "{MZ IM-em roz`l`aczy`l si`e mud: %s{x\n\r", mud );
	send_to_all_char( buf1 );
    }
    else if ( !str_cmp( buf1, "player_on" ) )
    {
	cmd = one_argument2( cmd, mud );
	cmd = one_argument2( cmd, plr );
	if ( !str_cmp( mud, "lac" ) )
	    strcpy( mud, "Laca" );
	if ( !str_cmp( mud, "agat" ) )
	    strcpy( mud, "agata" );
	sprintf( buf1, "{M%s{x wchodzi na {G%s{x", plr, mud );
	wiznet( buf1, NULL, NULL, NULL, WIZ_LOGIN, 0, 0 );
    }
    else if ( !str_cmp( buf1, "player_off" ) )
    {
	cmd = one_argument2( cmd, mud );
	cmd = one_argument2( cmd, plr );
	if ( !str_cmp( mud, "lac" ) )
	    strcpy( mud, "Laca" );
	if ( !str_cmp( mud, "agat" ) )
	    strcpy( mud, "agata" );
	sprintf( buf1, "{M%s{x wychodzi z {G%s{x", plr, mud );
	wiznet( buf1, NULL, NULL, NULL, WIZ_LOGIN, 0, 0 );
    }
} /* while */

    return;
}

#endif /* #if defined( IMUD ) */
