#include "db_manager_pgsql.h"

/*------------------------------------*/
int reconnect_to_db(){
	int tries = DB_CONNECT_TRIES;
    while (tries){
		if (PQstatus(db_connection) == CONNECTION_OK){
			return 1;
		}
		PQreset(db_connection);
		tries--;
		sleep(1);
    }
	return 0;
}

/* Connect to database*/
void connect_to_db(char *server, char *user, char *password,
		char *database){   
	
	if (!db_connection){
		db_connection = PQsetdbLogin(server,
				"5432",
				NULL,
				NULL,
				database,user,password);
	}
	time_t t;
	time(&t);			
	while (!reconnect_to_db()){
        fprintf(stderr,"%s %s\n",
                ctime(&t),
				PQerrorMessage(db_connection));		
		sleep(5);
	}
}

int disconnect_from_db(){   
	if (db_connection){
		PQfinish(db_connection);
	}
	return(1);
}

int query(char *qstring){
	int ret_res = 0;
	
	time_t t;
	time(&t);			
	
	if (db_connection){
		while (!reconnect_to_db()){
			fprintf(stderr,"%s %s\n",
					ctime(&t),
					PQerrorMessage(db_connection));					
			sleep(5);
		}
		
		PGresult   *res;
		res = PQexec(db_connection, qstring);
		if (res){
			ExecStatusType res_stat;
			res_stat = PQresultStatus(res);
			
			if (res_stat==PGRES_COMMAND_OK
			||res_stat==PGRES_EMPTY_QUERY
			||res_stat==PGRES_TUPLES_OK){
				ret_res = 1;
			}
			else{				
				fprintf(stderr, "%s QUERY failed: %s",
				ctime(&t),
				PQerrorMessage(db_connection));
			}
			PQclear(res);
		}
		else{
			fprintf(stderr,"%s QUERY failed: %s",
				ctime(&t),
				PQerrorMessage(db_connection));
		}
	}
	else{
		fprintf(stderr,"%s SQL connection not defined!",
			ctime(&t));
	}
	return ret_res;
}

/*---------------------------------------------------*/
void DateTimeToSQL(time_t timeval,TIMSTM time_stmp){
	struct tm *tm_ptr;
	tm_ptr = gmtime(&timeval);
	sprintf(time_stmp,"%d-%02d-%02d %02d:%02d:%02d", tm_ptr->tm_year+1900, tm_ptr->tm_mon+1, tm_ptr->tm_mday, tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec);
}

time_t GetGMTTime(const time_t *timeval){
	struct tm *tm_ptr;
	tm_ptr = gmtime(timeval);
	tm_ptr->tm_hour-=GMT_TIME_DIFF;
	return mktime(tm_ptr);
}

void write_position_report(PPOSITION_DATA PosRepData){
	TIMSTM tr_dt;	
	DateTimeToSQL(PosRepData->dt, tr_dt);	
	
	TIMSTM cur_dt;
	time_t the_time;	
	(void) time(&the_time);
	//time_t gmt_time;
	//gmt_time = GetGMTTime(&the_time);
	DateTimeToSQL(the_time, cur_dt);	
	
	char query_buf[QUERY_BUF_SIZE];
	sprintf(query_buf,
	//printf(
        "INSERT INTO %s ("\
		"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,"\
		"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)"\
		"VALUES("\
                "'%s','%s','%s','%s','%c','%c',%f,%f,"\
                "%f,%d,%d,%f,%f, %f,%f,%f,"\
               "'%s',%d,%d,"\
	       "%d )",
            DB_TABLE,
            DB_COL_ID,DB_COL_PERIOD,DB_COL_LAT,DB_COL_LON,DB_COL_NS,DB_COL_EW,DB_COL_HEADING,DB_COL_SPEED,
	    DB_COL_ODOMETER,DB_COL_SENS_IN,DB_COL_SENS_OUT,DB_COL_VOLTAGE,DB_COL_SENSOR1,
	    DB_COL_SENSOR2,DB_COL_LON_DEG,DB_COL_LAT_DEG,DB_COL_RECIEVED,DB_COL_VALID,DB_COL_ENGINE_ON,
	    DB_COL_FROM_MEM,
	    PosRepData->id,
            tr_dt,
            PosRepData->lat,
            PosRepData->lon,
            PosRepData->ns,
            PosRepData->ew,
            PosRepData->heading,
            PosRepData->speed,
            PosRepData->odometer,
            PosRepData->sensorsIn,
            PosRepData->sensorsOut,
            PosRepData->voltage,
            PosRepData->sensor1,
            PosRepData->sensor2,
            PosRepData->lon_deg,
            PosRepData->lat_deg,
            cur_dt,
            PosRepData->gpsStatus,
            PosRepData->engine,
            PosRepData->fromMemory
	);
	//syslog(LOG_INFO,"TRACKIG q=%s",query_buf);	
	query(query_buf);
	//puts(query_buf);
}
