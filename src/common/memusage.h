#ifndef MEMUSAGE_H
#define MEMUSAGE_H

struct memusage {
	size_t mu_rss;
	size_t mu_hwm;
	size_t mu_lib;
	size_t mu_text;
	size_t mu_data;
	size_t mu_stack;
	size_t mu_lock;
};

int memusage (struct memusage *mu);

#endif /* MEMUSAGE_H */
