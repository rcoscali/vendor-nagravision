openssl genrsa -out <keyfilename>.key -rand /dev/urandom 2048
openssl pkcs8 -in <keyfilename>.key -inform PEM -outform PEM -out <keyfilename>.pk8 -topk8
openssl rsa -in <keyfilename>.key -pubout -out <keyfilename>_pub.key
