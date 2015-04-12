
CFLAGS:= -m64 -g -w -std=c99
INCLUDES:= -I../../include -I../../../drmkernel/include \
	   -I../../../../../../../frameworks/native/include \
	   -I/usr/include/x86_64-linux-gnu \
	   -I../../../../../../../system/core/include 

TEST_OBJS:= decrypt.o
TEST_SRCS:= $(TEST_OBJS:%.o=%.c)
CRYPTO_OBJS:= CryptoKernel.o
CRYPTO_SRCS:= $(CRYPTO_OBJS:%.o=../%.c)
DRM_OBJS:= DrmKernel.o nvDatabase.o nvDatabaseSecureTable.o
DRM_SRCS:= $(DRM_OBJS:%.o=../../../drmkernel/src/%.c)

OBJS:= $(TEST_OBJS) $(CRYPTO_OBJS) $(DRM_OBJS)
SRCS:= $(TEST_SRCS) $(CRYPTO_SRCS) $(DRM_SRCS)

.PHONY: all clean

all: decryptTest

clean:
	/bin/rm -f decryptTest *.o

decryptTest: $(OBJS)
	gcc -m64 -g $(OBJS) -o $@ -lsqlite3 -lcrypto

decrypt.o: decrypt.c decrypt_data.h
CryptoKernel.o: ../CryptoKernel.c ../../include/CryptoKernel.h
DrmKernel.o: ../../../drmkernel/src/DrmKernel.c ../../../drmkernel/include/DrmKernel.h ../../../drmkernel/include/nvDatabase.h ../../../drmkernel/include/nvDatabaseSecureTable.h
nvDatabase.o: ../../../drmkernel/src/nvDatabase.c ../../../drmkernel/include/nvDatabase.h ../../../drmkernel/include/nvDatabaseSecureTable.h
nvDatabaseSecureTable.o: ../../../drmkernel/src/nvDatabaseSecureTable.c ../../../drmkernel/include/nvDatabaseSecureTable.h

%.o: %.c
	gcc $(CFLAGS) -DCRYPTO_HOST_KERNEL_TEST=1 $(INCLUDES) -c $< -o $@

%.o: ../%.c
	gcc $(CFLAGS) -DCRYPTO_HOST_KERNEL_TEST=1 $(INCLUDES) -c $< -o $@

%.o: ../../../drmkernel/src/%.c
	gcc $(CFLAGS) -DDRM_HOST_KERNEL_TEST=1 -DCRYPTO_HOST_KERNEL_TEST=1 $(INCLUDES) -c $< -o $@
