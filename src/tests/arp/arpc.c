/**
 * \file arpc.c
 *
 * \date 24.04.09
 * \author sikmir
 */

#include "types.h"
#include "common.h"
#include "arpc.h"
#include "shell.h"
#include "arp.h"
#include "eth.h"

#define COMMAND_NAME "arp"
static char available_keys[] = {
#include "arp_keys.inc"
};

static void show_help() {
	printf(
#include "arp_help.inc"
	);
}

int arp_shell_handler(int argsc, char **argsv) {
	SHELL_KEY keys[MAX_SHELL_KEYS];
	char *key_value;
	int keys_amount;
	unsigned char addr[4];
	unsigned char hwaddr[6];
	void *ifdev = NULL;

	keys_amount = parse_arg(COMMAND_NAME, argsc, argsv, available_keys,
			sizeof(available_keys), keys);

        if (get_key('i', keys, keys_amount, &key_value)) {
                if (NULL == (ifdev = eth_get_ifdev_by_name(key_value))) {
                        ERROR("can't find interface %s\n", key_value);
                        return -1;
                }
        }
	if (get_key('a', keys, keys_amount, &key_value)) {
		if (NULL == ipaddr_scan(key_value, addr)) {
	                ERROR("wrong ip addr format (%s)\n", key_value);
	                show_help();
	                return -1;
	        }
	}
        if (get_key('m', keys, keys_amount, &key_value)) {
                if (NULL == macaddr_scan(key_value, hwaddr)) {
                        ERROR("wrong mac addr format %s\n", key_value);
                        return -1;
                }
        }
        if (get_key('d', keys, keys_amount, &key_value)) {
                arp_delete_entity(ifdev, addr, hwaddr);
                return 0;
        }
        if (get_key('s', keys, keys_amount, &key_value)) {
                arp_add_entity(ifdev, addr, hwaddr);
                return 0;
	}
	if (get_key('h', keys, keys_amount, &key_value)) {
		show_help();
		return 0;
	}
	TRACE("Address\t\tHWtype\tHWaddress\t\tFlags\tIface\n");
	print_arp_cache();
	return 0;
}
