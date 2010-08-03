#define XINETD_TEST
#include <../../../../src/OVAL/probes/unix/xinetd.c>

int main (int argc, char *argv[])
{
	xiconf_t         *xcfg;
	xiconf_strans_t  *xres;

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

	xres = xiconf_getservice (xcfg, serv, prot);

	if (xres == NULL) {
		fprintf(stderr, "Not found.\n");
		return (3);
	} else {
		register unsigned int l;

		for (l = 0; l < xres->cnt; ++l) {
			fprintf(stdout,
				"xiconf_service_t(%s):\n"
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
				xres->srv[l]->id,
				xres->srv[l]->type,
				xres->srv[l]->flags,
				xres->srv[l]->socket_type,
				xres->srv[l]->name,
				xres->srv[l]->protocol,
				xres->srv[l]->user,
				xres->srv[l]->server,
				xres->srv[l]->server_args,
				xres->srv[l]->only_from,
				xres->srv[l]->no_access,
				xres->srv[l]->port,
				xres->srv[l]->disable,
				xres->srv[l]->wait,
				xres->srv[l]->def_disabled,
				xres->srv[l]->def_enabled);
		}
	}

	return (0);
}
