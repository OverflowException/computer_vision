set -e

g++ distributor.cpp -o distributor.out -std=c++0x > compile.log 2>&1
g++ render.cpp -o render.out -std=c++0x >> compile.log 2>&1

./distributor.out 60 > init.data

mkdir -p imgs

idx=1
while read params
do
    imgname="${idx}.bmp"
    ./render.out $params ./imgs/$imgname &
    idx=`expr $idx + 1`
done < ./init.data

