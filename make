#!/bin/sh
gcc -c trserv.c server.c db_manager_pgsql.c tracker_report_syst.c common.c -I$(pg_config --includedir)
gcc trserv.o server.o db_manager_pgsql.o tracker_report_syst.o common.o -L$(pg_config --libdir)  -lpq -o trpgserv
