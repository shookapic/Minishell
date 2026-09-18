/*
** EPITECH PROJECT, 2023
** find_symbol
** File description:
** find_symbol
*/

#include "../include/my.h"
#include "../include/shell_struct.h"

static void exec_from_shell_path(shell_t *shell)
{
    int env_index = 0;
    int saved_errno = ENOENT;
    char **path;
    char **path_parts;

    for (; shell->env[env_index] != NULL; env_index++) {
        if (my_strncmp(shell->env[env_index], "PATH=", 5) == 0)
            break;
    }
    if (shell->env[env_index] == NULL) {
        errno = saved_errno;
        return;
    }
    path = my_str_to_word_array(shell->env[env_index], "=");
    if (path == NULL)
        return;
    path_parts = my_str_to_word_array(path[1], ":");
    if (path_parts == NULL)
        return;
    for (int index = 0; path_parts[index] != NULL; index++) {
        shell->tmp = my_strconcat(path_parts[index],
            my_strconcat("/", shell->m_cmds[0]));
        if (execve(shell->tmp, shell->m_cmds, shell->env) == -1)
            saved_errno = errno;
    }
    errno = saved_errno;
}

int all_bin_ocmd(shell_t *shell)
{
    int child;
    int signal = 0;

    child = fork();
    if (child == 0) {
        if (execve(shell->one_cmd, shell->m_cmds, shell->env) == -1) {
            execute_this_ext(shell, -1);
            exit(1);
        }
    }
    waitpid(child, &signal, 0);
    siginit(signal);
    return 0;
}

int execute_this_ocmd(shell_t *shell)
{
    int sig = 0;
    int clone = fork();

    if (clone == 0) {
        exec_from_shell_path(shell);
        if (execute_this_ext(shell, -1) == 1)
            exit(1);
    } else {
        waitpid(clone, &sig, 0);
        siginit(sig);
        return 0;
    }
    return 1;
}

int loop_condition_ocmd(shell_t *shell)
{
    function_t shell_func[] = {
        {"exit", my_exit},
        {"env", my_env},
        {"setenv", my_setenv},
        {"unsetenv", my_unsetenv},
        {"cd", my_cd},
        {NULL, NULL}
    };
    for (int i = 0; shell_func[i].str != NULL; i++) {
        if (my_strcmp(shell_func[i].str, shell->m_cmds[0]) == 0) {
            shell_func[i].fct(shell);
            free(shell->tmp);
            return 0;
        }
    }
    if (execute_this_ocmd(shell) == 1)
        return all_bin_ocmd(shell);
    return 0;
}

int find_symbol(char *str, char symbol)
{
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == symbol)
            return 1;
    }
    return 0;
}
