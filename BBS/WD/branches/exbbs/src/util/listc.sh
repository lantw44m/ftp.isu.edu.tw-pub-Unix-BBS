
files=$(ls *.c | awk -F' ' '{print $1}')

let "count = 0"
for i in $files
do
  let "count = $count + 1"
  if [ $count -gt 5 ]
  then 
    let "count = 0"
    printf "\\\\"
    printf "\n"
  fi
  i=${i%.*}
  printf "%s " $i
done
