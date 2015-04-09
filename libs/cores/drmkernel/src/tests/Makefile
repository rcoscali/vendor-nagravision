
all: checkRecordCrypto onSaveRights onCheckRightsStatus dbTest

checkRecordCrypto: checkRecordCrypto.c ../DrmKernel.c
	gcc -g -w -DDRM_HOST_KERNEL_TEST=1 -std=c99 -I../../include -I../../../../../../../frameworks/native/include -I../../../../../../../system/core/include $< -o $@ -lcrypto

onSaveRights: onSaveRights.c ../DrmKernel.c
	gcc -g -w -DDRM_HOST_KERNEL_TEST=1 -std=c99 -I../../include -I../../../../../../../frameworks/native/include -I../../../../../../../system/core/include $< -o $@ -lcrypto

onCheckRightsStatus: onCheckRightsStatus.c ../DrmKernel.c
	gcc -g -w -DDRM_HOST_KERNEL_TEST=1 -std=c99 -I../../include -I../../../../../../../frameworks/native/include -I../../../../../../../system/core/include $< -o $@ -lcrypto

dbTest: dbTest.c ../nvDatabase.c ../nvDatabaseSecureTable.c ../DrmKernel.c
	gcc -g -w -DDRM_HOST_KERNEL_TEST=1 -std=c99 -I../../include -I../../../../../../../frameworks/native/include -I../../../../../../../system/core/include -c ../DrmKernel.c -o DrmKernel.o 
	gcc -g -w -DDRM_HOST_KERNEL_TEST=1 -std=c99 -I../../include -I../../../../../../../frameworks/native/include -I../../../../../../../system/core/include -c ../nvDatabase.c -o nvDatabase.o 
	gcc -g -w -DDRM_HOST_KERNEL_TEST=1 -std=c99 -I../../include -I../../../../../../../frameworks/native/include -I../../../../../../../system/core/include -c ../nvDatabaseSecureTable.c -o nvDatabaseSecureTable.o 
	gcc -g -w -DDRM_HOST_KERNEL_TEST=1 -std=c99 -I../../include -I../../../../../../../frameworks/native/include -I../../../../../../../system/core/include dbTest.c DrmKernel.o nvDatabase.o nvDatabaseSecureTable.o -o dbTest -lsqlite3 -lcrypto

clean:
	rm -f checkRecordCrypto onSaveRights onCheckRightsStatus dbTest *.o *.db

tests: checkRecordCrypto onSaveRights onCheckRightsStatus
	./checkRecordCrypto
	./onSaveRights
	./onCheckRightsStatus
