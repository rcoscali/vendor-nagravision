
ifeq ($(ANDROID_BUILD_TOP),)
all:
	$(shell echo "echo Build Env not setup !"; return 1;)
endif

JAVA_CMD := java -Xms256m -Xss64m
SIGNAPK_JAR := $(ANDROID_BUILD_TOP)/prebuilts/sdk/tools/lib/signapk.jar

SECURITY_DIR := $(ANDROID_BUILD_TOP)/build/target/product/security

PLATFORM_KEY := $(SECURITY_DIR)/platform.pk8
SHARED_KEY := $(SECURITY_DIR)/shared.pk8
MEDIA_KEY := $(SECURITY_DIR)/media.pk8
TEST_KEY := $(SECURITY_DIR)/testkey.pk8

PLATFORM_CERT := $(SECURITY_DIR)/platform.x509.pem
SHARED_CERT := $(SECURITY_DIR)/shared.x509.pem
MEDIA_CERT := $(SECURITY_DIR)/media.x509.pem
TEST_CERT := $(SECURITY_DIR)/testkey.x509.pem

APK-U := AspireDemoPlayer-unsigned.apk
APK-WS := AspireDemoPlayer.apk

.PHONY: all clean

all: $(APK-WS)

$(APK-WS): $(APK-U)
	$(JAVA_CMD) -jar $(SIGNAPK_JAR) -w $(TEST_CERT) $(TEST_KEY) $< $@

clean:
	rm -f $(APK-WS)
