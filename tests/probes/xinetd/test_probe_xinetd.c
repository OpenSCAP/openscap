
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define XINETD_TEST
#include <../../../../src/OVAL/probes/unix/xinetd.c>

#define EMPTYSTR_IF_NULL(val) val ? val : ""

static char *string_array_cstr(char **string_array)
{
	char buf[8192]; // Ought to be enough for anybody
	size_t string_array_pos, bufpos = 0;
	buf[0] = '\0';
	if (string_array != NULL) {
		for (string_array_pos = 0; string_array[string_array_pos] && (sizeof(buf) - bufpos - 1) > 1; ++string_array_pos) {
			bufpos += snprintf(buf + bufpos, sizeof(buf) - bufpos - 1, "%s ", string_array[string_array_pos]);
		}
	}
	return oscap_strdup(buf);
}

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
				EMPTYSTR_IF_NULL(xres->srv[l]->id),
				EMPTYSTR_IF_NULL(xres->srv[l]->type),
				string_array_cstr(xres->srv[l]->flags),
				EMPTYSTR_IF_NULL(xres->srv[l]->socket_type),
				EMPTYSTR_IF_NULL(xres->srv[l]->name),
				EMPTYSTR_IF_NULL(xres->srv[l]->protocol),
				EMPTYSTR_IF_NULL(xres->srv[l]->user),
				EMPTYSTR_IF_NULL(xres->srv[l]->server),
				EMPTYSTR_IF_NULL(xres->srv[l]->server_args),
				string_array_cstr(xres->srv[l]->only_from),
				string_array_cstr(xres->srv[l]->no_access),
				xres->srv[l]->port,
				xres->srv[l]->disable,
				xres->srv[l]->wait,
				xres->srv[l]->def_disabled,
				xres->srv[l]->def_enabled);
		}
	}

	return (0);
}
