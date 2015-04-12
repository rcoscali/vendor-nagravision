TESTS= checkRecordCrypto onSaveRights onCheckRightsStatus dbTest


CFLAGS= -m64 -g -w -DDRM_HOST_KERNEL_TEST=1 -std=c99 -I/usr/include/x86_64-linux-gnu -I../../include -I../../../../../../../frameworks/native/include -I../../../../../../../system/core/include 
LDFLAGS= -m64 -g -w -std=c99 

CHECK_RECORD_CRYPTO_OBJS:= checkRecordCrypto.o
ON_SAVE_RIGHTS_OBJS:= onSaveRights.o
ON_CHECK_RIGHTS_STATUS_OBJS:= onCheckRightsStatus.o
DBTEST_OBJS:= dbTest.o nvDatabase.o nvDatabaseSecureTable.o DrmKernel.o

all: $(TESTS)

.PHONY: all clean tests

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

%.o: ../%.c
	gcc $(CFLAGS) -c $< -o $@

checkRecordCrypto: $(CHECK_RECORD_CRYPTO_OBJS)
	gcc $(LDFLAGS) $(CHECK_RECORD_CRYPTO_OBJS) -o $@ -lcrypto

onSaveRights: $(ON_SAVE_RIGHTS_OBJS)
	gcc $(LDFLAGS) $(ON_SAVE_RIGHTS_OBJS) -o $@ -lcrypto

onCheckRightsStatus: $(ON_CHECK_RIGHTS_STATUS_OBJS)
	gcc $(LDFLAGS) $(ON_CHECK_RIGHTS_STATUS_OBJS) -o $@ -lcrypto

dbTest: $(DBTEST_OBJS)
	gcc $(LDFLAGS) $(DBTEST_OBJS) -o dbTest -lsqlite3 -lcrypto

clean:
	/bin/rm -f $(TESTS) *.o *.db
	/bin/rm -f *~

tests: $(TESTS)
	./checkRecordCrypto
	./onSaveRights
	./onCheckRightsStatus
	./dbTest

checkRecordCrypto.o: checkRecordCrypto.c ../DrmKernel.c ../../include/DrmKernel.h ../../include/nvDatabase.h  ../../include/nvDatabaseSecureTable.h 
onSaveRights.o: onSaveRights.c ../DrmKernel.c ../../include/DrmKernel.h ../../include/nvDatabase.h  ../../include/nvDatabaseSecureTable.h 
onCheckRightsStatus.o: onCheckRightsStatus.c ../DrmKernel.c ../../include/DrmKernel.h ../../include/nvDatabase.h  ../../include/nvDatabaseSecureTable.h 
dbTest.o: dbTest.c ../../include/DrmKernel.h 
DrmKernel.o: ../DrmKernel.c ../../include/DrmKernel.h ../../include/nvDatabase.h  ../../include/nvDatabaseSecureTable.h 
nvDatabase.o: ../nvDatabase.c ../../include/nvDatabase.h
nvDatabaseSecureTable.o: ../nvDatabaseSecureTable.c ../../include/nvDatabaseSecureTable.h 
