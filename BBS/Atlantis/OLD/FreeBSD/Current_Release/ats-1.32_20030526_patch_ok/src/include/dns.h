/*-------------------------------------------------------*/
/* lib/dns.h            ( NTHU CS MapleBBS Ver 3.00 )    */
/*-------------------------------------------------------*/
/* target : header file for DNS routines                 */
/* create : 96/11/20                                     */
/* update : 02/08/21 (Dopin)                             */
/* Dopin  : 我把 dns.h dns_query / dns_open 合在一起     */
/*-------------------------------------------------------*/

#ifndef _DNS_H_

#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <resolv.h>

#ifndef INADDR_NONE
#define INADDR_NONE     0xffffffff
#endif
#define INADDR_FMT      "%u.%u.%u.%u"

typedef union {
  unsigned char d[4];
  unsigned long addr;
}     ip_addr;

/*
 * The standard udp packet size PACKETSZ (512) is not sufficient for some
 * nameserver answers containing very many resource records. The resolver may
 * switch to tcp and retry if it detects udp packet overflow. Also note that
 * the resolver routines res_query and res_search return the size of the
 * un*truncated answer in case the supplied answer buffer it not big enough
 * to accommodate the entire answer.
 */

#if     PACKETSZ > 1024
#define MAXPACKET       PACKETSZ
#else
#define MAXPACKET       1024    /* max packet size used internally by BIND */
#endif

#define MAX_MXLIST      1024

typedef union {

  HEADER hdr;
  u_char buf[MAXPACKET];
}  querybuf;                 /* response of DNS query */


static inline unsigned short
getshort(unsigned char *c) {
  unsigned short u;

  u = c[0];
  return (u << 8) + c[1];
}

/* domain name */ /* type of query */ /* buffer to put answer */
dns_query(char *name, int qtype, querybuf *ans) {
   querybuf buf;

   qtype = res_mkquery(QUERY, name, C_IN, qtype, (char *) NULL, 0, NULL,
   (char *) &buf, sizeof(buf));

   if(qtype >= 0) {
      qtype = res_send((char *) &buf, qtype, (char *)ans, sizeof(querybuf));

      /* avoid problems after truncation in tcp packets */

      if (qtype > sizeof(querybuf))
      qtype = sizeof(querybuf);
   }

  return qtype;
}

int dns_open(char *host, int port) {
   querybuf ans;
   int n, ancount, qdcount;
   unsigned char *cp, *eom;
   char buf[MAXDNAME];
   struct sockaddr_in sin;
   int type;

#if 1
  /* Thor.980707 : 因 gem.c 呼叫時可能將 host 用 ip 放入，故作特別處理 */
   if(*host>='0' && *host<='9') {
       for(n=0, cp=host; n<4; n++, cp++) {
          buf[n]=0;
          while(*cp >= '0' && *cp <='9') {
             buf[n] *= 10;
             buf[n] += *cp++ - '0';
          }

          if(!*cp) break;
        }
        if(n==3) {
           cp = buf;
           goto ip;
       }
   }
  /* Thor.980707 : 隨便寫寫，要講究完全 match 再說 :p */
#endif

   n = dns_query(host, T_A, &ans);
   if(n < 0) return n;

   /* find first satisfactory answer */
   cp = (u_char *) & ans + sizeof(HEADER);
   eom = (u_char *) & ans + n;

   for(qdcount = ntohs(ans.hdr.qdcount); qdcount--; cp += n + QFIXEDSZ)
     if ((n = dn_skipname(cp, eom)) < 0)
      return n;

   ancount = ntohs(ans.hdr.ancount);
   while(--ancount >= 0 && cp < eom) {
      if((n = dn_expand((u_char *)&ans, eom, cp, buf, MAXDNAME)) < 0)
         return -1;

      cp += n;

      type = getshort(cp);
      n = getshort(cp + 8);
      cp += 10;

      if(type == T_A) {
         int sock;
         ip_addr *ip;
#if 1
ip:
#endif
         sin.sin_family = AF_INET;
         sin.sin_port = htons(port);
         memset(sin.sin_zero, 0, sizeof(sin.sin_zero));

         ip = (ip_addr *) & (sin.sin_addr.s_addr);
         ip->d[0] = cp[0];
         ip->d[1] = cp[1];
         ip->d[2] = cp[2];
         ip->d[3] = cp[3];

         sock = socket(AF_INET, SOCK_STREAM, 0);
         if(sock < 0) return sock;

         if(!connect(sock, (struct sockaddr *) & sin, sizeof(sin))) return sock;

         close(sock);
      }

      cp += n;
   }

   return -1;
}

#endif  _DNS_H_
