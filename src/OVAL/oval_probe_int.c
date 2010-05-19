static struct oval_syschar *oval_probe_envvar_handler(struct oval_object *obj)
{
        struct oval_syschar *sys;
        struct oval_entity  *ent;
        struct oval_object_content_iterator *cit;
        struct oval_object_content *con;

        char *var_name;

        cit = oval_object_get_object_contents(object);

	while (oval_object_content_iterator_has_more(cit)) {
                con = oval_object_content_iterator_next(cit);

                switch(oval_object_content_get_type(con)) {
                case OVAL_OBJECTCONTENT_ENTITY:
                        ent = oval_object_content_get_entity(con);
                        /* Aaarghhh! Morbo: OBJECTS DO NOT WORK THAT WAY! */
                }
        }

        return(NULL); /* let's return NULL for now and get more cups of coffe */
}

int oval_probe_envvar_handler(oval_subtype_t type, void *ptr, int act, ...)
{
        int ret = 0;
        va_list ap;

        va_start(ap, act);

        switch(act) {
        case PROBE_HANDLER_ACT_EVAL:
        {
                struct oval_object   *obj = va_arg(ap, struct oval_object *);
                struct oval_syschar **sys = va_arg(ap, struct oval_syschar **);
                int flags = va_arg(ap, int);

                *sys = oval_probe_envvar_eval(obj);
                ret  = (*sys == NULL ? -1 : 0);
                break;
        }
        case PROBE_HANDLER_ACT_INIT:
        case PROBE_HANDLER_ACT_OPEN:
        case PROBE_HANDLER_ACT_CLOSE:
        case PROBE_HANDLER_ACT_FREE:
        case PROBE_HANDLER_ACT_RESET:
                break;
        default:
                errno = EINVAL;
                ret = -1;
        }

        va_end(ap);
        return(ret);
}

int oval_probe_var_handler(oval_subtype_t type, void *ptr, int act, ...)
{
        int ret = 0;
        va_list ap;

        va_start(ap, act);

        switch(act) {
        case PROBE_HANDLER_ACT_EVAL:
        {
                struct oval_object   *obj = va_arg(ap, struct oval_object *);
                struct oval_syschar **sys = va_arg(ap, struct oval_syschar **);
                int flags = va_arg(ap, int);

                break;
        }
        case PROBE_HANDLER_ACT_INIT:
        case PROBE_HANDLER_ACT_OPEN:
        case PROBE_HANDLER_ACT_CLOSE:
        case PROBE_HANDLER_ACT_FREE:
        case PROBE_HANDLER_ACT_RESET:
                break;
        default:
                errno = EINVAL;
                ret = -1;
        }

        va_end(ap);
        return(ret);
}
