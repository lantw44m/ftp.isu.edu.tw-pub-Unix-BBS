#
#    �Ұ� bbs ���M�A�Ȫ� Script�A�b /etc/rc.local �[�J startbbs.sh�A
#    �άO��o���ɮץ�� /usr/local/etc/rc.d ����i�A�o�˶}���K�i�۰�
#    ����C
#
#    P.S.�ϥΫe�O�o chmod +x startbbs.sh
#
#!/bin/sh
if [ -x /home/bbs/bin/bbsd ]; then
	/home/bbs/bin/bbsd 23 && echo -n ' bbsd'
fi
#if [ -x /home/bbs/bin/bbspop3d ]; then
#	/home/bbs/bin/bbspop3d && echo -n ' bbspop3d'
#fi
#if [ -x /home/bbs/bin/gopherd ]; then
#	/home/bbs/bin/gopherd && echo -n ' gopherd'
#fi
#if [ -x /home/bbs/bin/fingerd ]; then
#	/home/bbs/bin/fingerd && echo -n ' fingerd'
#fi
if [ -x /home/bbs/innd/innbbsd ]; then
	su bbs -c '/home/bbs/innd/innbbsd' && echo -n ' innbbsd'
fi
if [ -f /home/bbs/etc/cron.bbs ]; then
	su bbs -c 'crontab /home/bbs/etc/cron.bbs' && echo -n ' bbs crontab'
fi
