#ifdef ZBBSD_FREE_PORT
#define MAX_ACS_ZP 2

usint zbbsd_port[][2] = {   /* 前面是站台編號 後面是埠 釋出版預設只有一站台 */
   { 0, 23 }, { 0, 3001 }   /* 故悉數設為編號 0 以後請 1, 2, 3 ... 以此類推 */
};
#endif

#ifdef MBBSD_FREE_PORT
#define MAX_ACS_MP 5

usint mbbsd_port[][2] = {
   { 0, 9001}, { 0, 9002 }, { 0, 9003 }, { 0, 9004 }, { 0, 9005 }
};
#endif

int port_2_station(char daemon_model, usint port) {
   int i;
   int s_n = -1;

#ifdef ZBBSD_FREE_PORT
   if(daemon_model == 'Z')
      for(i = 0 ; i < MAX_ACS_ZP ; i++)
         if(zbbsd_port[i][1] == port) {
            s_n = zbbsd_port[i][0];
            break;
         }
#endif

#ifdef MBBSD_FREE_PORT
   if(daemon_model == 'M')
      for(i = 0 ; i < MAX_ACS_MP ; i++)
         if(mbbsd_port[i][1] == port) {
            s_n = mbbsd_port[i][0];
            break;
         }
#endif

   return s_n;
}

int station_2_port(char daemon_model, uschar station_number) {
   int i;
   int p_n = -1;

#ifdef ZBBSD_FREE_PORT
   if(daemon_model == 'Z')
      for(i = 0 ; i < MAX_ACS_ZP ; i++)
         if(zbbsd_port[i][0] == station_number) {
            p_n = zbbsd_port[i][1];
            break;
         }
#endif

#ifdef MBBSD_FREE_PORT
   if(daemon_model == 'M')
      for(i = 0 ; i < MAX_ACS_MP ; i++)
         if(mbbsd_port[i][0] == station_number) {
            p_n = mbbsd_port[i][1];
            break;
         }
#endif

   return p_n;
}
