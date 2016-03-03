
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sat Nov  7 21:52:42 2015
 * Project: libcollections
 *
 * Copyright (C) 2015 Rodrigo Freitas
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 */

#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

#include "collections.h"
#include "plugin/plugin.h"

/*
 * -- ARGUMENTS
 */

/**
 * @name cplugin_arg_c
 * @brief Gets current argument value of 'char' type.
 *
 * It is recomended to use CPLUGIN_GET_ARG_c macro instead of a direct call to
 * this function.
 *
 * @param [in] args: The cplugin_arg_t object with function arguments.
 * @param [in] arg_name: The argument name.
 *
 * @return On success return the 'char' value of the argument or -1 otherwise.
 */
char LIBEXPORT cplugin_arg_c(const cplugin_arg_t *args, const char *arg_name)
{
    cvalue_t *p = NULL;

    p = get_arg_value(args, arg_name);

    if (NULL == p)
        return -1;

    return CVALUE_CHAR(p);
}

/**
 * @name cplugin_arg_i
 * @brief Gets current argument value of 'int' type.
 *
 * It is recomended to use CPLUGIN_GET_ARG_i macro instead of a direct call to
 * this function.
 *
 * @param [in] args: The cplugin_arg_t object with function arguments.
 * @param [in] arg_name: The argument name.
 *
 * @return On success return the 'int' value of the argument or -1 otherwise.
 */
int LIBEXPORT cplugin_arg_i(const cplugin_arg_t *args, const char *arg_name)
{
    cvalue_t *p = NULL;

    p = get_arg_value(args, arg_name);

    if (NULL == p)
        return -1;

    return CVALUE_INT(p);
}

/**
 * @name cplugin_arg_p
 * @brief Gets current argument value of 'void *' type.
 *
 * It is recomended to use CPLUGIN_GET_ARG_p macro instead of a direct call to
 * this function.
 *
 * @param [in] args: The cplugin_arg_t object with function arguments.
 * @param [in] arg_name: The argument name.
 *
 * @return On success return the 'void *' value of the argument or NULL
 *         otherwise.
 */
void LIBEXPORT *cplugin_arg_p(const cplugin_arg_t *args, const char *arg_name)
{
    cvalue_t *p = NULL;
    unsigned int size = 0;

    p = get_arg_value(args, arg_name);

    if (NULL == p)
        return NULL;

    return CVALUE_POINTER(p, &size);
}

/**
 * @name cplugin_arg_f
 * @brief Gets current argument value of 'float' type.
 *
 * It is recomended to use CPLUGIN_GET_ARG_f macro instead of a direct call to
 * this function.
 *
 * @param [in] args: The cplugin_arg_t object with function arguments.
 * @param [in] arg_name: The argument name.
 *
 * @return On success return the 'float' value of the argument or -1 otherwise.
 */
float LIBEXPORT cplugin_arg_f(const cplugin_arg_t *args, const char *arg_name)
{
    cvalue_t *p = NULL;

    p = get_arg_value(args, arg_name);

    if (NULL == p)
        return -1.0F;

    return CVALUE_FLOAT(p);
}

/**
 * @name cplugin_arg_count
 * @brief Gets the number of arguments of a function.
 *
 * It is recomended to use CPLUGIN_ARG_COUNT macro instead of a direct call to
 * this function.
 *
 * @param [in] args: The cplugin_arg_t object with function arguments.
 *
 * @return On success returns the function number of arguments or -1
 *         otherwise.
 */
int LIBEXPORT cplugin_arg_count(const cplugin_arg_t *args)
{
    cerrno_clear();

    if (NULL == args) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return cdll_size((void *)args);
}

/*
 * -- RETURN VALUE
 */

/**
 * @name cplugin_set_return_value
 * @brief Sets up the return value from an exported plugin function.
 *
 * This functions must be called within an exported plugin function. It should be
 * called as a C return statement, so a value may returned to the caller.
 *
 * It will block using a mutex to add the returned value into an internal list
 * of return values.
 *
 * It is recommended that the CPLUGIN_RETURN_VALUE macro is called instead of a
 * direct call to this function.
 *
 * If the return value is a pointer, its size must also be informed, so the
 * library is able to dup it inside. Also a custom function to release memory
 * from this data may be informed.
 *
 * @param [in] cpl: The cplugin_t object.
 * @param [in] function_name: The function name which will have its return
 *                            value set.
 * @param [in] caller_id: Identification function number.
 * @param [in] type: The return type.
 * @param [in] ...: The return value.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int LIBEXPORT cplugin_set_return_value(cplugin_t *cpl, const char *function_name,
    unsigned int caller_id, enum cl_type type, ...)
{
    struct cplugin_function_s *foo = NULL;
    struct cplugin_fdata_s *return_value = NULL;
    va_list ap;
    void *p;
    int psize;

    cerrno_clear();

    if ((NULL == cpl) || (NULL == function_name)) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    foo = cdll_map(cpl->functions, search_cplugin_function_s,
                   (char *)function_name);

    if (NULL == foo) {
        cset_errno(CL_FUNCTION_NOT_FOUND);
        return -1;
    }

    return_value = new_cplugin_fdata_s(NULL, type, caller_id);

    if (NULL == return_value)
        return -1;

    va_start(ap, NULL);

    switch (type) {
        case CL_INT:
            return_value->value = cvalue_new(CL_INT, va_arg(ap, int), NULL);
            break;

        case CL_UINT:
            return_value->value = cvalue_new(CL_UINT,
                                             (unsigned int)va_arg(ap, int),
                                             NULL);

            break;

        case CL_VOID:
            /* noop */
            break;

        case CL_POINTER:
            p = va_arg(ap, void *);
            psize = va_arg(ap, int);
            return_value->value = cvalue_new(CL_POINTER, true, p, psize, NULL);
            break;

        case CL_CHAR:
            return_value->value = cvalue_new(CL_CHAR, (char)va_arg(ap, int),
                                             NULL);

            break;

        case CL_FLOAT:
            return_value->value = cvalue_new(CL_FLOAT, va_arg(ap, double), NULL);
            break;

        default:
            cset_errno(CL_UNSUPPORTED_TYPE);
            destroy_cplugin_fdata_s(return_value);
            return -1;
    }

    va_end(ap);

    /* Add into the return values list */
    pthread_mutex_lock(&foo->m_return_value);
    foo->values = cdll_unshift(foo->values, return_value);
    pthread_mutex_unlock(&foo->m_return_value);

    return 0;
}

/**
 * @name cplugin_get_value_c
 * @brief Gets the function return value as a 'char'.
 *
 * @param [in] return_value: The function return object.
 *
 * @return On success returns the function return value as an 'char' or -1
 *         otherwise.
 */
char LIBEXPORT cplugin_get_value_c(cplugin_value_t *return_value)
{
    struct cplugin_fdata_s *crv = (struct cplugin_fdata_s *)return_value;

    cerrno_clear();

    if (NULL == return_value) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    if (crv->type != CL_CHAR) {
        cset_errno(CL_WRONG_TYPE);
        return -1;
    }

    return CVALUE_CHAR(crv->value);
}

/**
 * @name cplugin_get_value_i
 * @brief Gets the function return value as an 'int'.
 *
 * @param [in] return_value: The function return object.
 *
 * @return On success returns the function return value as an 'int' or -1
 *         otherwise.
 */
int LIBEXPORT cplugin_get_value_i(cplugin_value_t *return_value)
{
    struct cplugin_fdata_s *crv = (struct cplugin_fdata_s *)return_value;

    cerrno_clear();

    if (NULL == return_value) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    if (crv->type != CL_INT) {
        cset_errno(CL_WRONG_TYPE);
        return -1;
    }

    return CVALUE_INT(crv->value);
}

/**
 * @name cplugin_get_value_p
 * @brief Gets the function return value as a 'void *'.
 *
 * @param [in] return_value: The function return object.
 *
 * @return On success returns the function return value as a 'void *' or NULL
 *         otherwise.
 */
void LIBEXPORT *cplugin_get_value_p(cplugin_value_t *return_value)
{
    struct cplugin_fdata_s *crv = (struct cplugin_fdata_s *)return_value;
    unsigned int size = 0;

    cerrno_clear();

    if (NULL == return_value) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    if (crv->type != CL_POINTER) {
        cset_errno(CL_WRONG_TYPE);
        return NULL;
    }

    return CVALUE_POINTER(crv->value, &size);
}

/**
 * @name cplugin_get_value_f
 * @brief Gets the function return value as a 'float'.
 *
 * @param [in] return_value: The function return object.
 *
 * @return On success returns the function return value as a 'float' or -1
 *         otherwise.
 */
float LIBEXPORT cplugin_get_value_f(cplugin_value_t *return_value)
{
    struct cplugin_fdata_s *crv = (struct cplugin_fdata_s *)return_value;

    cerrno_clear();

    if (NULL == return_value) {
        cset_errno(CL_NULL_ARG);
        return -1.0F;
    }

    if (crv->type != CL_FLOAT) {
        cset_errno(CL_WRONG_TYPE);
        return -1.0F;
    }

    return CVALUE_FLOAT(crv->value);
}

/**
 * @name cplugin_destroy_value
 * @brief Relases an object which stores the function return value.
 *
 * @param [in] return_value: The object which will be released.
 *
 * @return On success returns 0 or -1 otherwise.
 */
int LIBEXPORT cplugin_destroy_value(cplugin_value_t *return_value)
{
    cerrno_clear();

    if (NULL == return_value) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    destroy_cplugin_fdata_s(return_value);

    return 0;
}

/*
 * -- PLUGIN INFORMATION API
 */

cplugin_info_t LIBEXPORT *cplugin_info(const cplugin_t *cpl)
{
    cerrno_clear();

    if (NULL == cpl) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return cpl->info;
}

cplugin_info_t LIBEXPORT *cplugin_info_from_file(const char *pathname)
{
    void *handle = NULL;
    struct cplugin_info_s *info = NULL;
    enum cplugin_plugin_type plugin_type;

    cerrno_clear();

    if (NULL == pathname) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    plugin_type = guess_plugin_type(pathname);
    handle = dl_open(pathname, plugin_type);

    if (NULL == handle) {
        cset_errno(CL_DLOPEN);
        return NULL;
    }

    /* Loads plugin info */
    info = dl_load_info(handle, plugin_type);

    if (handle != NULL)
        dl_close(handle, plugin_type);

    return info;
}

int LIBEXPORT cplugin_info_unref(cplugin_info_t *info)
{
    cerrno_clear();

    if (NULL == info) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    info_unref(info);

    return 0;
}

const char LIBEXPORT *cplugin_name(const cplugin_info_t *info)
{
    cerrno_clear();

    if (NULL == info) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return info_get_name(info);
}

const char LIBEXPORT *cplugin_version(const cplugin_info_t *info)
{
    cerrno_clear();

    if (NULL == info) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return info_get_version(info);
}

const char LIBEXPORT *cplugin_author(const cplugin_info_t *info)
{
    cerrno_clear();

    if (NULL == info) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return info_get_author(info);
}

const char LIBEXPORT *cplugin_description(const cplugin_info_t *info)
{
    cerrno_clear();

    if (NULL == info) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return info_get_description(info);
}

cstring_t LIBEXPORT *cplugin_API(const cplugin_info_t *info)
{
    cjson_t *api;

    cerrno_clear();

    if (NULL == info) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    api = info_get_api(info);

    if (NULL == api)
        return NULL;

    return api_to_string(api);
}

cstring_list_t LIBEXPORT *cplugin_functions(const cplugin_info_t *info)
{
    cerrno_clear();

    if (NULL == info) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return api_functions(info);
}

enum cl_type LIBEXPORT cplugin_function_return_type(const cplugin_info_t *info,
    const char *function_name)
{
    cerrno_clear();

    if ((NULL == info) || (NULL == function_name)) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return api_function_return_type(info, function_name);
}

cstring_list_t LIBEXPORT *cplugin_function_arguments(const cplugin_info_t *info,
    const char *function_name)
{
    cerrno_clear();

    if ((NULL == info) || (NULL == function_name)) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return api_function_arguments(info, function_name);
}

enum cplugin_arg LIBEXPORT cplugin_function_arg_mode(const cplugin_info_t *info,
    const char *function_name)
{
    cerrno_clear();

    if ((NULL == info) || (NULL == function_name)) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return api_function_arg_mode(info, function_name);
}

enum cl_type LIBEXPORT cplugin_function_arg_type(const cplugin_info_t *info,
    const char *function_name, const char *argument_name)
{
    cerrno_clear();

    if ((NULL == info) || (NULL == function_name) || (NULL == argument_name)) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    return api_function_arg_type(info, function_name, argument_name);
}

/*
 * -- PLUGIN MANIPULATION
 */

/**
 * @name cplugin_call_ex
 * @brief Makes a call to a function inside a plugin.
 *
 * This function is responsible for making the call of a function inside a
 * plugin. Its arguments are inserted into a list so they can be accessed
 * inside of it.
 *
 * The called function arguments must be passed according with the type
 * defined on the plugin API.
 *
 * To the case of a function with the CPLUGIN_ARG_FIXED arguments type,
 * they must be passed this way: "argument_name", value, "argument_name",
 * "value", NULL. Such as:
 *
 * cplugin_call(7, cpl, "foo", "arg1", value, "arg2", value, NULL);
 *
 * To the case of CPLUGIN_ARG_VAR arguments type, they must be passed in
 * this way: "argument_name", type, value, "argument_name", type, value,
 * NULL. Such as:
 *
 * cplugin_call_ex(9, cpl, "foo", "arg1", type, value, "arg2", type, value,
 *                 NULL);
 *
 * It is recomended to use cplugin_call macro instead of a direct call to
 * this function.
 *
 * @param [in] argc: The total number of arguments passed to the function.
 * @param [in] cpl: The cplugin_t object from the loaded plugin.
 * @param [in] function_name: The function name which will be called.
 * @param [in] ...: Function arguments.
 *
 * @return On success returns a cplugin_value_t with the called function return
 *         value or NULL otherwise.
 */
cplugin_value_t LIBEXPORT *cplugin_call_ex(int argc, cplugin_t *cpl,
    const char *function_name, ...)
{
    struct cplugin_function_s *foo = NULL;
    va_list ap;
    int fargc = 0;
    struct cplugin_fdata_s *cplv = NULL;
    uint32_t caller_id = 0;

    cerrno_clear();
    va_start(ap, NULL);
    argc -= CPLUGIN_CALL_DEF_ARGUMENTS;
    foo = cdll_map(cpl->functions, search_cplugin_function_s,
                   (char *)function_name);

    if (NULL == foo) {
        cset_errno(CL_FUNCTION_NOT_FOUND);
        return NULL;
    }

    /* Checks if the arguments are right */
    if (foo->type_of_args == CPLUGIN_ARG_FIXED) {
        fargc = cdll_size(foo->args);

        if ((argc / 2) != fargc) {
            cset_errno(CL_INVALID_VALUE);
            return NULL;
        }
    }

    /* Set up arguments value */
    if (foo->type_of_args != CPLUGIN_NO_ARGS)
        if (adjust_arguments(foo, argc, ap) < 0)
            return NULL;

    /*
     * Creates an identification number to the function.
     *
     * TODO: Implement a way to ensure that this will not repeat for other
     *       calls.
     */
    caller_id = random_caller_id(foo);

    /*
     * If the same function is called at the same time, one may set its
     * return value while the other is still trying to get it or both may
     * try set it at the same time.
     *
     * One way to solve this is to use the random identification number
     * and passed it to the called function.
     */

    /* Call the function */
    dl_call(foo, caller_id, cpl);

    if (foo->return_value != CL_VOID)
        cplv = cplugin_get_return_value(cpl, function_name, caller_id);

    /* Unload arguments */
    if (foo->type_of_args == CPLUGIN_ARG_VAR) {
        cdll_free(foo->args, destroy_cplugin_fdata_s);
        foo->args = NULL;
    }

    if ((NULL == cplv) && (foo->return_value != CL_VOID))
        return NULL;

    return cplv;
}

cplugin_t LIBEXPORT *cplugin_load(const char *pathname)
{
    struct cplugin_function_s *flist = NULL;
    struct cplugin_s *cpl = NULL;
    cplugin_info_t *info = NULL;
    void *handle = NULL;
    cplugin_internal_data_t *plugin_idata = NULL;
    enum cplugin_plugin_type plugin_type;

    cerrno_clear();
    srandom(time(NULL) + cseed());

    if (NULL == pathname) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    if (access(pathname, 0x00) == -1) {
        cset_errno(CL_FILE_NOT_FOUND);
        return NULL;
    }

    plugin_type = guess_plugin_type(pathname);
    handle = dl_open(pathname, plugin_type);

    if (NULL == handle) {
        cset_errno(CL_DLOPEN);
        return NULL;
    }

    /* Loads plugin info */
    info = dl_load_info(handle, plugin_type);

    if (NULL == info)
        goto error_block;

    /* Transform the JSON API to a list of functions and arguments */
    flist = api_parse(info);

    if (NULL == flist)
        goto error_block;

    /*
     * Look for the real plugin exported functions (internal API) and point
     * to them so they can be called.
     */
    if (dl_load_functions(flist, handle, plugin_type) < 0)
        goto error_block;

    /* Runs the plugin initialization function */
    plugin_idata = dl_plugin_startup(handle, plugin_type);

    if (NULL == plugin_idata) {
        cset_errno(CL_PLUGIN_STARTUP);
        goto error_block;
    } else {
        /*
         * If the plugin does not have an initialization function makes
         * sure that the @idata is set to NULL.
         */
        if (plugin_idata == handle)
            plugin_idata = NULL;
    }

    cpl = new_cplugin_s();

    if (NULL == cpl)
        goto error_block;

    cpl->type = plugin_type;
    cpl->handle = handle;
    cpl->functions = flist;
    cpl->info = info;
    cpl->idata = plugin_idata;

    return cpl;

error_block:
    if (handle != NULL)
        dl_close(handle, plugin_type);

    if (flist != NULL)
        destroy_cplugin_function_s_list(flist);

    return NULL;
}

int LIBEXPORT cplugin_unload(cplugin_t *cpl)
{
    cerrno_clear();

    if (NULL == cpl) {
        cset_errno(CL_NULL_ARG);
        return -1;
    }

    /* Runs the plugin closing function */
    if (dl_plugin_shutdown(cpl) < 0) {
        cset_errno(CL_PLUGIN_SHUTDOWN);
        return -1;
    }

    if (cpl->handle != NULL)
        dl_close(cpl->handle, cpl->type);

    if (destroy_cplugin_s(cpl) < 0)
        return -1;

    return 0;
}

cplugin_internal_data_t LIBEXPORT *cplugin_get_startup_data(cplugin_t *cpl)
{
    cerrno_clear();

    if (NULL == cpl) {
        cset_errno(CL_NULL_ARG);
        return NULL;
    }

    return cpl->idata;
}

cplugin_internal_data_t LIBEXPORT
    *cplugin_get_shutdown_arg(cplugin_internal_data_t *arg)
{
    return arg;
}

