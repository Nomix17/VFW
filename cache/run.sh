while [[ $# -gt 0 ]]; do 
  case "$1" in
    --fullpath | -p) 
      shift
      mediapath="$1" 
      ;;
   --nameonly | -n)
      shift
      mediapath="$(pwd)/$1"
      ;;

    *)
      echo "Unknown option: $1" 
      exit 1
      ;;

  esac
  shift

done
echo "$mediapath"
cd "/opt/VFW/bin/"
./VFW "$mediapath"
