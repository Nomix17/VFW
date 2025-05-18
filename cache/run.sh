dirpath=$(pwd)
mediapath="$dirpath/$1"
cd "/opt/VFW/bin/"
if [ "$1" != "" ]; then
	./VFW "$mediapath"
else
	./VFW
fi

