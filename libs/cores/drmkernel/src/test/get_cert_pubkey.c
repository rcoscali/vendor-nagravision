
#include <string.h>

#include "openssl/bio.h"
#include "openssl/evp.h"
#include "openssl/sha.h"
#include "openssl/rsa.h"
#include "openssl/pem.h"
#include "openssl/err.h"

static const char *mNagraRootCertPem = \
  "-----BEGIN CERTIFICATE-----\n"					   \
  "MIIELTCCAxWgAwIBAgIJAPEWvTHHh7OHMA0GCSqGSIb3DQEBCwUAMIGsMQswCQYD\n"	\
  "VQQGEwJDSDENMAsGA1UECAwEVmF1ZDERMA8GA1UEBwwIQ2hlc2VhdXgxFDASBgNV\n"	\
  "BAoMC05hZ3JhdmlzaW9uMRowGAYDVQQLDBFFbWJlZGRlZCBTZWN1cml0eTEjMCEG\n"	\
  "A1UEAwwaTmFncmEgRHJtIFJvb3QgQ2VydGlmaWNhdGUxJDAiBgkqhkiG9w0BCQEW\n"	\
  "FWRybWNhQG5hZ3JhdmlzaW9uLmNvbTAeFw0xNDA5MTIxNTA5NTlaFw0xNTA3MDkx\n"	\
  "NTA5NTlaMIGsMQswCQYDVQQGEwJDSDENMAsGA1UECAwEVmF1ZDERMA8GA1UEBwwI\n"	\
  "Q2hlc2VhdXgxFDASBgNVBAoMC05hZ3JhdmlzaW9uMRowGAYDVQQLDBFFbWJlZGRl\n"	\
  "ZCBTZWN1cml0eTEjMCEGA1UEAwwaTmFncmEgRHJtIFJvb3QgQ2VydGlmaWNhdGUx\n"	\
  "JDAiBgkqhkiG9w0BCQEWFWRybWNhQG5hZ3JhdmlzaW9uLmNvbTCCASIwDQYJKoZI\n"	\
  "hvcNAQEBBQADggEPADCCAQoCggEBANF/a7VtD/X2Yc/0g5dsuA61GGlUrZOgDXdj\n"	\
  "+Jl6dTm5ukLZfQKjKli+sMuGofVGJQeTQk/GeLXFWSBIOmBxQggkBygaGWor+bEy\n"	\
  "nT8RIb4OvowMIjzC+bCuF92OUMSfu2OeR6WkqjaQZ1JXQfuEiVjAKyQt1zDOfoEY\n"	\
  "cVFkcgIIq63Ui0nSKQJAGscXQSU/1mNeTF8TACyYRPS4T6R9tsbWBbKstaUTVuHI\n"	\
  "jsluYqSKhITNaalPnIl2Gk1RVN2m6jpi5HM9zmxkkfBNquRkQ/+adisen0d5wEgp\n"	\
  "Yy1RNQ5R7iJqvi60zP0Qnm8dxDxaeD4bwDUrG/voMv/pfnOkqscCAwEAAaNQME4w\n"	\
  "HQYDVR0OBBYEFLSRw3MgpsE6W5XavCl8Z+SEZYvVMB8GA1UdIwQYMBaAFLSRw3Mg\n"	\
  "psE6W5XavCl8Z+SEZYvVMAwGA1UdEwQFMAMBAf8wDQYJKoZIhvcNAQELBQADggEB\n"	\
  "AJAhYAtI9dt3E3CXLEIrQzKsvdR4jEQEm4kwHWjTJnyi4Iwegq3lYxMDJStKJ+Uk\n"	\
  "PZQmBrIJSFagy/lxFoo+d4QMgfRDql3vXEpwQzPRUu5V31x6FtxAZK1XNOxk+DRr\n"	\
  "9fp5k9hxKBWeh3Gj0zhSES2I9ou5hMtr3zWU/ZalA4skFU9a8NeLeQbWPYwED3mk\n"	\
  "mMofbER4GXDUwtmmnVH7EzJAIOo4muCaylBuWbrSoEauTFL7ymmW7bnnD/0lGV3/\n"	\
  "p9CEbIwUpcvZTFskHO3DLy9XZnBUyi2GR+ufZToY7WlqQTakLrOYkwk/Jf9IrvHk\n"	\
  "nGrGEEfwdUZhrT/7J1utV3c=\n"						\
  "-----END CERTIFICATE-----\n";

static const char *mDeviceId = "734fd5e7b2efdc5b0146882a0e019006";

int
main(int argc, char ** argv)
{
  BIO *bio_err = (BIO *)NULL;
  BIO *bio = (BIO *)NULL;
  BIO *bmd = (BIO *)NULL;
  BIO *inp = (BIO *)NULL;
  BIO *out = (BIO *)NULL;
  EVP_PKEY *pkey = (EVP_PKEY *)NULL;
  X509 *cert = (X509 *)NULL;
  EVP_MD *md = NULL;
  EVP_MD_CTX *md_ctx;
  EVP_PKEY_CTX *p_ctx;
  char *devicePersoData = NULL;
  int i;
  RSA *key;
  ASN1_PCTX *asn1_ctx;
  unsigned char *buf[100];
  size_t len;

  bio_err = BIO_new(BIO_s_file());
  BIO_set_fp(bio_err, stderr, BIO_NOCLOSE|BIO_FP_TEXT);
  bio = BIO_new(BIO_f_buffer());
  BIO_ctrl(bio, BIO_C_SET_BUFF_READ_DATA, strlen(mNagraRootCertPem)+1, (void *)mNagraRootCertPem);
  out = BIO_new_fp(stdout, BIO_NOCLOSE);
  cert = PEM_read_bio_X509_AUX(bio,NULL,NULL,NULL);
  BIO_free(bio);
  if (cert)
    {
      pkey=X509_get_pubkey(cert);
      if (pkey)
	{
	  asn1_ctx = ASN1_PCTX_new();
	  EVP_PKEY_print_public(out, pkey, 0, asn1_ctx);
	  key = EVP_PKEY_get1_RSA(pkey);
	  if (key) 
	    {
	      fprintf(stdout, "RSA pub key available !!\n");
	      bmd = BIO_new(BIO_f_md());
	      md = EVP_get_digestbyname((char *)"sha256");
	      if (!BIO_set_md(bmd, md))
		{
		  BIO_printf(bio_err, "Error setting digest sha256\n");
		  ERR_print_errors(bio_err);
		  fprintf(stderr, "Error setting digest sha256\n");
		  return 1;
		}
	      bio = BIO_new(BIO_f_buffer());
	      BIO_ctrl(bio, BIO_C_SET_BUFF_READ_DATA, strlen(mDeviceId), (void *)mDeviceId);
	      inp = BIO_push(bmd, bio);
	      if (BIO_read(inp, buf, 100) <= 0)
		{
		  fprintf(stderr, "Error reading device id\n");
		  return 1;
		}
	      BIO_get_md_ctx(inp, &md_ctx);
	      len = 100;
	      if (!EVP_DigestSignFinal(md_ctx, (unsigned char *)buf, &len))
		{
		  fprintf(stderr, "Error signing data !!\n");
		  return 1;
		}
	      for (i=0; i<(int)len; i++)
		BIO_printf(out, "%02x", (unsigned short)buf[i]);
	    }
	  else
	    fprintf(stderr, "NULL key !\n");
	}
      else
	fprintf(stderr, "NULL pkey !\n");
    }
  else
    fprintf(stderr, "NULL cert !\n");

  return 0;
}
