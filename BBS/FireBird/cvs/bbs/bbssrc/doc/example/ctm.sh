#!/bin/sh
# �� shell script �Q�� ctm �@����s�h�� ctm Delta
# NAME ���Ʀr���e���ɦW
# �Ϊk : �N ctm.sh ���n�Ѷ}���ؿ� ex. /home/bbsadm/bbssrc�ACTM�ɰ��]��b
#	 /home/bbsadm/CTM�A�Q�n��s�s�� 2 - 23 �� Delta
#
#	cd /home/bbsadm/bbssrc
#	ctm.sh /home/bbsadm/CTM 2 23
#					������D�� mail: skyo@mgt.ncu.edu.tw
#
#   $Id: ctm.sh,v 1.1 2000/01/15 01:45:24 edwardc Exp $    
NAME="fb3src-"
A=$2
while [ $3 -ge $A ]
do
	ctm -v $1/$NAME$A.gz
        A=`expr $A + 1 `
done
