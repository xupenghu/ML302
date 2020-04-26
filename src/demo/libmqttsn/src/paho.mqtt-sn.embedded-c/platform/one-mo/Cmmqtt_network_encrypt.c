/*******************************************************
copy from MTK
*******************************************************/
/*
#include <stdint.h>
#include <stdio.h>
#include "lwip/opt.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
*/
#include "cmmqtt_platform.h"

#include <stdio.h>
#include <string.h>
//#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include "mbedtls/error.h"
#include "mbedtls/ssl.h"
#include "mbedtls/net.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/pk.h"
#include "mbedtls/debug.h"
#include "mbedtls/platform.h"
//#include "vfs.h"
//#include "vfs_ops.h"


#define	O_RDONLY	0		/* +1 == FREAD */
#define	O_WRONLY	1		/* +1 == FWRITE */
#define	O_RDWR		2		/* +1 == FREAD|FWRITE */

struct	stat 
{
  dev_t		st_dev;
  ino_t		st_ino;
  mode_t	st_mode;
  nlink_t	st_nlink;
  uid_t		st_uid;
  gid_t		st_gid;
  dev_t		st_rdev;
  off_t		st_size;
#if defined(__rtems__)
  struct timespec st_atim;
  struct timespec st_mtim;
  struct timespec st_ctim;
  blksize_t     st_blksize;
  blkcnt_t	st_blocks;
#else
  /* SysV/sco doesn't have the rest... But Solaris, eabi does.  */
#if defined(__svr4__) && !defined(__PPC__) && !defined(__sun__)
  time_t	st_atime;
  time_t	st_mtime;
  time_t	st_ctime;
#else
  time_t	st_atime;
  long		st_spare1;
  time_t	st_mtime;
  long		st_spare2;
  time_t	st_ctime;
  long		st_spare3;
  blksize_t	st_blksize;
  blkcnt_t	st_blocks;
  long	st_spare4[2];
#endif
#endif
};

#define DEBUG_LEVEL 6

extern void cmmqtt_log(const char *fmt,...);

int cmmqtt_real_confirm(int verify_result)
{
#define VERIFY_ITEM(Result, Item, ErrMsg) \
    do { \
        if (((Result) & (Item)) != 0) { \
            printf(ErrMsg); \
        } \
    } while (0)

    cmmqtt_log("certificate verification result: 0x%02x", verify_result);
    VERIFY_ITEM(verify_result, MBEDTLS_X509_BADCERT_EXPIRED, "! fail ! server certificate has expired");
    VERIFY_ITEM(verify_result, MBEDTLS_X509_BADCERT_REVOKED, "! fail ! server certificate has been revoked");
    VERIFY_ITEM(verify_result, MBEDTLS_X509_BADCERT_CN_MISMATCH, "! fail ! CN mismatch");
    VERIFY_ITEM(verify_result, MBEDTLS_X509_BADCERT_NOT_TRUSTED, "! fail ! self-signed or not signed by a trusted CA");
    return 0;
}


/*void cmmqtt_get_CA()
{
	nvdm_status_t status;
    nvdm_modem_data_item_type_t type;
    N1RfAfcCalData data; 
	char buf [1600];
	nvdm_modem_read_normal_data_item("cmmqtt","cmmqtt_clientCA",
									&type, buf,1600);
}*/
uint32_t cmmqtt_avRandom()
{
    return (((uint32_t)rand() << 16) + rand());
}

void cmmqtt_ssl_debug( void *ctx, int level, const char *file, int line, const char *str )
{
    //printf("%s\n", str);
    cmmqtt_log("%s\n", str);
}

int cmmqtt_ssl_random(void *p_rng, unsigned char *output, size_t output_len)
{
    uint32_t rnglen = output_len;
    uint8_t   rngoffset = 0;

    while (rnglen > 0) {
        *(output + rngoffset) = (unsigned char)cmmqtt_avRandom() ;
        rngoffset++;
        rnglen--;
    }
    return 0;
}

int cmmqtt_ssl_parse_crt(mbedtls_x509_crt *crt)
{
    char buf[1024];
    mbedtls_x509_crt *local_crt = crt;
    int i = 0;
    while (local_crt) {
        printf("# %d\r\n", i);
        mbedtls_x509_crt_info(buf, sizeof(buf) - 1, "", local_crt);
        {
            char str[512];
            const char *start, *cur;
            start = buf;
            for (cur = buf; *cur != '\0'; cur++) {
                if (*cur == '\n') {
                    size_t len = cur - start + 1;
                    if (len > 511) {
                        len = 511;
                    }
                    memcpy(str, start, len);
                    str[len] = '\0';
                    start = cur + 1;
                    printf("%s", str);
                }
            }
        }
        printf("crt content:%d!\r\n", strlen(buf));
        local_crt = local_crt->next;
        i++;
    }
    return i;
}
int cmmqtt_ssl_read_all(CMMQTT_Network *n, unsigned char *buffer, int len, int timeout_ms)
{
    size_t readLen = 0;
    int ret = -1;
   
    cmmqtt_log("mqtt_ssl_read_all len=%d", len);
    
    cmmqtt_log("mqtt ssl read all timer=%d ms", timeout_ms);
    mbedtls_ssl_conf_read_timeout(&(n->conf), timeout_ms);
    while (readLen < len) {
        ret = mbedtls_ssl_read(&(n->ssl), (unsigned char *)(buffer + readLen), (len - readLen));
        cmmqtt_log("%s, mbedtls_ssl_read return:%d", __func__, ret);
        if (ret > 0) {
            readLen += ret;
        } else if (ret == 0) {
            cmmqtt_log("mqtt ssl read timeout");
            return -2; //eof
        } else {
            if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {//read already complete(if call mbedtls_ssl_read again, it will return 0(eof))
                cmmqtt_log("MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY");
                return -2;
            }
            return -1; //Connnection error
        }
    }
    cmmqtt_log("mqtt_ssl_read_all readlen=%d", readLen);
    return readLen;
}
int cmmqtt_ssl_write_all(CMMQTT_Network *n, unsigned char *buffer, int len, int timeout_ms)
{
    size_t writtenLen = 0;
    int ret = -1;

    cmmqtt_log("mqtt_ssl_write_all len=%d", len);

    cmmqtt_log("mqtt ssl read write timer=%d ms", timeout_ms);
    while (writtenLen < len) {
        ret = mbedtls_ssl_write(&(n->ssl), (unsigned char *)(buffer + writtenLen), (len - writtenLen));
        if (ret > 0) {
            writtenLen += ret;
            continue;
        } else if (ret == 0) {
            cmmqtt_log("mqtt ssl write timeout");
            return writtenLen;
        } else {
            cmmqtt_log("mqtt ssl write fail");
            return -1; //Connnection error
        }
    }
    cmmqtt_log("mqtt ssl write len=%d", writtenLen);
    return writtenLen;
}

void cmmqtt_ssl_disconnect(CMMQTT_Network *n)
{
    mbedtls_ssl_close_notify(&(n->ssl));
    mbedtls_net_free(&(n->fd));
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    mbedtls_x509_crt_free( &(n->cacertl));
    if ((n->pkey).pk_info != NULL) {
        cmmqtt_log("mqtt need free client crt&key");
        mbedtls_x509_crt_free( &(n->clicert));
        mbedtls_pk_free( &(n->pkey) );
    }
#endif
    mbedtls_ssl_free( &(n->ssl));
    mbedtls_ssl_config_free(&(n->conf));
    cmmqtt_log( " mqtt_ssl_disconnect\n" );
}


int cmmqtt_ssl_init(mbedtls_ssl_context *ssl,
                         mbedtls_net_context *tcp_fd,
                         mbedtls_ssl_config *conf,
                         mbedtls_x509_crt *crt509_ca, const char *ca_crt, size_t ca_len,
                         mbedtls_x509_crt *crt509_cli, const char *cli_crt, size_t cli_len,
                         mbedtls_pk_context *pk_cli, const char *cli_key, size_t key_len,  const char *cli_pwd, size_t pwd_len
                        )
{
    int ret = -1;
    //verify_source_t *verify_source = &custom_config->verify_source;

    /*
     * 0. Initialize the RNG and the session data
     */
#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold(DEBUG_LEVEL);
#endif
    mbedtls_net_init( tcp_fd );
    mbedtls_ssl_init( ssl );
    mbedtls_ssl_config_init( conf );
    mbedtls_x509_crt_init(crt509_ca);

    /*verify_source->trusted_ca_crt==NULL
     * 0. Initialize certificates
     */

    cmmqtt_log( "  . Loading the CA root certificate ..." );
    if (NULL != ca_crt) {
        if (0 != (ret = mbedtls_x509_crt_parse(crt509_ca, (const unsigned char *)ca_crt, ca_len))) {
            cmmqtt_log(" failed ! x509parse_crt returned -0x%04x", -ret);
            return ret;
        }
    }
    cmmqtt_ssl_parse_crt(crt509_ca);
    cmmqtt_log( " ok (%d skipped)", ret );


    /* Setup Client Cert/Key */
#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if defined(MBEDTLS_CERTS_C)
    mbedtls_x509_crt_init(crt509_cli);
    mbedtls_pk_init( pk_cli );
#endif
    if ( cli_crt != NULL && cli_key != NULL) {
#if defined(MBEDTLS_CERTS_C)
       	cmmqtt_log("start prepare client cert .\n");
        ret = mbedtls_x509_crt_parse( crt509_cli, (const unsigned char *) cli_crt, cli_len );
#else
        {
            ret = 1;
           cmmqtt_log("MBEDTLS_CERTS_C not defined.");
        }
#endif
        if ( ret != 0 ) {
            cmmqtt_log( " failed!  mbedtls_x509_crt_parse returned -0x%x\n\n", -ret );
            return ret;
        }

#if defined(MBEDTLS_CERTS_C)
        cmmqtt_log("start mbedtls_pk_parse_key[%s]", cli_pwd);
        ret = mbedtls_pk_parse_key( pk_cli,
                                    (const unsigned char *) cli_key, key_len,
                                    (const unsigned char *) cli_pwd, pwd_len);
#else
        {
            ret = 1;
            cmmqtt_log("MBEDTLS_CERTS_C not defined.");
        }
#endif

        if ( ret != 0 ) {
            cmmqtt_log( " failed\n  !  mbedtls_pk_parse_key returned -0x%x\n\n", -ret);
            return ret;
        }
    }
#endif /* MBEDTLS_X509_CRT_PARSE_C */
	 cmmqtt_log("ssl_init_ok");
    return 0;
}

int cmmqtt_connect_network_tls(CMMQTT_Network *n, const char *addr, const char *port,
                      const char *ca_crt, size_t ca_crt_len,
                      const char *client_crt,	size_t client_crt_len,
                      const char *client_key,	size_t client_key_len,
                      const char *client_pwd, size_t client_pwd_len)
{
    int ret = -1;
    /*
     * 0. Init
     */
    if (0 != (ret = cmmqtt_ssl_init(&(n->ssl), &(n->fd), &(n->conf),
                                         &(n->cacertl), ca_crt, ca_crt_len,
                                         &(n->clicert), client_crt, client_crt_len,
                                         &(n->pkey), client_key, client_key_len, client_pwd, client_pwd_len)))
    {
        cmmqtt_log( " failed ! ssl_client_init returned -0x%04x", -ret );
        return ret;
    }
	   /*
     * 1. Start the connection
     */
    cmmqtt_log("  . Connecting to tcp/%s/%s...", addr, port);
    if (0 != (ret = mbedtls_net_connect(&(n->fd), addr, port, MBEDTLS_NET_PROTO_TCP))) {
        cmmqtt_log(" failed ! net_connect returned -0x%04x", -ret);
        return ret;
    }
    cmmqtt_log( "connection ok\n" );
	/*
     * 2. Setup stuff
     */
    cmmqtt_log( "  . Setting up the SSL/TLS structure..." );
    if ( ( ret = mbedtls_ssl_config_defaults( &(n->conf),
                 MBEDTLS_SSL_IS_CLIENT,
                 MBEDTLS_SSL_TRANSPORT_STREAM,
                 MBEDTLS_SSL_PRESET_DEFAULT ) ) != 0 ) {
        cmmqtt_log( " failed! mbedtls_ssl_config_defaults returned %d", ret );
        return ret;
    }
    cmmqtt_log( " setup ok" );

	
	 /* OPTIONAL is not optimal for security,
			 * but makes interop easier in this simplified example */
		if (ca_crt != NULL) {
			mbedtls_ssl_conf_authmode( &(n->conf),  MBEDTLS_SSL_VERIFY_REQUIRED);
		} else {
			mbedtls_ssl_conf_authmode( &(n->conf), MBEDTLS_SSL_VERIFY_NONE);
		}
	
	#if defined(MBEDTLS_X509_CRT_PARSE_C)
		mbedtls_ssl_conf_ca_chain( &(n->conf), &(n->cacertl), NULL);
	
		if ( ( ret = mbedtls_ssl_conf_own_cert( &(n->conf), &(n->clicert), &(n->pkey) ) ) != 0 ) {
			cmmqtt_log( " failed\n  ! mbedtls_ssl_conf_own_cert returned %d\n\n", ret );
			return ret;
		}
	#endif
		mbedtls_ssl_conf_rng( &(n->conf), cmmqtt_ssl_random, NULL );
		mbedtls_ssl_conf_dbg( &(n->conf), cmmqtt_ssl_debug, NULL );
	
	
		if ( ( ret = mbedtls_ssl_setup(&(n->ssl), &(n->conf)) ) != 0 ) {
			cmmqtt_log( " failed! mbedtls_ssl_setup returned %d", ret );
			return ret;
		}
		mbedtls_ssl_set_hostname(&(n->ssl), addr);
		mbedtls_ssl_set_bio( &(n->ssl), &(n->fd), mbedtls_net_send, mbedtls_net_recv,mbedtls_net_recv_timeout);


	 /*
      * 4. Handshake
      */
    cmmqtt_log("  . Performing the SSL/TLS handshake...");
	n->conf.read_timeout = 40*1000;
    while ((ret = mbedtls_ssl_handshake(&(n->ssl))) != 0) {
        if ((ret != MBEDTLS_ERR_SSL_WANT_READ) && (ret != MBEDTLS_ERR_SSL_WANT_WRITE)) {
            cmmqtt_log( " failed  ! mbedtls_ssl_handshake returned -0x%04x", -ret);
            return ret;
        }
    }
    cmmqtt_log( "handshake ok" );

	/*
     * 5. Verify the server certificate
     */
    cmmqtt_log("  . Verifying peer X.509 certificate..");
    if (0 != (ret = cmmqtt_real_confirm(mbedtls_ssl_get_verify_result(&(n->ssl))))) {
        cmmqtt_log(" failed  ! verify result not confirmed.");
        return ret;
    }
	cmmqtt_log(" verify CA ok");

	n->mqttread = cmmqtt_ssl_read_all;
    n->mqttwrite = cmmqtt_ssl_write_all;
    n->disconnect = cmmqtt_ssl_disconnect;
	n->disconnected_cb = cmmqtt_disconnect_cb;
	return 0;
}

void cmmqtt_get_encryptinfo(char **buf,const char *item,const char *item_name)
{
	if(buf != NULL)
	{
		 int fd = vfs_open(item_name, O_RDONLY);
		 if(fd < 0)
		 {
			cmmqtt_free(*buf);
			*buf = NULL;
			return;
		 }
		 int len = 0;
		 uint8_t buffer[256];
		 struct stat st = {};
         vfs_fstat(fd, &st);
         int file_size = st.st_size;
         if (file_size <= 1024*4)
        {
            while (file_size > 0)
            {
                int read_size = (file_size > 256) ? 256 : file_size;
                int read_len = vfs_read(fd, buffer, read_size);
                if (read_len <= 0)
                    break;
				len = len + sprintf((*buf+len), "%s", buffer);
				memset(buffer,0,256);
                file_size -= read_len;
            }
        }
		if(len != file_size )
		{
			cmmqtt_free(*buf);
			*buf = NULL;
		}
	
	}

}












