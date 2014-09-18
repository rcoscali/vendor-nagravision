openssl req -outform DER -out <certfilename>.csr -key <certowner>.key -rand /dev/urandom -new -utf8
openssl req -x509 -inform DER -in <certfilename>.csr -key <certowner>.key -outform DER -out <certfilename>.crt -days 300 -utf8
openssl x509 -inform DER -in <certfilename>.crt -out <certfilename>.pem -outform PEM
