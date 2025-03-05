/*
 * parser.c - Command line parsing routines
 *
 * This file implements the parsing of the command line into a command_t
 * structure. Students should implement tokenization, handling of redirection
 * operators, pipelines, command substitution, and environment variable
 * expansion.
 *
 * Note: The functions `env_expand` and `command_substitute` are provided as
 * stubs. Students must complete their implementation to perform proper
 * environment variable expansion and command substitution.
 */
#include "shell.h"

/* Forward declarations for helper functions */
static char *get_next_token(char **p);
static char *substitute_token(const char *token);
static char *env_expand(const char *token);
static char *command_substitute(const char *token);

/*
 * parse_command_line - Parse the input command line string and build a
 * linked list of command_t structures representing the command and pipeline.
 *
 * This function handles:
 *   - Tokenization of the command line, including handling quoted strings.
 *   - Detection of background execution (ampersand '&').
 *   - Input redirection using '<' and output redirection using '>' or '>>'
 *     (with append mode support for '>>').
 *   - A single pipeline (using the '|' token) by creating a linked list of
 *     command_t structures.
 *   - For each normal token, call substitute_token() to perform environment
 *     variable expansion and command substitution.
 *
 * Returns 0 on success, or a nonzero value on error.
 */
int parse_command_line(const char *cmdline, command_t **cmd, int *bg) {
  char *line_copy = strdup(cmdline); // Make a modifiable copy of the input
  if (!line_copy) {
    return -1;
  }
  char *p = line_copy;
  command_t *head = malloc(sizeof(command_t));
  if (!head) {
    free(line_copy);
    return -1;
  }
  memset(head, 0, sizeof(command_t));
  *bg = 0;
  head->append = 0;
  head->next = NULL;

  command_t *current = head;

  while (1) {
    // Skip leading whitespace
    while (*p && isspace((unsigned char)*p)) {
      p++;
    }
    if (*p == '\0' || *p == '\n') {
      break; // End of line
    }

    // Get the next token (allocated string)
    char *token = get_next_token(&p);
    if (!token) {
      break;
    }

    // Process special tokens
    if (strcmp(token, "<") == 0) {
      free(token);
      // Next token should be the input file
      token = get_next_token(&p);
      if (!token) {
        fprintf(stderr, "Error: no input file specified\n");
        free(line_copy);
        return -1;
      }
      current->infile = substitute_token(token);
      free(token);
    } else if ((strcmp(token, ">") == 0) || (strcmp(token, ">>") == 0)) {
      int isAppend = (strcmp(token, ">>") == 0) ? 1 : 0;
      free(token);
      // Next token should be the output file
      token = get_next_token(&p);
      if (!token) {
        fprintf(stderr, "Error: no output file specified\n");
        free(line_copy);
        return -1;
      }
      current->outfile = substitute_token(token);
      current->append = isAppend;
      free(token);
    } else if (strcmp(token, "|") == 0) {
      free(token);
      // Start a new command in the pipeline
      current->next = malloc(sizeof(command_t));
      if (!current->next) {
        free(line_copy);
        return -1;
      }
      current = current->next;
      memset(current, 0, sizeof(command_t));
      current->append = 0;
      current->next = NULL;
    } else if (strcmp(token, "&") == 0) {
      free(token);
      // Mark the background flag
      *bg = 1;
      // Typically, '&' should be at the end. Continue parsing if any tokens
      // remain.
    } else {
      // Normal argument: process substitutions.
      char *processed = substitute_token(token);
      free(token);
      if (current->argc < MAXARGS - 1) {
        current->argv[current->argc++] = processed;
      } else {
        fprintf(stderr, "Error: too many arguments\n");
        free(processed);
        free(line_copy);
        return -1;
      }
    }
  }

  // Terminate the argv array for each command in the pipeline
  for (command_t *cmd_iter = head; cmd_iter != NULL;
       cmd_iter = cmd_iter->next) {
    cmd_iter->argv[cmd_iter->argc] = NULL;
  }

  free(line_copy);
  *cmd = head;
  return 0;
}

/*
 * get_next_token - Retrieve the next token from the input string.
 *
 * This function skips leading whitespace and then extracts a token.
 * A token is defined as:
 *   - A sequence of non-whitespace characters, or
 *   - A quoted string (using single or double quotes) which may contain spaces.
 *   - A special token if the current character is one of: '<', '>', '|', '&'.
 *     For '>', it also checks for an appended '>' to form ">>".
 *
 * The function returns a newly allocated string containing the token.
 * The caller is responsible for freeing the returned token.
 */
static char *get_next_token(char **p) {
  while (**p && isspace((unsigned char)**p)) {
    (*p)++;
  }
  if (**p == '\0' || **p == '\n') {
    return NULL;
  }

  char *start = *p;
  char *token = NULL;

  // Check for quoted token
  if (**p == '\'' || **p == '\"') {
    char quote = **p;
    (*p)++; // Skip the opening quote
    start = *p;
    while (**p && **p != quote) {
      (*p)++;
    }
    size_t len = *p - start;
    token = malloc(len + 1);
    if (!token)
      return NULL;
    strncpy(token, start, len);
    token[len] = '\0';
    if (**p == quote) {
      (*p)++; // Skip closing quote
    }
  }
  // Check for special characters
  else if (**p == '<' || **p == '|' || **p == '&' || **p == '>') {
    if (**p == '>') {
      // Check if it is a ">>" for append mode
      if (*(*p + 1) == '>') {
        token = strdup(">>");
        *p += 2;
        return token;
      }
    }
    token = malloc(2);
    if (!token)
      return NULL;
    token[0] = **p;
    token[1] = '\0';
    (*p)++;
  }
  // Normal unquoted token
  else {
    while (**p && !isspace((unsigned char)**p) && **p != '<' && **p != '>' &&
           **p != '|' && **p != '&') {
      (*p)++;
    }
    size_t len = *p - start;
    token = malloc(len + 1);
    if (!token)
      return NULL;
    strncpy(token, start, len);
    token[len] = '\0';
  }

  return token;
}

/*
 * substitute_token - Process a token for environment variable expansion and
 * command substitution.
 *
 * If the token starts with "$(" and ends with ")", perform command
 * substitution. If the token starts with '$', perform environment variable
 * expansion. Otherwise, return a duplicate of the token.
 *
 * Returns a newly allocated string with the substitution performed.
 */
static char *substitute_token(const char *token) {
  if (!token)
    return NULL;
  if (strncmp(token, "$(", 2) == 0 && token[strlen(token) - 1] == ')') {
    return command_substitute(token);
  }
  if (token[0] == '$') {
    return env_expand(token);
  }
  return strdup(token);
}

/*
 * env_expand - Stub for environment variable expansion.
 *
 * The token is expected to be of the form "$VAR". Students should implement
 * the expansion by retrieving the environment variable and returning its value.
 */
static char *env_expand(const char *token) {
  // TODO: Implement environment variable expansion.
  // For now, simply return a duplicate of the token.
  return strdup(token);
}

/*
 * command_substitute - Stub for command substitution.
 *
 * The token is expected to be of the form "$(command)". Students should
 * implement command substitution by executing the command and capturing its
 * output.
 */
static char *command_substitute(const char *token) {
  // TODO: Implement command substitution.
  // For now, simply return a duplicate of the token.
  return strdup(token);
}
