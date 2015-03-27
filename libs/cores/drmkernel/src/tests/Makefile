
all: checkRecordCrypto onSaveRights onCheckRightsStatus

checkRecordCrypto: checkRecordCrypto.c
	gcc -g -w -DDRM_HOST_KERNEL_TEST=1 -std=c99 -I../../include -I../../../../../../../frameworks/native/include -I../../../../../../../system/core/include $< -o $@ -lcrypto

onSaveRights: onSaveRights.c
	gcc -g -w -DDRM_HOST_KERNEL_TEST=1 -std=c99 -I../../include -I../../../../../../../frameworks/native/include -I../../../../../../../system/core/include $< -o $@ -lcrypto

onCheckRightsStatus: onCheckRightsStatus.c
	gcc -g -w -DDRM_HOST_KERNEL_TEST=1 -std=c99 -I../../include -I../../../../../../../frameworks/native/include -I../../../../../../../system/core/include $< -o $@ -lcrypto

clean:
	rm -f checkRecordCrypto onSaveRights onCheckRightsStatus

tests: checkRecordCrypto onSaveRights onCheckRightsStatus
	./checkRecordCrypto
	./onSaveRights
	./onCheckRightsStatus
