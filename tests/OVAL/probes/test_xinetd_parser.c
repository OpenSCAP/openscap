#define XINETD_TEST
#include <../../../../src/OVAL/probes/unix/xinetd.c>

int main (int argc, char *argv[])
{
	xiconf_t         *xcfg;
	xiconf_service_t *xsrv;

	char *path, *serv, *prot;

	if (argc != 4) {
		fprintf(stderr, "Usage: %s <path> <service> <protocol>\n", argv[0]);
		return (1);
	}

	path = argv[1];
	serv = argv[2];
	prot = argv[3];

	xcfg = xiconf_parse (path, 32);

	if (xcfg == NULL) {
		fprintf(stderr, "Parse error.\n");
		return (2);
	}

	xsrv = xiconf_getservice (xcfg, serv, prot);

	if (xsrv == NULL) {
		fprintf(stderr, "Not found.\n");
		return (3);
	} else {
		fprintf(stdout,
			"xiconf_service_t:\n"
			"         type: %s\n"
			"        flags: %s\n"
			"  socket_type: %s\n"
			"         name: %s\n"
			"     protocol: %s\n"
			"         user: %s\n"
			"       server: %s\n"
			"  server_args: %s\n"
			"    only_from: %s\n"
			"    no_access: %s\n"
			"         port: %d\n"
			"      disable: %d\n"
			"         wait: %d\n"
			" def_disabled: %d\n"
			"  def_enabled: %d\n",
			xsrv->type,
			xsrv->flags,
			xsrv->socket_type,
			xsrv->name,
			xsrv->protocol,
			xsrv->user,
			xsrv->server,
			xsrv->server_args,
			xsrv->only_from,
			xsrv->no_access,
			xsrv->port,
			xsrv->disable,
			xsrv->wait,
			xsrv->def_disabled,
			xsrv->def_enabled);
	}

	return (0);
}
