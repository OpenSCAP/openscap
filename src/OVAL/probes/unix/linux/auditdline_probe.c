// TODO License SOPRA STERIA

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// System libraries
#include <libaudit.h>

// OpenSCAP libraries
#include <probe/probe.h>
#include <debug_priv.h>
#include <sexp-manip.h>
#include <list.h>
#include <oscap_string.h>
#include "../OVAL/probes/probe/entcmp.h"

// probe include
#include "auditdline_probe.h"

/* Global vars */
extern char key[AUDIT_MAX_KEY_LEN + 1];
int list_requested, interpret;
const char key_sep[2] = {AUDIT_KEY_SEPARATOR, 0};
extern int _audit_elf;

struct audit_line *audit_line_new()
{
	struct audit_line *audit_line = malloc(sizeof(struct audit_line));
	audit_line->audit_line = oscap_string_new();
	audit_line->filter_key = oscap_stringlist_new();
	return audit_line;
}

void auditd_line_free(struct audit_line *audit_line)
{
	oscap_string_free(audit_line->audit_line);
	oscap_stringlist_free(audit_line->filter_key);
	free(audit_line);
}

void audit_line_push(struct audit_line *audit_line, const char *audit_line_part)
{
	oscap_string_append_string(audit_line->audit_line, audit_line_part);
}

bool audit_line_add_filter_key(struct audit_line *audit_line, const char *filter_key)
{
	return oscap_stringlist_add_string(audit_line->filter_key, filter_key);
}

// The following code is a modified version of auditctl's code

/*
 * This function detects if we have a watch. A watch is detected when we
 * have syscall == all and a perm field.
 */
static int is_watch(const struct audit_rule_data *r)
{
	unsigned int i, perm = 0, all = 1;

	for (i = 0; i < r->field_count; i++)
	{
		int field = r->fields[i] & ~AUDIT_OPERATORS;
		if (field == AUDIT_PERM)
			perm = 1;
		// Watches can have only 4 field types
		if (field != AUDIT_PERM && field != AUDIT_FILTERKEY &&
			field != AUDIT_DIR && field != AUDIT_WATCH)
			return 0;
	}

	if (((r->flags & AUDIT_FILTER_MASK) != AUDIT_FILTER_USER) &&
		((r->flags & AUDIT_FILTER_MASK) != AUDIT_FILTER_TASK) &&
		((r->flags & AUDIT_FILTER_MASK) != AUDIT_FILTER_EXCLUDE) &&
		((r->flags & AUDIT_FILTER_MASK) != AUDIT_FILTER_FS))
	{
		for (i = 0; i < (AUDIT_BITMASK_SIZE - 1); i++)
		{
			if (r->mask[i] != (uint32_t)~0)
			{
				all = 0;
				break;
			}
		}
	}
	if (perm && all)
		return 1;
	return 0;
}

static int print_syscall(const struct audit_rule_data *r, unsigned int *sc, struct audit_line *audit_line)
{
	int count = 0;
	int all = 1;
	unsigned int i;
	int machine = audit_detect_machine();
	int syscall_buf_len = 0;

	/* Rules on the following filters do not take a syscall */
	if (((r->flags & AUDIT_FILTER_MASK) == AUDIT_FILTER_USER) ||
		((r->flags & AUDIT_FILTER_MASK) == AUDIT_FILTER_TASK) ||
		((r->flags & AUDIT_FILTER_MASK) == AUDIT_FILTER_EXCLUDE) ||
		((r->flags & AUDIT_FILTER_MASK) == AUDIT_FILTER_FS))
		return 0;

	/* See if its all or specific syscalls */
	for (i = 0; i < (AUDIT_BITMASK_SIZE - 1); i++)
	{
		if (r->mask[i] != (uint32_t)~0)
		{
			all = 0;
			break;
		}
	}

	if (all)
	{
		syscall_buf_len = snprintf(NULL, 0, " -S all");
		char *syscall_buf = malloc(syscall_buf_len + 1);
		snprintf(syscall_buf, syscall_buf_len + 1, " -S all");
		audit_line_push(audit_line, syscall_buf);
		free(syscall_buf);
		count = i;
	}
	else
		for (i = 0; i < AUDIT_BITMASK_SIZE * 32; i++)
		{
			int word = AUDIT_WORD(i);
			int bit = AUDIT_BIT(i);
			if (r->mask[word] & bit)
			{
				const char *ptr;
				if (_audit_elf)
					machine = audit_elf_to_machine(_audit_elf);
				if (machine < 0)
					ptr = NULL;
				else
					ptr = audit_syscall_to_name(i, machine);
				if (!count)
				{
					syscall_buf_len = snprintf(NULL, 0, " -S ");
					char *syscall_buf = malloc(syscall_buf_len + 1);
					snprintf(syscall_buf, syscall_buf_len + 1, " -S ");
					audit_line_push(audit_line, syscall_buf);
					free(syscall_buf);
				}
				if (ptr)
				{
					syscall_buf_len = snprintf(NULL, 0, "%s%s", !count ? "" : ",", ptr);
					char *syscall_buf = malloc(syscall_buf_len + 1);
					snprintf(syscall_buf, syscall_buf_len + 1, "%s%s", !count ? "" : ",", ptr);
					audit_line_push(audit_line, syscall_buf);
					free(syscall_buf);
				}
				else
				{
					syscall_buf_len = snprintf(NULL, 0, "%s%u", !count ? "" : ",", i);
					char *syscall_buf = malloc(syscall_buf_len + 1);
					snprintf(syscall_buf, syscall_buf_len + 1, "%s%u", !count ? "" : ",", i);
					audit_line_push(audit_line, syscall_buf);
					free(syscall_buf);
				}
				count++;
				*sc = i;
			}
		}
	return count;
}

static void print_field_cmp(int value, int op, struct audit_line *audit_line)
{
	int cmp_buf_len = 0;
	char *cmp_buf = NULL;
	switch (value)
	{
	case AUDIT_COMPARE_UID_TO_OBJ_UID:
		cmp_buf_len = snprintf(NULL, 0, " -C uid%sobj_uid",
							   audit_operator_to_symbol(op));
		cmp_buf = malloc(cmp_buf_len + 1);
		snprintf(cmp_buf, cmp_buf_len + 1, " -C uid%sobj_uid",
				 audit_operator_to_symbol(op));
		audit_line_push(audit_line, cmp_buf);
		free(cmp_buf);
		break;
	case AUDIT_COMPARE_GID_TO_OBJ_GID:
		cmp_buf_len = snprintf(NULL, 0, " -C gid%sobj_gid",
							   audit_operator_to_symbol(op));
		cmp_buf = malloc(cmp_buf_len + 1);
		snprintf(cmp_buf, cmp_buf_len + 1, " -C gid%sobj_gid",
				 audit_operator_to_symbol(op));
		audit_line_push(audit_line, cmp_buf);
		free(cmp_buf);
		break;
	case AUDIT_COMPARE_EUID_TO_OBJ_UID:
		cmp_buf_len = snprintf(NULL, 0, " -C euid%sobj_uid",
							   audit_operator_to_symbol(op));
		cmp_buf = malloc(cmp_buf_len + 1);
		snprintf(cmp_buf, cmp_buf_len, " -C euid%sobj_uid",
				 audit_operator_to_symbol(op));
		audit_line_push(audit_line, cmp_buf);
		free(cmp_buf);
		break;
	case AUDIT_COMPARE_EGID_TO_OBJ_GID:
		cmp_buf_len = snprintf(NULL, 0, " -C egid%sobj_gid",
							   audit_operator_to_symbol(op));
		cmp_buf = malloc(cmp_buf_len + 1);
		snprintf(cmp_buf, cmp_buf_len + 1, " -C egid%sobj_gid",
				 audit_operator_to_symbol(op));
		audit_line_push(audit_line, cmp_buf);
		free(cmp_buf);
		break;
	case AUDIT_COMPARE_AUID_TO_OBJ_UID:
		cmp_buf_len = snprintf(NULL, 0, " -C auid%sobj_uid",
							   audit_operator_to_symbol(op));
		cmp_buf = malloc(cmp_buf_len + 1);
		snprintf(cmp_buf, cmp_buf_len + 1, " -C auid%sobj_uid",
				 audit_operator_to_symbol(op));
		audit_line_push(audit_line, cmp_buf);
		free(cmp_buf);
		break;
	case AUDIT_COMPARE_SUID_TO_OBJ_UID:
		cmp_buf_len = snprintf(NULL, 0, " -C suid%sobj_uid",
							   audit_operator_to_symbol(op));
		cmp_buf = malloc(cmp_buf_len + 1);
		snprintf(cmp_buf, cmp_buf_len + 1, " -C suid%sobj_uid",
				 audit_operator_to_symbol(op));
		audit_line_push(audit_line, cmp_buf);
		free(cmp_buf);
		break;
	case AUDIT_COMPARE_SGID_TO_OBJ_GID:
		cmp_buf_len = snprintf(NULL, 0, " -C sgid%sobj_gid",
							   audit_operator_to_symbol(op));
		cmp_buf = malloc(cmp_buf_len + 1);
		snprintf(cmp_buf, cmp_buf_len + 1, " -C sgid%sobj_gid",
				 audit_operator_to_symbol(op));
		audit_line_push(audit_line, cmp_buf);
		free(cmp_buf);
		break;
	case AUDIT_COMPARE_FSUID_TO_OBJ_UID:
		cmp_buf_len = snprintf(NULL, 0, " -C fsuid%sobj_uid",
							   audit_operator_to_symbol(op));
		cmp_buf = malloc(cmp_buf_len + 1);
		snprintf(cmp_buf, cmp_buf_len + 1, " -C fsuid%sobj_uid",
				 audit_operator_to_symbol(op));
		audit_line_push(audit_line, cmp_buf);
		free(cmp_buf);
		break;
	case AUDIT_COMPARE_FSGID_TO_OBJ_GID:
		cmp_buf_len = snprintf(NULL, 0, " -C fsgid%sobj_gid",
							   audit_operator_to_symbol(op));
		cmp_buf = malloc(cmp_buf_len + 1);
		snprintf(cmp_buf, cmp_buf_len + 1, " -C fsgid%sobj_gid",
				 audit_operator_to_symbol(op));
		audit_line_push(audit_line, cmp_buf);
		free(cmp_buf);
		break;
	case AUDIT_COMPARE_UID_TO_AUID:
		cmp_buf_len = snprintf(NULL, 0, " -C uid%sauid",
							   audit_operator_to_symbol(op));
		cmp_buf = malloc(cmp_buf_len + 1);
		snprintf(cmp_buf, cmp_buf_len + 1, " -C uid%sauid",
				 audit_operator_to_symbol(op));
		audit_line_push(audit_line, cmp_buf);
		free(cmp_buf);
		break;
	case AUDIT_COMPARE_UID_TO_EUID:
		cmp_buf_len = snprintf(NULL, 0, " -C uid%seuid",
							   audit_operator_to_symbol(op));
		cmp_buf = malloc(cmp_buf_len + 1);
		snprintf(cmp_buf, cmp_buf_len + 1, " -C uid%seuid",
				 audit_operator_to_symbol(op));
		audit_line_push(audit_line, cmp_buf);
		free(cmp_buf);
		break;
	case AUDIT_COMPARE_UID_TO_FSUID:
		cmp_buf_len = snprintf(NULL, 0, " -C uid%sfsuid",
							   audit_operator_to_symbol(op));
		cmp_buf = malloc(cmp_buf_len + 1);
		snprintf(cmp_buf, cmp_buf_len + 1, " -C uid%sfsuid",
				 audit_operator_to_symbol(op));
		audit_line_push(audit_line, cmp_buf);
		free(cmp_buf);
		break;
	case AUDIT_COMPARE_UID_TO_SUID:
		cmp_buf_len = snprintf(NULL, 0, " -C uid%ssuid",
							   audit_operator_to_symbol(op));
		cmp_buf = malloc(cmp_buf_len + 1);
		snprintf(cmp_buf, cmp_buf_len + 1, " -C uid%ssuid",
				 audit_operator_to_symbol(op));
		audit_line_push(audit_line, cmp_buf);
		free(cmp_buf);
		break;
	case AUDIT_COMPARE_AUID_TO_FSUID:
		cmp_buf_len = snprintf(NULL, 0, " -C auid%sfsuid",
							   audit_operator_to_symbol(op));
		cmp_buf = malloc(cmp_buf_len + 1);
		snprintf(cmp_buf, cmp_buf_len + 1, " -C auid%sfsuid",
				 audit_operator_to_symbol(op));
		audit_line_push(audit_line, cmp_buf);
		free(cmp_buf);
		break;
	case AUDIT_COMPARE_AUID_TO_SUID:
		cmp_buf_len = snprintf(NULL, 0, " -C auid%ssuid",
							   audit_operator_to_symbol(op));
		cmp_buf = malloc(cmp_buf_len + 1);
		snprintf(cmp_buf, cmp_buf_len + 1, " -C auid%ssuid",
				 audit_operator_to_symbol(op));
		audit_line_push(audit_line, cmp_buf);
		free(cmp_buf);
		break;
	case AUDIT_COMPARE_AUID_TO_EUID:
		cmp_buf_len = snprintf(NULL, 0, " -C auid%seuid",
							   audit_operator_to_symbol(op));
		cmp_buf = malloc(cmp_buf_len + 1);
		snprintf(cmp_buf, cmp_buf_len + 1, " -C auid%seuid",
				 audit_operator_to_symbol(op));
		audit_line_push(audit_line, cmp_buf);
		free(cmp_buf);
		break;
	case AUDIT_COMPARE_EUID_TO_SUID:
		cmp_buf_len = snprintf(NULL, 0, " -C euid%ssuid",
							   audit_operator_to_symbol(op));
		cmp_buf = malloc(cmp_buf_len + 1);
		snprintf(cmp_buf, cmp_buf_len + 1, " -C euid%ssuid",
				 audit_operator_to_symbol(op));
		audit_line_push(audit_line, cmp_buf);
		free(cmp_buf);
		break;
	case AUDIT_COMPARE_EUID_TO_FSUID:
		cmp_buf_len = snprintf(NULL, 0, " -C euid%sfsuid",
							   audit_operator_to_symbol(op));
		cmp_buf = malloc(cmp_buf_len + 1);
		snprintf(cmp_buf, cmp_buf_len + 1, " -C euid%sfsuid",
				 audit_operator_to_symbol(op));
		audit_line_push(audit_line, cmp_buf);
		free(cmp_buf);
		break;
	case AUDIT_COMPARE_SUID_TO_FSUID:
		cmp_buf_len = snprintf(NULL, 0, " -C suid%sfsuid",
							   audit_operator_to_symbol(op));
		cmp_buf = malloc(cmp_buf_len + 1);
		snprintf(cmp_buf, cmp_buf_len + 1, " -C suid%sfsuid",
				 audit_operator_to_symbol(op));
		audit_line_push(audit_line, cmp_buf);
		free(cmp_buf);
		break;
	case AUDIT_COMPARE_GID_TO_EGID:
		cmp_buf_len = snprintf(NULL, 0, " -C gid%segid",
							   audit_operator_to_symbol(op));
		cmp_buf = malloc(cmp_buf_len + 1);
		snprintf(cmp_buf, cmp_buf_len + 1, " -C gid%segid",
				 audit_operator_to_symbol(op));
		audit_line_push(audit_line, cmp_buf);
		free(cmp_buf);
		break;
	case AUDIT_COMPARE_GID_TO_FSGID:
		cmp_buf_len = snprintf(NULL, 0, " -C gid%sfsgid",
							   audit_operator_to_symbol(op));
		cmp_buf = malloc(cmp_buf_len + 1);
		snprintf(cmp_buf, cmp_buf_len, " -C gid%sfsgid",
				 audit_operator_to_symbol(op));
		audit_line_push(audit_line, cmp_buf);
		free(cmp_buf);
		break;
	case AUDIT_COMPARE_GID_TO_SGID:
		cmp_buf_len = snprintf(NULL, 0, " -C gid%ssgid",
							   audit_operator_to_symbol(op));
		cmp_buf = malloc(cmp_buf_len + 1);
		snprintf(cmp_buf, cmp_buf_len + 1, " -C gid%ssgid",
				 audit_operator_to_symbol(op));
		audit_line_push(audit_line, cmp_buf);
		free(cmp_buf);
		break;
	case AUDIT_COMPARE_EGID_TO_FSGID:
		cmp_buf_len = snprintf(NULL, 0, " -C egid%sfsgid",
							   audit_operator_to_symbol(op));
		cmp_buf = malloc(cmp_buf_len + 1);
		snprintf(cmp_buf, cmp_buf_len + 1, " -C egid%sfsgid",
				 audit_operator_to_symbol(op));
		audit_line_push(audit_line, cmp_buf);
		free(cmp_buf);
		break;
	case AUDIT_COMPARE_EGID_TO_SGID:
		cmp_buf_len = snprintf(NULL, 0, " -C egid%ssgid",
							   audit_operator_to_symbol(op));
		cmp_buf = malloc(cmp_buf_len + 1);
		snprintf(cmp_buf, cmp_buf_len + 1, " -C egid%ssgid",
				 audit_operator_to_symbol(op));
		audit_line_push(audit_line, cmp_buf);
		free(cmp_buf);
		break;
	case AUDIT_COMPARE_SGID_TO_FSGID:
		cmp_buf_len = snprintf(NULL, 0, " -C sgid%sfsgid",
							   audit_operator_to_symbol(op));
		cmp_buf = malloc(cmp_buf_len + 1);
		snprintf(cmp_buf, cmp_buf_len + 1, " -C sgid%sfsgid",
				 audit_operator_to_symbol(op));
		audit_line_push(audit_line, cmp_buf);
		free(cmp_buf);
		break;
	}
}

static int print_arch(unsigned int value, int op, struct audit_line *audit_line)
{
	size_t arch_buf_len = 0;
	int machine;
	_audit_elf = value;
	machine = audit_elf_to_machine(_audit_elf);
	if (machine < 0)
	{

		arch_buf_len = snprintf(NULL, 0, " -F arch%s0x%X", audit_operator_to_symbol(op),
								(unsigned)value);
		char *arch_buf = malloc(arch_buf_len + 1);
		snprintf(arch_buf, arch_buf_len + 1, " -F arch%s0x%X", audit_operator_to_symbol(op),
				 (unsigned)value);
		audit_line_push(audit_line, arch_buf);
		free(arch_buf);
	}
	else
	{
		if (interpret == 0)
		{
			if (__AUDIT_ARCH_64BIT & _audit_elf)
			{
				arch_buf_len = snprintf(NULL, 0, " -F arch%sb64",
										audit_operator_to_symbol(op));
				char *arch_buf = malloc(arch_buf_len + 1);
				snprintf(arch_buf, arch_buf_len + 1, " -F arch%sb64",
						 audit_operator_to_symbol(op));
				audit_line_push(audit_line, arch_buf);
				free(arch_buf);
			}
			else
			{
				arch_buf_len = snprintf(NULL, 0, " -F arch%sb32",
										audit_operator_to_symbol(op));
				char *arch_buf = malloc(arch_buf_len + 1);
				snprintf(arch_buf, arch_buf_len + 1, " -F arch%sb32",
						 audit_operator_to_symbol(op));
				audit_line_push(audit_line, arch_buf);
				free(arch_buf);
			}
		}
		else
		{
			const char *ptr = audit_machine_to_name(machine);
			arch_buf_len = snprintf(NULL, 0, " -F arch%s%s", audit_operator_to_symbol(op),
									ptr);
			char *arch_buf = malloc(arch_buf_len + 1);
			snprintf(arch_buf, arch_buf_len + 1, " -F arch%s%s", audit_operator_to_symbol(op),
					 ptr);
			audit_line_push(audit_line, arch_buf);
			free(arch_buf);
		}
	}
	return machine;
}

/*
 *  This function prints 1 rule from the kernel reply
 */
struct audit_line *get_rule(const struct audit_rule_data *r)
{
	unsigned int i, count = 0, sc = 0;
	size_t boffset = 0;
	int mach = -1, watch = is_watch(r);
	unsigned long long a0 = 0, a1 = 0;
	struct audit_line *audit_line = audit_line_new();

	if (!watch)
	{ /* This is syscall auditing */
		size_t buf_len = 0;
		buf_len = snprintf(NULL, 0, "-a %s,%s",
						   audit_action_to_name((int)r->action),
						   audit_flag_to_name(r->flags));

		char *syscall_audit = malloc(buf_len + 1);
		snprintf(syscall_audit, buf_len + 1, "-a %s,%s",
				 audit_action_to_name((int)r->action),
				 audit_flag_to_name(r->flags));

		audit_line_push(audit_line, syscall_audit);
		free(syscall_audit);

		// Now find the arch and print it
		for (i = 0; i < r->field_count; i++)
		{
			int field = r->fields[i] & ~AUDIT_OPERATORS;
			if (field == AUDIT_ARCH)
			{
				int op = r->fieldflags[i] & AUDIT_OPERATORS;
				mach = print_arch(r->values[i], op, audit_line);
			}
		}
		// And last do the syscalls
		count = print_syscall(r, &sc, audit_line);
	}

	// Now iterate over the fields
	for (i = 0; i < r->field_count; i++)
	{
		const char *name;
		int op = r->fieldflags[i] & AUDIT_OPERATORS;
		int field = r->fields[i] & ~AUDIT_OPERATORS;
		size_t field_value_len = 0;

		if (field == AUDIT_ARCH)
			continue; // already printed

		name = audit_field_to_name(field);
		if (name)
		{
			// Special cases to print the different field types
			// in a meaningful way.
			if (field == AUDIT_MSGTYPE)
			{
				if (!audit_msg_type_to_name(r->values[i]))
				{
					field_value_len = snprintf(NULL, 0, " -F %s%s%d", name,
											   audit_operator_to_symbol(op),
											   r->values[i]);
					char *audit_msg_buf = malloc(field_value_len + 1);
					snprintf(audit_msg_buf, field_value_len + 1, " -F %s%s%d", name,
							 audit_operator_to_symbol(op),
							 r->values[i]);
					audit_line_push(audit_line, audit_msg_buf);
					free(audit_msg_buf);
				}
				else
				{
					field_value_len = snprintf(NULL, 0, " -F %s%s%s", name,
											   audit_operator_to_symbol(op),
											   audit_msg_type_to_name(
												   r->values[i]));
					char *audit_msg_buf = malloc(field_value_len + 1);
					snprintf(audit_msg_buf, field_value_len + 1, " -F %s%s%s", name,
							 audit_operator_to_symbol(op),
							 audit_msg_type_to_name(
								 r->values[i]));
					audit_line_push(audit_line, audit_msg_buf);
					free(audit_msg_buf);
				}
			}
			else if ((field >= AUDIT_SUBJ_USER &&
					  field <= AUDIT_OBJ_LEV_HIGH) &&
					 field != AUDIT_PPID)
			{
				field_value_len = snprintf(NULL, 0, " -F %s%s%.*s", name,
										   audit_operator_to_symbol(op),
										   r->values[i], &r->buf[boffset]);
				char *audit_field = malloc(field_value_len + 1);
				snprintf(audit_field, field_value_len + 1, " -F %s%s%.*s", name,
						 audit_operator_to_symbol(op),
						 r->values[i], &r->buf[boffset]);
				audit_line_push(audit_line, audit_field);
				free(audit_field);
				boffset += r->values[i];
			}
			else if (field == AUDIT_WATCH)
			{
				if (watch)
				{
					field_value_len = snprintf(NULL, 0, "-w %.*s", r->values[i],
											   &r->buf[boffset]);
					char *audit_field = malloc(field_value_len + 1);
					snprintf(audit_field, field_value_len + 1, "-w %.*s", r->values[i],
							 &r->buf[boffset]);
					audit_line_push(audit_line, audit_field);
					free(audit_field);
				}
				else
				{
					field_value_len = snprintf(NULL, 0, " -F path=%.*s", r->values[i],
											   &r->buf[boffset]);
					char *audit_field = malloc(field_value_len + 1);
					snprintf(audit_field, field_value_len + 1, " -F path=%.*s", r->values[i],
							 &r->buf[boffset]);
					audit_line_push(audit_line, audit_field);
					free(audit_field);
				}
				boffset += r->values[i];
			}
			else if (field == AUDIT_DIR)
			{
				if (watch)
				{
					field_value_len = snprintf(NULL, 0, "-w %.*s", r->values[i],
											   &r->buf[boffset]);
					char *audit_field = malloc(field_value_len + 1);
					snprintf(audit_field, field_value_len + 1, "-w %.*s", r->values[i],
							 &r->buf[boffset]);
					audit_line_push(audit_line, audit_field);
					free(audit_field);
				}
				else
				{
					field_value_len = snprintf(NULL, 0, " -F dir=%.*s", r->values[i],
											   &r->buf[boffset]);
					char *audit_field = malloc(field_value_len + 1);
					snprintf(audit_field, field_value_len + 1, " -F dir=%.*s", r->values[i],
							 &r->buf[boffset]);
					audit_line_push(audit_line, audit_field);
					free(audit_field);
				}

				boffset += r->values[i];
			}
			else if (field == AUDIT_EXE)
			{
				field_value_len = snprintf(NULL, 0, " -F exe=%.*s",
										   r->values[i], &r->buf[boffset]);
				char *audit_field = malloc(field_value_len + 1);
				snprintf(audit_field, field_value_len + 1, " -F exe=%.*s",
						 r->values[i], &r->buf[boffset]);
				audit_line_push(audit_line, audit_field);
				free(audit_field);
				boffset += r->values[i];
			}
			else if (field == AUDIT_FILTERKEY)
			{
				char *rkey, *ptr, *saved;
				if (asprintf(&rkey, "%.*s", r->values[i],
							 &r->buf[boffset]) < 0)
					rkey = NULL;
				boffset += r->values[i];
				ptr = strtok_r(rkey, key_sep, &saved);
				while (ptr)
				{
					if (watch)
					{
						field_value_len = snprintf(NULL, 0, " -k %s", ptr);
						char *audit_field = malloc(field_value_len + 1);
						snprintf(audit_field, field_value_len + 1, " -k %s", ptr);
						audit_line_push(audit_line, audit_field);
						audit_line_add_filter_key(audit_line, ptr);
						free(audit_field);
					}
					else
					{
						field_value_len = snprintf(NULL, 0, " -F key=%s", ptr);
						char *audit_field = malloc(field_value_len + 1);
						snprintf(audit_field, field_value_len + 1, " -F key=%s", ptr);
						audit_line_push(audit_line, audit_field);
						audit_line_add_filter_key(audit_line, ptr);
						free(audit_field);
					}
					ptr = strtok_r(NULL, key_sep, &saved);
				}
				free(rkey);
			}
			else if (field == AUDIT_PERM)
			{
				char perms[5];
				unsigned int val = r->values[i];
				perms[0] = 0;
				if (val & AUDIT_PERM_READ)
					strcat(perms, "r");
				if (val & AUDIT_PERM_WRITE)
					strcat(perms, "w");
				if (val & AUDIT_PERM_EXEC)
					strcat(perms, "x");
				if (val & AUDIT_PERM_ATTR)
					strcat(perms, "a");
				if (watch)
				{
					field_value_len = snprintf(NULL, 0, " -p %s", perms);
					char *audit_field = malloc(field_value_len + 1);
					snprintf(audit_field, field_value_len + 1, " -p %s", perms);
					audit_line_push(audit_line, audit_field);
					free(audit_field);
				}
				else
				{
					field_value_len = snprintf(NULL, 0, " -F perm=%s", perms);
					char *audit_field = malloc(field_value_len + 1);
					snprintf(audit_field, field_value_len + 1, " -F perm=%s", perms);
					audit_line_push(audit_line, audit_field);
					free(audit_field);
				}
			}
			else if (field == AUDIT_INODE)
			{
				// This is unsigned
				field_value_len = snprintf(NULL, 0, " -F %s%s%u", name,
										   audit_operator_to_symbol(op),
										   r->values[i]);
				char *audit_field = malloc(field_value_len + 1);
				snprintf(audit_field, field_value_len + 1, " -F %s%s%u", name,
						 audit_operator_to_symbol(op),
						 r->values[i]);
				audit_line_push(audit_line, audit_field);
				free(audit_field);
			}
			else if (field == AUDIT_FIELD_COMPARE)
			{
				print_field_cmp(r->values[i], op, audit_line);
			}
			// This section is based on internal libaudit and auparse component
			/* else if (field >= AUDIT_ARG0 && field <= AUDIT_ARG3)
			{
				if (field == AUDIT_ARG0)
					a0 = r->values[i];
				else if (field == AUDIT_ARG1)
					a1 = r->values[i];

				// Show these as hex
				if (count > 1 || interpret == 0)
					printf(" -F %s%s0x%X", name, 
						audit_operator_to_symbol(op),
						r->values[i]);
				else {	// Use ignore to mean interpret
					const char *out;
					idata id;
					char val[32];
					int type;

					id.syscall = sc;
					id.machine = mach;
					id.a0 = a0;
					id.a1 = a1;
					id.name = name;
					id.cwd = NULL;
					snprintf(val, 32, "%x", r->values[i]);
					id.val = val;
					type = auparse_interp_adjust_type(
						AUDIT_SYSCALL, name, val);
					out = auparse_do_interpretation(type,
							&id,
							AUPARSE_ESC_TTY);
					printf(" -F %s%s%s", name,
						audit_operator_to_symbol(op),
								out);
					free((void *)out);
				}
			} */
			else if (field == AUDIT_EXIT)
			{
				int e = abs((int)r->values[i]);
				const char *err = audit_errno_to_name(e);
				if (((int)r->values[i] < 0) && err)
				{
					field_value_len = snprintf(NULL, 0, " -F %s%s-%s", name,
											   audit_operator_to_symbol(op),
											   err);
					char *audit_field = malloc(field_value_len + 1);
					snprintf(audit_field, field_value_len + 1, " -F %s%s-%s", name,
							 audit_operator_to_symbol(op),
							 err);
					audit_line_push(audit_line, audit_field);
					free(audit_field);
				}
				else
				{
					field_value_len = snprintf(NULL, 0, " -F %s%s%d", name,
											   audit_operator_to_symbol(op),
											   (int)r->values[i]);
					char *audit_field = malloc(field_value_len + 1);
					snprintf(audit_field, field_value_len + 1, " -F %s%s%d", name,
							 audit_operator_to_symbol(op),
							 (int)r->values[i]);
					audit_line_push(audit_line, audit_field);
					free(audit_field);
				}
			}
			else if (field == AUDIT_FSTYPE)
			{
				if (!audit_fstype_to_name(r->values[i]))
				{
					field_value_len = snprintf(NULL, 0, " -F %s%s%d", name,
											   audit_operator_to_symbol(op),
											   r->values[i]);
					char *audit_field = malloc(field_value_len + 1);
					snprintf(audit_field, field_value_len + 1, " -F %s%s%d", name,
							 audit_operator_to_symbol(op),
							 r->values[i]);
					audit_line_push(audit_line, audit_field);
					free(audit_field);
				}
				else
				{
					field_value_len = snprintf(NULL, 0, " -F %s%s%s", name,
											   audit_operator_to_symbol(op),
											   audit_fstype_to_name(
												   r->values[i]));
					char *audit_field = malloc(field_value_len + 1);
					snprintf(audit_field, field_value_len + 1, " -F %s%s%s", name,
							 audit_operator_to_symbol(op),
							 audit_fstype_to_name(
								 r->values[i]));
					audit_line_push(audit_line, audit_field);
					free(audit_field);
				}
			}
			else
			{
				// The default is signed decimal
				field_value_len = snprintf(NULL, 0, " -F %s%s%d", name,
										   audit_operator_to_symbol(op),
										   r->values[i]);
				char *audit_field = malloc(field_value_len + 1);
				snprintf(audit_field, field_value_len + 1, " -F %s%s%d", name,
						 audit_operator_to_symbol(op),
						 r->values[i]);
				audit_line_push(audit_line, audit_field);
				free(audit_field);
			}
		}
		else
		{
			// The field name is unknown
			field_value_len = snprintf(NULL, 0, " f%d%s%d", r->fields[i],
									   audit_operator_to_symbol(op),
									   r->values[i]);
			char *audit_field = malloc(field_value_len + 1);
			snprintf(audit_field, field_value_len + 1, " f%d%s%d", r->fields[i],
					 audit_operator_to_symbol(op),
					 r->values[i]);
			audit_line_push(audit_line, audit_field);
			free(audit_field);
		}
	}
	return audit_line;
}

/*
 * A reply from the kernel is expected. Get and display it.
 */
static struct oscap_list *get_reply(int fd)
{
	struct oscap_list *audit_list = oscap_list_new();
	int i, retval;
	int line_number = 1;
	int timeout = 40; /* loop has delay of .1 - so this is 4 seconds */
	struct audit_reply rep;
	fd_set read_mask;
	FD_ZERO(&read_mask);
	FD_SET(fd, &read_mask);

	for (i = 0; i < timeout; i++)
	{
		struct timeval t;

		t.tv_sec = 0;
		t.tv_usec = 100000; /* .1 second */
		do
		{
			retval = select(fd + 1, &read_mask, NULL, NULL, &t);
		} while (retval < 0 && errno == EINTR);
		// We'll try to read just in case
		retval = audit_get_reply(fd, &rep, GET_REPLY_NONBLOCKING, 0);
		if (retval > 0)
		{
			if (rep.type == NLMSG_ERROR && rep.error->error == 0)
			{
				i = 0;	  /* reset timeout */
				continue; /* This was an ack */
			}
			else if (rep.type == NLMSG_DONE)
			{
				break;
			}
			// TODO gestion des types de messages n'�tant pas des rules
			struct audit_line *audit_line = get_rule(rep.ruledata);
			audit_line->line_number = line_number;
			oscap_list_add(audit_list, audit_line);
			line_number++;
		}
	}
	return audit_list;
}

// End of the modified code

static struct oscap_list *get_all_audit_rules(probe_ctx *ctx)
{

	int audit_fd = audit_open();

	if (audit_fd > -1)
	{

		// Request the kernel to provide the audit rules list
		if (audit_request_rules_list_data(audit_fd) > 0)
		{

			dD("Successfull request to get the kernel audit rules.");

			struct oscap_list *audit_list = get_reply(audit_fd);
			audit_close(audit_fd);
			return audit_list;
		}
		else
		{
			int errnum = errno;
			dE("Failed to request the kernel for audit rules : %s", strerror(errnum));
			SEXP_t *item = probe_item_create(
				OVAL_DGAMI_AUDITDLINE, NULL,
				"filter_key", OVAL_DATATYPE_STRING, "",
				NULL);

			probe_item_setstatus(item, SYSCHAR_STATUS_ERROR);
			probe_item_add_msg(item, OVAL_MESSAGE_LEVEL_ERROR,
							   "Failed to request the kernel for audit rules : %s.", strerror(errnum));
			probe_item_collect(ctx, item);
		}
	}
	else
	{
		int errnum = errno;
		dE("Failed to get a handle for the audit system : %s", strerror(errnum));
		SEXP_t *item = probe_item_create(
			OVAL_DGAMI_AUDITDLINE, NULL,
			"filter_key", OVAL_DATATYPE_STRING, "",
			NULL);

		probe_item_setstatus(item, SYSCHAR_STATUS_ERROR);
		probe_item_add_msg(item, OVAL_MESSAGE_LEVEL_ERROR,
						   "Failed to obtain a handle to the audit system : %s.", strerror(errnum));
		probe_item_collect(ctx, item);
	}

	audit_close(audit_fd);
	return NULL;
}

int auditdline_probe_offline_mode_supported()
{
	return PROBE_OFFLINE_CHROOT;
}

int auditdline_probe_main(probe_ctx *ctx, void *arg)
{

	SEXP_t *probe_in = probe_ctx_getobject(ctx);
	SEXP_t *key_filter = probe_obj_getent(probe_in, "filter_key", 1);
	bool isNil = probe_ent_getval(key_filter) == NULL;

	struct oscap_list *audit_list = get_all_audit_rules(ctx);

	if (audit_list != NULL)
	{
		struct oscap_iterator *audit_list_iter = oscap_iterator_new(audit_list);
		while (oscap_iterator_has_more(audit_list_iter))
		{
			struct audit_line *audit_line = oscap_iterator_next(audit_list_iter);
			if (!isNil)
			{
				struct oscap_string_iterator *filter_key_iter = oscap_stringlist_get_strings(audit_line->filter_key);
				while (oscap_string_iterator_has_more(filter_key_iter))
				{
					const char *filter_key = oscap_string_iterator_next(filter_key_iter);
					SEXP_t *sexp_filter_key = SEXP_string_newf("%s", filter_key);
					if (probe_entobj_cmp(key_filter, sexp_filter_key) == OVAL_RESULT_TRUE)
					{
						SEXP_t *item = probe_item_create(
							OVAL_DGAMI_AUDITDLINE, NULL,
							"filter_key", OVAL_DATATYPE_STRING, filter_key,
							"auditline", OVAL_DATATYPE_STRING, oscap_string_get_cstr(audit_line->audit_line),
							"line_number", OVAL_DATATYPE_INTEGER, audit_line->line_number,
							NULL);
						probe_item_setstatus(item, SYSCHAR_STATUS_EXISTS);
						probe_item_collect(ctx, item);
						SEXP_free(sexp_filter_key);
						break;
					}
					SEXP_free(sexp_filter_key);
				}
				oscap_string_iterator_free(filter_key_iter);
			}
			else
			{
				SEXP_t *item = probe_item_create(
					OVAL_DGAMI_AUDITDLINE, NULL,
					"auditline", OVAL_DATATYPE_STRING, oscap_string_get_cstr(audit_line->audit_line),
					"line_number", OVAL_DATATYPE_INTEGER, audit_line->line_number,
					NULL);
				probe_item_setstatus(item, SYSCHAR_STATUS_EXISTS);
				probe_item_collect(ctx, item);
			}
		}
		oscap_iterator_free(audit_list_iter);
	}

	// Cleanup of the ressources
	SEXP_free(key_filter);
	oscap_list_free(audit_list, auditd_line_free);

	return 0;
}