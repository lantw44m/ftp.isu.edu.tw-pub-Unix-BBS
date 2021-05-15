#!/bin/bash
#���{���ΨӦ۰ʧ�sitoc-Maple��������ذϡC
#�Ф��n�����U���U�Ӫ���l�ɡA�]���n�@�������P�_���ΡC
#�ѼƽХ[�Witoc�������W�C
#�Ϊk: updategem.sh itoc
dir=~/tmp
if [ ! -d $dir ]; then
	mkdir $dir
fi
if [ -z "$1" ]; then
	echo "�ѼƽХ[�Witoc�������W�C"
	echo '�Ϊk: ./updategem.sh itoc(�o�Oitoc�������W)'
	echo "usage: ./updategem.sh itoc"
	exit
else
	brd=`ls ~/gem/brd|grep "\<$1\>"`
	if [ -z "$brd" ]; then
		echo "$1�o�Ӫ����s�b"
		echo "$1 doesn't exist"
		exit
	fi
fi
oldfile=`ls $dir|grep itoc_gem`
newfile=`lynx -dump http://processor.tfcis.org | grep http | grep itoc_gem|awk '{print $2}'`
if [ -f $dir/$oldfile ]; then
	oldfiledate=`echo "$oldfile"| sed -e 's/itoc_gem_//' -e 's/.tgz//'`
	newfiledate=`echo "$newfile"| sed -e 's/^.*itoc_gem_//' -e 's/.tgz//'`
	if [ "$oldfiledate" == "$newfiledate" ]; then
		echo "��ذϤw�g�O�̷s���A�ثe���ݧ�s�C"
		echo "It's already up to date."
		exit
	else
		rm $dir/$oldfile
	fi
fi
filename=${newfile##*/}
lynx --dump "$newfile" > "$dir"/"$filename"
tar zxf $dir/$filename -C $dir
cp -rf $dir/gem/* ~/gem/brd/$1/
rm -rf $dir/gem
echo "$1����ذϧ�s����"
echo "$1 gem update successful"
