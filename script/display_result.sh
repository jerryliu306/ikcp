cat $1 | grep cost | awk -F: 'BEGIN{e=0; max=0; min=9999999;} {print $2; total+=$2; n+=1; if($2>536)e+=1; if($2>max)max=$2; if($2<min)min=$2;} END{print total/n, e, max, min;}'
