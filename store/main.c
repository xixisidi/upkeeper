#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include "upk_db.h"

int DEBUG = 0;

int main( 
    int   argc, 
    char *argv[] 
) {
    sqlite3 *pdb;
    char    *file = "store.sqlite";
    int      rc;
    int      i;
    char    *event;
    char    *status;
    char    *service;
    char    *package;
    char    *cp;

    /* test */
    upk_test_is( 1, 1 );

    rc = db_init( file, &pdb );

    if(rc < 0) {
	printf("db_init failed. Exiting.\n");
	exit(-1);
    }
 
    for( i=0; i<=2; i++ ) {
        service = sqlite3_mprintf("service-%d", i);
        rc = upk_db_service_find_or_create(
                pdb, "package", service );
        sqlite3_free( service );
    }

    for( i=0; i<=5; i++ ) {
        service = sqlite3_mprintf("service-%d", i);
        package = sqlite3_mprintf("package-%d", i);
        upk_db_service_actual_status( pdb, package, service, "stop");
        upk_db_service_desired_status( pdb, package, service, "start");
        sqlite3_free( service );
        sqlite3_free( package );
    }
    
    service = sqlite3_mprintf("service-10", i);
    package = sqlite3_mprintf("package-10", i);
    upk_db_service_actual_status( pdb, package, service, "stop");
    sqlite3_free( service );
    sqlite3_free( package );

    cp = upk_db_service_actual_status( pdb, "package-1", "service-1", NULL );
    upk_test_eq( cp, "stop" );
    cp = upk_db_service_actual_status( pdb, "package-2", "service-2", NULL );
    upk_test_eq( cp, "stop" );
    cp = upk_db_service_actual_status( pdb, "package-5", "service-5", NULL );
    upk_test_eq( cp, "stop" );
    cp = upk_db_service_actual_status( pdb, "package-1", "service-2", NULL );
    upk_test_eq( cp, NULL );
    cp = upk_db_service_actual_status( pdb, "package-0", "service-2", NULL );
    upk_test_eq( cp, NULL );
    cp = upk_db_service_actual_status( pdb, "package-1", "service-9", NULL );
    upk_test_eq( cp, NULL );
    cp = upk_db_service_actual_status( pdb, "package-10", "service-10", NULL );
    upk_test_eq( cp, "stop" );

    upk_db_status_checker( pdb, upk_db_status_checker_launchcallback );

    upk_db_exec_single( pdb, "SELECT signal_send( 1, 456456 )" );

    sqlite3_close( pdb );

    return(0);
}
