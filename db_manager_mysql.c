#include "db_manager_mysql.h"

/*------------------------------------*/
/* Connect to database*/
int connect_to_db(char *server, char *user, char *password,
		char *database){   
	if (db_connection){
		return(1);
	}	
	db_connection = mysql_init(NULL);
	if (!mysql_real_connect(db_connection, server,
		user, password, database, 0, NULL, 0)) {
		fprintf(stderr, "%s\n", mysql_error(db_connection));
		return(1);
	}
	return(1);
}

int disconnect_from_db(){   
	if (db_connection){
		mysql_close(db_connection);
	}
	return(1);
}

int query(char *qstring){
	if (db_connection){
		if (mysql_query(db_connection, qstring)) {
			fprintf(stderr, "%s\n", mysql_error(db_connection));
			return(0);
		}
		return(1);
	}
	else{
		printf("SQL connection not defined!");
		return(0);
	}
}

/*---------------------------------------------------*/
void DateTimeToSQL(time_t timeval,TIMSTM time_stmp){
	struct tm *tm_ptr;
	tm_ptr = gmtime(&timeval);
	sprintf(time_stmp,"%d-%02d-%02d %02d:%02d:%02d", tm_ptr->tm_year+1900, tm_ptr->tm_mon+1, tm_ptr->tm_mday, tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec);
	//printf("%d-%02d-%02d %02d:%02d:%02d", tm_ptr->tm_year+1900, tm_ptr->tm_mon+1, tm_ptr->tm_mday, tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec);
}
/*
void StrToSQL(char *in, char *out){
	mysql_real_escape_string(db_connection,out,in,strlen(in)*2+1);
	sprintf(out,"'%s'",out);
}
*/
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
        "INSERT INTO `%s`.`%s` SET "\
              "`%s`='%s',"\
              "`%s`='%s',"\
              "`%s`='%s',"\
              "`%s`='%s',"\
              "`%s`='%c',"\
              "`%s`='%c',"\
              "`%s`=%f,"\
              "`%s`=%f,"\
              "`%s`=%f,"\
              "`%s`=%d,"\
              "`%s`=%d,"\
              "`%s`=%f,"\
              "`%s`=%f,"\
              "`%s`=%f,"\
              "`%s`=%f,"\
              "`%s`=%f,"\
              "`%s`='%s',"\
              "`%s`=%d,"\
              "`%s`=%d,"\
              "`%s`=%d",
            database, DB_TABLE,
            DB_COL_ID, PosRepData->id,
            DB_COL_PERIOD, tr_dt,
            DB_COL_LAT, PosRepData->lat,
            DB_COL_LON, PosRepData->lon,
            DB_COL_NS, PosRepData->ns,
            DB_COL_EW, PosRepData->ew,
            DB_COL_HEADING, PosRepData->heading,
            DB_COL_SPEED, PosRepData->speed,
            DB_COL_ODOMETER, PosRepData->odometer,
            DB_COL_SENS_IN, PosRepData->sensorsIn,
            DB_COL_SENS_OUT, PosRepData->sensorsOut,
            DB_COL_VOLTAGE, PosRepData->voltage,
            DB_COL_SENSOR1, PosRepData->sensor1,
            DB_COL_SENSOR2, PosRepData->sensor2,
            DB_COL_LON_DEG, PosRepData->lon_deg,
            DB_COL_LAT_DEG, PosRepData->lat_deg,
            DB_COL_RECIEVED, cur_dt,
            DB_COL_VALID, PosRepData->gpsStatus,
            DB_COL_ENGINE_ON, PosRepData->engine,
            DB_COL_FROM_MEM, PosRepData->fromMemory
	);
	//syslog(LOG_INFO,"TRACKIG q=%s",query_buf);	
	query(query_buf);
	//puts(query_buf);
}
