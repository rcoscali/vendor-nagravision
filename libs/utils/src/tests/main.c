#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "uuid.h"

#define NIL_UUID      { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#define NOT_NIL_UUID  { 0x9a, 0x04, 0xf0, 0x79, 0x98, 0x40, 0x42, 0x86, \
                        0xab, 0x92, 0xe6, 0x5b, 0xe0, 0x88, 0x5f, 0x95 }
#define NOT_NIL_UUID2 { 0xab, 0x92, 0xe6, 0x5b, 0xe0, 0x88, 0x5f, 0x95, \
                        0x9a, 0x04, 0xf0, 0x79, 0x98, 0x40, 0x42, 0x86 }

#define NIL_UUID_STR      "00000000-0000-0000-0000-000000000000"
#define NOT_NIL_UUID_STR  "9a04f079-9840-4286-ab92-e65be0885f95"
#define NOT_NIL_UUID_STR2 "a65b92eb-e808-f955-94a0-f08407994286"

static int n_tests = 0;
static int n_tests_passed = 0;
static int n_tests_failed = 0;

int
main(int argc, char **argv)
{
  uint8_t anuuid[16];
  uint8_t anuuid2[16];
  
  uuid_str2bin(NOT_NIL_UUID_STR, anuuid);
  fprintf(stderr, "%s cmp %02x%02x%02x%02x-%02x%02x-%02x%02x-"
          "%02x%02x-%02x%02x%02x%02x%02x%02x\n", NOT_NIL_UUID_STR,
          anuuid[0], anuuid[1], anuuid[2], anuuid[3], 
          anuuid[4], anuuid[5], anuuid[6], anuuid[7], 
          anuuid[8], anuuid[9], anuuid[10], anuuid[11], 
          anuuid[12], anuuid[13], anuuid[14], anuuid[15]);
  uuid_str2bin(NOT_NIL_UUID_STR2, anuuid2);
  fprintf(stderr, "%s cmp %02x%02x%02x%02x-%02x%02x-%02x%02x-"
          "%02x%02x-%02x%02x%02x%02x%02x%02x\n", NOT_NIL_UUID_STR2,
          anuuid2[0], anuuid2[1], anuuid2[2], anuuid2[3], 
          anuuid2[4], anuuid2[5], anuuid2[6], anuuid2[7], 
          anuuid2[8], anuuid2[9], anuuid2[10], anuuid2[11], 
          anuuid2[12], anuuid2[13], anuuid2[14], anuuid2[15]);
  

  /*
   * uuid_is_nil with nil UUID
   */
  uint8_t nilUUID[16] = NIL_UUID;
  char nilUUIDstr[40];
  n_tests++;
  if (uuid_bin2str(nilUUID, nilUUIDstr, 40) < 0)
    {
      fprintf(stderr, "Failure\n");
      goto skip1;
    }
  char isnil = uuid_is_nil(nilUUID);
  if (isnil) 
    n_tests_passed++;
  else
    n_tests_failed++;
  printf("%s is nil? %s <%s>\n", 
	 nilUUIDstr, isnil ? "YES" : "NO", isnil ? "passed" : "failed" );

 skip1:
  {
    /*
     * uuid_is_nil with not nil UUID
     */
    uint8_t notNilUUID[16] = NOT_NIL_UUID;
    char notNilUUIDstr[40];
    n_tests++;
    if (uuid_bin2str(notNilUUID, notNilUUIDstr, 40) < 0)
      {
	fprintf(stderr, "Failure\n");
	goto skip2;
      }
    char isnil = uuid_is_nil(notNilUUID);
    if (!isnil) 
      n_tests_passed++;
    else
      n_tests_failed++;
    printf("%s is nil? %s <%s>\n", 
	   notNilUUIDstr, isnil ? "YES" : "NO", isnil ? "failed" : "passed" );
  }

 skip2:
  {
    /*
     * uuid_cmp with one nil UUID
     */
    uint8_t nilUUID[16] = NIL_UUID;
    char nilUUIDstr[40];
    uint8_t notNilUUID[16] = NOT_NIL_UUID;
    char notNilUUIDstr[40];
    n_tests++;
    if (uuid_bin2str(nilUUID, nilUUIDstr, 40) < 0)
      {
	fprintf(stderr, "Failure\n");
	goto skip3;
      }
    n_tests++;
    if (uuid_bin2str(notNilUUID, notNilUUIDstr, 40) < 0)
      {
	fprintf(stderr, "Failure\n");
	goto skip3;
      }
    int8_t cmpval1 = uuid_cmp(notNilUUID, nilUUID);
    int8_t cmpval2 = uuid_cmp(nilUUID, notNilUUID);
    if (cmpval1 + cmpval2 == 0 && cmpval1 != 0)
      n_tests_passed+=2;
    else
      n_tests_failed+=2;
    printf("%s cmp %s = %d <%s>\n", 
	   notNilUUIDstr, nilUUIDstr, cmpval1, cmpval1 ? "passed" : "failed" );
    printf("%s cmp %s = %d <%s>\n", 
	   nilUUIDstr, notNilUUIDstr, cmpval2, cmpval2 ? "passed" : "failed" );
    
  }

 skip3:
  {
    /*
     * uuid_cmp with both nil UUID
     */
    uint8_t nilUUID1[16] = NIL_UUID;
    char nilUUID1str[40];
    uint8_t nilUUID2[16] = NIL_UUID;
    char nilUUID2str[40];
    n_tests++;
    if (uuid_bin2str(nilUUID1, nilUUID1str, 40) < 0)
      {
	fprintf(stderr, "Failure\n");
	goto skip4;
      }
    n_tests++;
    if (uuid_bin2str(nilUUID2, nilUUID2str, 40) < 0)
      {
	fprintf(stderr, "Failure\n");
	goto skip4;
      }
    int8_t cmpval1 = uuid_cmp(nilUUID1, nilUUID2);
    int8_t cmpval2 = uuid_cmp(nilUUID2, nilUUID1);
    if (cmpval1 == cmpval2 && cmpval1 == 0)
      n_tests_passed+=2;
    else
      n_tests_failed+=2;
    printf("%s cmp %s = %d <%s>\n", 
	   nilUUID1str, nilUUID2str, cmpval1, cmpval1 ? "failed" : "passed" );
    printf("%s cmp %s = %d <%s>\n", 
	   nilUUID2str, nilUUID1str, cmpval2, cmpval2 ? "failed" : "passed" );
  }

 skip4:
  {
    /*
     * uuid_cmp with both not nil UUID
     */
    uint8_t notNilUUID1[16] = NOT_NIL_UUID;
    char notNilUUID1str[40];
    uint8_t notNilUUID2[16] = NOT_NIL_UUID;
    char notNilUUID2str[40];
    n_tests++;
    if (uuid_bin2str(notNilUUID1, notNilUUID1str, 40) < 0)
      {
	fprintf(stderr, "Failure\n");
	goto skip5;
      }
    n_tests++;
    if (uuid_bin2str(notNilUUID2, notNilUUID2str, 40) < 0)
      {
	fprintf(stderr, "Failure\n");
	goto skip5;
      }
    int8_t cmpval1 = uuid_cmp(notNilUUID1, notNilUUID2);
    int8_t cmpval2 = uuid_cmp(notNilUUID2, notNilUUID1);
    if (cmpval1 == cmpval2 && cmpval1 == 0)
      n_tests_passed+=2;
    else
      n_tests_failed+=2;
    printf("%s cmp %s = %d <%s>\n", 
	   notNilUUID1str, notNilUUID2str, cmpval1, cmpval1 ? "failed" : "passed" );
    printf("%s cmp %s = %d <%s>\n", 
	   notNilUUID2str, notNilUUID1str, cmpval2, cmpval2 ? "failed" : "passed" );
    
  }

 skip5:
  {
    /*
     * uuid_cmp with both not nil UUID
     */
    uint8_t notNilUUID1[16] = NOT_NIL_UUID;
    char notNilUUID1str[40];
    uint8_t notNilUUID2[16] = NOT_NIL_UUID2;
    char notNilUUID2str[40];
    n_tests++;
    if (uuid_bin2str(notNilUUID1, notNilUUID1str, 40) < 0)
      {
	fprintf(stderr, "Failure\n");
	goto skip6;
      }
    n_tests++;
    if (uuid_bin2str(notNilUUID2, notNilUUID2str, 40) < 0)
      {
	fprintf(stderr, "Failure\n");
	goto skip6;
      }
    int8_t cmpval1 = uuid_cmp(notNilUUID1, notNilUUID2);
    int8_t cmpval2 = uuid_cmp(notNilUUID2, notNilUUID1);
    if (cmpval1 + cmpval2 == 0 && cmpval1 != 0)
      n_tests_passed+=2;
    else
      n_tests_failed+=2;
    printf("%s cmp %s = %d <%s>\n", 
	   notNilUUID1str, notNilUUID2str, cmpval1, cmpval1 ? "passed" : "failed" );
    printf("%s cmp %s = %d <%s>\n", 
	   notNilUUID2str, notNilUUID1str, cmpval2, cmpval2 ? "passed" : "failed" );
    
  }

 skip6:
  {
    /*
     * uuid_cmp with one nil UUID
     */
    uint8_t nilUUID[16];
    char nilUUIDstr[40] = NIL_UUID_STR;
    uint8_t notNilUUID[16];
    char notNilUUIDstr[40] = NOT_NIL_UUID_STR;
    n_tests++;
    if (uuid_str2bin(nilUUIDstr, nilUUID) < 0)
      {
	fprintf(stderr, "Failure\n");
	goto skip7;
      }
    n_tests++;
    if (uuid_str2bin(notNilUUIDstr, notNilUUID) < 0)
      {
	fprintf(stderr, "Failure\n");
	goto skip7;
      }
    int8_t cmpval1 = uuid_cmp(notNilUUID, nilUUID);
    int8_t cmpval2 = uuid_cmp(nilUUID, notNilUUID);
    if (cmpval1 + cmpval2 == 0 && cmpval1 != 0)
      n_tests_passed+=2;
    else
      n_tests_failed+=2;
    printf("%s cmp %s = %d <%s>\n", 
	   notNilUUIDstr, nilUUIDstr, cmpval1, cmpval1 ? "passed" : "failed" );
    printf("%s cmp %s = %d <%s>\n", 
	   nilUUIDstr, notNilUUIDstr, cmpval2, cmpval2 ? "passed" : "failed" );
    
  }

 skip7:
  {
    /*
     * uuid_cmp with both nil UUID
     */
    uint8_t nilUUID1[16];
    char nilUUID1str[40] = NIL_UUID_STR;
    uint8_t nilUUID2[16];
    char nilUUID2str[40] = NIL_UUID_STR;
    n_tests++;
    if (uuid_str2bin(nilUUID1str, nilUUID1) < 0)
      {
	fprintf(stderr, "Failure\n");
	goto skip8;
      }
    n_tests++;
    if (uuid_str2bin(nilUUID2str, nilUUID2) < 0)
      {
	fprintf(stderr, "Failure\n");
	goto skip8;
      }
    int8_t cmpval1 = uuid_cmp(nilUUID1, nilUUID2);
    int8_t cmpval2 = uuid_cmp(nilUUID2, nilUUID1);
    if (cmpval1 == cmpval2 && cmpval1 == 0)
      n_tests_passed+=2;
    else
      n_tests_failed+=2;
    printf("%s cmp %s = %d <%s>\n", 
	   nilUUID1str, nilUUID2str, cmpval1, cmpval1 ? "failed" : "passed" );
    printf("%s cmp %s = %d <%s>\n", 
	   nilUUID2str, nilUUID1str, cmpval2, cmpval2 ? "failed" : "passed" );
  }

 skip8:
  {
    /*
     * uuid_cmp with both not nil UUID
     */
    uint8_t notNilUUID1[16];
    char notNilUUID1str[40] = NOT_NIL_UUID_STR;
    uint8_t notNilUUID2[16];
    char notNilUUID2str[40] = NOT_NIL_UUID_STR;
    n_tests++;
    if (uuid_str2bin(notNilUUID1str, notNilUUID1) < 0)
      {
	fprintf(stderr, "Failure\n");
	goto skip9;
      }
    n_tests++;
    if (uuid_str2bin(notNilUUID2str, notNilUUID2) < 0)
      {
	fprintf(stderr, "Failure\n");
	goto skip9;
      }
    int8_t cmpval1 = uuid_cmp(notNilUUID1, notNilUUID2);
    int8_t cmpval2 = uuid_cmp(notNilUUID2, notNilUUID1);
    if (cmpval1 == cmpval2 && cmpval1 == 0)
      n_tests_passed+=2;
    else
      n_tests_failed+=2;
    printf("%s cmp %s = %d <%s>\n", 
	   notNilUUID1str, notNilUUID2str, cmpval1, cmpval1 ? "failed" : "passed" );
    printf("%s cmp %s = %d <%s>\n", 
	   notNilUUID2str, notNilUUID1str, cmpval2, cmpval2 ? "failed" : "passed" );
    
  }

 skip9:
  {
    /*
     * uuid_cmp with both not nil UUID
     */
    uint8_t notNilUUID1[16];
    char notNilUUID1str[40] = NOT_NIL_UUID_STR;
    uint8_t notNilUUID2[16];
    char notNilUUID2str[40] = NOT_NIL_UUID_STR2;
    n_tests++;
    if (uuid_str2bin(notNilUUID1str, notNilUUID1) < 0)
      {
	fprintf(stderr, "Failure\n");
	goto skip10;
      }
    n_tests++;
    if (uuid_str2bin(notNilUUID2str, notNilUUID2) < 0)
      {
	fprintf(stderr, "Failure\n");
	goto skip10;
      }
    int8_t cmpval1 = uuid_cmp(notNilUUID1, notNilUUID2);
    int8_t cmpval2 = uuid_cmp(notNilUUID2, notNilUUID1);
    if (cmpval1 + cmpval2 == 0 && cmpval1 != 0)
      n_tests_passed+=2;
    else
      n_tests_failed+=2;
    printf("%s cmp %s = %d <%s>\n", 
	   notNilUUID1str, notNilUUID2str, cmpval1, cmpval1 ? "passed" : "failed" );
    printf("%s cmp %s = %d <%s>\n", 
	   notNilUUID2str, notNilUUID1str, cmpval2, cmpval2 ? "passed" : "failed" );
    
  }

 skip10:
 summary:
  {
    fprintf(stderr, 
	    "\n\n********************************************************\n"
	    "Number of tests: %d / passed: %d / failed: %d / skipped: %d\n", 
	    n_tests, n_tests_passed, n_tests_failed, 
	    n_tests - (n_tests_passed+n_tests_failed));
  }

  return 0;
}
