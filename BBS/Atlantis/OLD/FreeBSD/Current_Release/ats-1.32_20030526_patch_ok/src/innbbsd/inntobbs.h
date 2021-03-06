#ifndef INNTOBBS_H
#define INNTOBBS_H

enum HeaderValue {
SUBJECT_H, FROM_H, DATE_H, MID_H, NEWSGROUPS_H,
NNTPPOSTINGHOST_H, NNTPHOST_H, CONTROL_H, PATH_H,
ORGANIZATION_H, X_Auth_From_H, APPROVED_H, DISTRIBUTION_H, REFERENCES_H,
/* Dopin: For RFC 2045 by PaulLiu.bbs@processor.tfcis.org */
// KEYWORDS_H, SUMMARY_H, LASTHEADER,
KEYWORDS_H, SUMMARY_H, MIME_VERSION_H, MIME_CONTENTTYPE_H,
MIME_CONTENTENCODE_H, LASTHEADER,
/* Dopin */
};

#if !defined(PalmBBS)
extern char *HEADER[];
extern char *BODY;
extern char *FROM, *SUBJECT, *SITE, *DATE, *POSTHOST,
            *NNTPHOST, *PATH, *GROUPS, *MSGID, *CONTROL;
extern char *REMOTEHOSTNAME, *REMOTEUSERNAME;
#else
extern char **XHEADER;
extern char *BODY;
extern char *FROM, *SUBJECT, *SITE, *DATE, *POSTHOST,
            *NNTPHOST, *XPATH, *GROUPS, *MSGID, *CONTROL;
extern char *REMOTEHOSTNAME, *REMOTEUSERNAME;
#endif

int receive_article();

#if defined(PalmBBS)
#ifndef INNTOBBS
#ifndef PATH
# define PATH XPATH
#endif
#ifndef HEADER
# define HEADER XHEADER
#endif
#endif
#endif

#endif
