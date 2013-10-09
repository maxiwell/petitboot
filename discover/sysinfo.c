
#include <string.h>

#include <talloc/talloc.h>

#include "discover-server.h"
#include "sysinfo.h"

static struct system_info *sysinfo;
static struct discover_server *server;

const struct system_info *system_info_get(void)
{
	return sysinfo;
}

void system_info_register_interface(unsigned int hwaddr_size, uint8_t *hwaddr,
		const char *name)
{
	struct interface_info *if_info;
	unsigned int i;

	for (i = 0; i < sysinfo->n_interfaces; i++) {
		if_info = sysinfo->interfaces[i];

		if (if_info->hwaddr_size != hwaddr_size)
			continue;

		if (memcmp(if_info->hwaddr, hwaddr, hwaddr_size))
			continue;

		/* update the name and we're done */
		talloc_free(if_info->name);
		if_info->name = talloc_strdup(if_info, name);
		discover_server_notify_system_info(server, sysinfo);
		return;
	}

	if_info = talloc_zero(sysinfo, struct interface_info);
	if_info->hwaddr_size = hwaddr_size;
	if_info->hwaddr = talloc_memdup(if_info, hwaddr, hwaddr_size);
	if_info->name = talloc_strdup(if_info, name);

	sysinfo->n_interfaces++;
	sysinfo->interfaces = talloc_realloc(sysinfo, sysinfo->interfaces,
						struct interface_info *,
						sysinfo->n_interfaces);
	sysinfo->interfaces[sysinfo->n_interfaces - 1] = if_info;

	discover_server_notify_system_info(server, sysinfo);
}

static void system_info_set_identifier(struct system_info *info
		__attribute__((unused)))
{
	/* todo: call helpers to set type & id */
}

void system_info_init(struct discover_server *s)
{
	sysinfo = talloc_zero(server, struct system_info);
	server = s;
	system_info_set_identifier(sysinfo);
}